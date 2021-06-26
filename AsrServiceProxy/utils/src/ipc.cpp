#include "include/ipc.hpp"
#include <string>
#include "include/aip_log.hpp"
#include "include/utils.hpp"

extern "C" {
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
}

enum {
    DEFAULT_SHM_TIMEOUT = 1000,
};

enum {
    SEM_LOCK = 0,
    SEM_CLIENT_MSG = 1,
    SEM_SERVER_MSG = 2,
    SEM_COUNT = 3
};

key_t shm_getkey(const char* keyname, int idx) {
    std::string key_path("./keys/");
    key_path += keyname;
    
    if (!FileUtil::touch(key_path.c_str())) {
        AIP_LOG_FATAL("create key file failed %s", key_path.c_str());
        return -1;
    }
    return ftok(key_path.c_str(), idx);
}

int init_shm_ipc(const char* shm_name, ipc_role_t role, shm_ipc_t* shm, int size) {
    std::string name_prefix(shm_name);
    std::string shm_key_file = name_prefix + "_shm";
    std::string sem_key_file = name_prefix + "_sem";

    key_t shm_key = shm_getkey(shm_key_file.c_str(), 0);
    key_t sem_key = shm_getkey(sem_key_file.c_str(), 0);

    int shm_flag = 0;
    // create things only at server side
    if (role == ROLE_SERVER) {
        shm_flag = 0666 | IPC_CREAT;
    } else {
        size = 0;
    }

    shm->shm_id = shmget(shm_key, size, shm_flag);

    if (shm->shm_id < 0) {
        AIP_LOG_FATAL("shmget failed;(%s)", shm_name);
        return -1;
    }

    shm->shm_ptr = shmat(shm->shm_id, NULL, 0);

    if (shm->shm_ptr == (void*)(-1)) {
        AIP_LOG_FATAL("shmat failed;(%s)", shm_name);
        return -1;
    }
    shm->name = shm_name;
    shm->shm_size = size;
    shm->snd_timeout_in_ms = DEFAULT_SHM_TIMEOUT;
    shm->rec_timeout_in_ms = DEFAULT_SHM_TIMEOUT;

    shm->sem_id = semget(sem_key, SEM_COUNT, shm_flag);

    short val = 1;
    int ret = semctl(shm->sem_id, 0, SETVAL, val);
    if (ret < 0) {
        AIP_LOG_FATAL("segctl failed;(%s)", shm_name);
        return -1;
    }
    if (shm->sem_id < 0) {
        AIP_LOG_FATAL("semget failed;(%s)", shm_name);
        return -1;
    }

    AIP_LOG_NOTICE("init_shm_pic finished;(%s)", shm_name);
    return 0;
}

void set_shm_ipc_timeout(shm_ipc_t* shm, int snd_timeout, int rec_timeout) {
    shm->snd_timeout_in_ms = snd_timeout;
    shm->rec_timeout_in_ms = rec_timeout;
}

static int semp(int semid, int semnum, int undo, int nowait, int timeout) {
    int flag  = 0;
    if (undo) {
        flag |= SEM_UNDO;
    }
    if (nowait) {
        flag |= IPC_NOWAIT;
    }
    struct sembuf sops = {semnum, -1, flag};
    struct timespec ts = {timeout / 1000, (timeout % 1000) * 1000000};
    return (semtimedop(semid, &sops, 1, timeout > 0 ? &ts : NULL));
}

static int semv(int semid, int semnum, int undo) {
    int flag = 0;
    if (undo) {
        flag |= SEM_UNDO;
    }
    
    struct sembuf sops = {semnum, +1, SEM_UNDO};
    return (semop(semid, &sops, 1));
}

int pst_msg(shm_ipc_t* shm, void* msg_data, int msg_size) {
    int ret = -1;
    if (semp(shm->shm_id, SEM_LOCK, 1, 0, 
        shm->snd_timeout_in_ms) == 0) {
        // clear semaphores
        while (0 == semp(shm->shm_id, SEM_SERVER_MSG, 0, 1, -1)) {
        }

        // write msg
        msg_header_t header = {TYPE_POST, msg_size};        
        random_uuid(header.msg_id);
        
        memcpy(shm->shm_ptr, &header, sizeof(msg_header_t));
        memcpy(shm->shm_ptr + sizeof(msg_header_t), msg_data, msg_size);
        
        // tell server 
        semv(shm->shm_id, SEM_CLIENT_MSG, 0);
        
        // wait for ack
        if (0 != semp(shm->shm_id, SEM_SERVER_MSG, 0, 0, shm->rec_timeout_in_ms)) {
            AIP_LOG_WARNING("shm pst msg timedout for ack");
        } else {
            ret = 0;
        }
        
        // unlock
        semv(shm->shm_id, SEM_LOCK, 1);        
        return ret;
    } else {
        AIP_LOG_WARNING("shm pst msg failed");
        return ret;
    }
}

int snd_msg(shm_ipc_t* shm, void* msg_data, int msg_size, void** response, int* response_size) {
    int ret = -1;
    if (semp(shm->shm_id, SEM_LOCK, 1, 0,
        shm->snd_timeout_in_ms) == 0) {
        // clear semaphores
        while (0 == semp(shm->shm_id, SEM_SERVER_MSG, 0, 1, -1)) {
        }

        // write msg
        msg_header_t header = {"", TYPE_SEND, msg_size};
        random_uuid(header.msg_id);
        
        memcpy(shm->shm_ptr, &header, sizeof(msg_header_t));
        memcpy(shm->shm_ptr + sizeof(msg_header_t), msg_data, msg_size);
        
        // tell server 
        semv(shm->shm_id, SEM_CLIENT_MSG, 0);
        
        while (true) {
            // wait for ack
            if (0 != semp(shm->shm_id, SEM_SERVER_MSG, 0, 0, shm->rec_timeout_in_ms)) {
                AIP_LOG_WARNING("shm snd msg timedout for ack");
                ret = -1;
                break;
            } else {
                msg_header_t res_header;
                memcpy(&res_header, shm->shm_ptr, sizeof(msg_header_t));
                if (strncmp(res_header.msg_id, header.msg_id, sizeof(header.msg_id)) == 0) {
                    *response_size = res_header.size;
                    *response = malloc(res_header.size);
                    
                    if (*response) {
                        memcpy(*response, shm->shm_ptr + sizeof(msg_header_t), res_header.size);
                        ret = 0;
                    } else {
                        ret = -1;
                        AIP_LOG_FATAL("alloc memory failed, alloc size %d", header.size);
                    }
                    break;
                } else {
                    ret = -1;
                    AIP_LOG_WARNING("ack not for me");
                }
            }            
        }
        
        // unlock
        semv(shm->shm_id, SEM_LOCK, 1);
        return ret;
    } else {
        AIP_LOG_WARNING("shm snd msg failed");
        return ret;
    }
}

int rec_msg(shm_ipc_t* shm, msg_header_t& msg_hdr, void** request, int* request_size) {
    int ret = -1;
    // wait for client msg
    if (0 == semp(shm->shm_id, SEM_CLIENT_MSG, 0, 0, shm->rec_timeout_in_ms)) {
        memcpy(&msg_hdr, shm->shm_ptr, sizeof(msg_header_t));
        *request_size = msg_hdr.size;
        *request = malloc(msg_hdr.size);
                    
        if (*request) {
            memcpy(*request, shm->shm_ptr + sizeof(msg_header_t), msg_hdr.size);
            ret = 0;
        } else {
            ret = -1;
            AIP_LOG_FATAL("alloc memory failed, alloc size %d", msg_hdr.size);
        }

        if (msg_hdr.type == TYPE_POST) {
            semv(shm->shm_id, SEM_SERVER_MSG, 0);
        }
    }
    return ret;
}

int ack_msg(shm_ipc_t* shm, msg_header_t& msg_hdr, void* msg_data, int msg_size) {
    memcpy(shm->shm_ptr, &msg_hdr, sizeof(msg_header_t));
    memcpy(shm->shm_ptr + sizeof(msg_header_t), msg_data, msg_size);
    // tell client 
    semv(shm->shm_id, SEM_SERVER_MSG, 0);
    return 0;
}