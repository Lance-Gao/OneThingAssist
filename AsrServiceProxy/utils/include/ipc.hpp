#ifndef UTILS_IPC_HPP
#define UTILS_IPC_HPP

#include <sys/types.h>
#include <sys/ipc.h>
#include <string>

typedef enum _ipc_role {
    ROLE_SERVER = 0,
    ROLE_CLIENT
} ipc_role_t;

typedef struct _shm_ipc {
    std::string name;
    int shm_size;
    void* shm_ptr;
    int sem_id;
    int shm_id;
    int snd_timeout_in_ms;
    int rec_timeout_in_ms;
} shm_ipc_t;

typedef enum _shm_msg_type {
    TYPE_SEND,
    TYPE_POST,
} shm_msg_type_t;

typedef struct _msg_header {
    char msg_id[37]; 
    shm_msg_type_t type;
    int size;
} msg_header_t;

// get a global unique id
// keyname:  name of the key
key_t shm_getkey(const char* keyname, int idx);

int init_shm_ipc(const char* shm_name, ipc_role_t role, shm_ipc_t* shm, int size);

void set_shm_ipc_timeout(shm_ipc_t* shm, int snd_timeout, int rec_timeout);

int pst_msg(shm_ipc_t* shm, void* msg_data, int msg_size);

int snd_msg(shm_ipc_t* shm, void* msg_data, int msg_size, void** response, int* response_size);

int rec_msg(shm_ipc_t* shm, msg_header_t& msg_hdr, void** response, int* response_size);

int ack_msg(shm_ipc_t* shm, msg_header_t& msg_hdr, void* msg_data, int msg_size);

#endif // UTILS_IPC_HPP