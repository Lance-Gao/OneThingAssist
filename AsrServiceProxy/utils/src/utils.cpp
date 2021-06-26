#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include "include/utils.hpp"
#include <sys/time.h>
#include <errno.h>
#include "include/aip_log.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <sys/prctl.h>
#include <signal.h>
#include "base64.hpp"

int StringUtil::split(const std::string& origin, 
                      char ch_delimiter,  std::vector<std::string>* result) {
    result->clear();

    // split heards through major delimiter
    size_t pos = 0;
    std::string token;
    std::string delimitor;
    std::string str(origin);

    delimitor.push_back(ch_delimiter);

    while ((pos = str.find(delimitor)) != std::string::npos) {
        token.empty();
        if (pos >= 0) {
            token = str.substr(0, pos);    
        }
        result->push_back(token);
        str.erase(0, pos + delimitor.length());
    }

    result->push_back(str);

    return 0;
}

int StringUtil::replace(std::string& src, char s, char d) {
    int counter = 0;
    for (std::string::iterator it = src.begin(); 
        it != src.end();
        it++) {
        if (*it == s) {
            counter++;
            *it = d;
        }
    }
    return counter;
}

int StringUtil::to_bool(const char* sval) {
    static const char* s_trues[] = {"yes", "y", "sure", "true", "t", "ok", "1", NULL};
    std::string ss(sval);
    ss = space_trim(ss);
    std::string lval;
    to_lower(ss, &lval);
    
    const char** idx = &s_trues[0];
    
    while (*idx != NULL) {
        if (strcmp(*idx, lval.c_str()) == 0) {
            return 1;
        }
        idx++;
    }
    
    return !!atoi(lval.c_str());
}

int StringUtil::split(const std::string& str, char delimiter,
                      std::set<std::string>* result) {
    result->clear();
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        result->insert(item);
    }

    return 0;
}

int StringUtil::split(const std::string& str,
                      char major_delimiter, char minor_delimiter,
                      std::map<std::string, std::string>* result,
                      bool accept_minor_delim_in_value, bool truncate_result) {
    if (truncate_result) {
        result->clear();
    }

    std::vector<std::string> major_result;

    if (split(str, major_delimiter, &major_result)) {
        return -1;
    }

    for (uint32_t i = 0u; i < major_result.size(); i++) {
        std::vector<std::string> tmp;
        std::string& line = major_result[i];
        std::string name;
        std::string value;

        if (split(line, minor_delimiter, &tmp)) {
            return -1;
        }

        if (tmp.size() == 0) {
            return -1;
        }

        name = space_trim(tmp[0]);

        if (tmp.size() == 1) {
            value = "";
        } else if (tmp.size() == 2) {
            value = space_trim(tmp[1]);
        } else {
            if (!accept_minor_delim_in_value) {
                return -1;
            }

            value = space_trim(line.substr(line.find(minor_delimiter) + 1));
        }

        result->insert(std::map<std::string, std::string>::value_type(name, value));
    }

    return 0;
}

std::string StringUtil::space_trim(const std::string& src) {
    int start_pos = 0;
    int length = src.size();

    for (std::string::const_iterator it = src.begin(); it != src.end(); it++) {
        if (!isspace(*it) /*&& !isblank(*it)*/) {
            break;
        }

        start_pos++;
        length--;
    }

    for (std::string::const_reverse_iterator it = src.rbegin(); it != src.rend(); it++) {
        if (!isspace(*it) /*&& !isblank(*it)*/) {
            break;
        }

        length--;
    }

    return src.substr(start_pos, length);
}

std::string StringUtil::int64_to_str(const int64_t ival) {
    std::stringstream sstream;
    sstream << ival;
    return sstream.str();
}

bool StringUtil::is_empty(const std::string& src) {
    const char* idx = src.c_str();

    int length = src.size();

    if (length == 0) {
        return true;
    }

    while (*idx) {
        if (!isspace(*idx & 0x7f)) {
            return false;
        }

        idx++;
    }

    return true;
}

int StringUtil::to_lower(const std::string& src, std::string* lower) {
    lower->assign(src);
    std::transform(src.begin(), src.end(), lower->begin(), tolower);

    return 0;
}

int StringUtil::to_upper(const std::string& src, std::string* upper) {
    upper->assign(src);
    std::transform(src.begin(), src.end(), upper->begin(), toupper);

    return 0;
}

std::string StringUtil::uri_encode(const std::string& input, bool encode_slash) {
    std::stringstream ss;
    const char* str = input.c_str();

    for (uint32_t i = 0; i < input.size(); i++) {
        unsigned char c = str[i];

        if (isalnum(c) || c == '_' || c == '-' || c == '~' || c == '.') {
            ss << c;
        } else if (c == '/') {
            if (encode_slash) {
                ss << "%" << to_hex(c);
            } else {
                ss << c;
            }
        } else {
            ss << "%" << to_hex(c);
        }
    }

    return ss.str();
}

int StringUtil::uri_decode(const std::string& input, std::string* out) {
    const unsigned char* str = reinterpret_cast<const unsigned char*>(input.c_str());

    const int NORMALSTATE = 0;
    const int PERCENTAPPEARSTATE = 1;
    const int PERCENTANDHEXCHARAPPEARSTATE = 2;
    const int ERRORSTATE = 3;

    int state = NORMALSTATE;
    uint32_t iter = 0;
    std::stringstream ss;

    while (true) {
        if (iter >= input.size()) {
            break;
        }

        if (state == ERRORSTATE) {
            break;
        }

        unsigned char c = str[iter];
        unsigned char o = 0;

        switch (state) {
        case NORMALSTATE:
            if (c == '%') {
                state = PERCENTAPPEARSTATE;
            } else {
                if (isalnum(c) || c == '.' || c == '~' || c == '-' || c == '_' || c == '/') {
                    ss << c;
                } else {
                    state = ERRORSTATE;
                }
            }

            break;

        case PERCENTAPPEARSTATE:
            if (is_hex_char(c)) {
                o = to_bin(c) << 4;
                state = PERCENTANDHEXCHARAPPEARSTATE;
            } else {
                state = ERRORSTATE;
            }

            break;

        case PERCENTANDHEXCHARAPPEARSTATE:
            if (is_hex_char(c)) {
                o += to_bin(c);
                ss << o;
                state = NORMALSTATE;
            } else {
                state = ERRORSTATE;
            }

            break;

        default:
            state = ERRORSTATE;
            break;
        }

        iter++;
    }

    if (state != NORMALSTATE) {
        return -1;
    }

    out->assign(ss.str());

    return 0;
}

std::string StringUtil::to_hex(unsigned char c) {
    const char* hex = "0123456789ABCDEF";
    std::string result;
    result.reserve(2);
    result.push_back(hex[c >> 4]);
    result.push_back(hex[c & 0xf]);

    return result;
}

std::string StringUtil::to_hex(const std::string& src) {
    const char* hex = "0123456789ABCDEF";
    const unsigned char* c_str = reinterpret_cast<const unsigned char*>(src.c_str());

    std::stringstream ss;

    for (uint32_t i = 0; i < src.size(); i++) {
        unsigned char c = c_str[i];
        ss << hex[c >> 4] << hex[c & 0xf];
    }

    return ss.str();
}

std::string StringUtil::to_upper(const std::string& src) {
    std::string upper;
    to_upper(src, &upper);
    return upper;
}

std::string StringUtil::to_lower(const std::string& src) {
    std::string lower;
    to_lower(src, &lower);
    return lower;
}

std::string StringUtil::trim(const std::string& src) {
    const char* c_str = src.c_str();

    int start = 0;
    int end = src.size() - 1;
    int length = src.size();

    for (; start < length; start++) {
        if ((c_str[start] & 0x80) || !isspace(c_str[start])) {
            break;
        }
    }

    for (; end >= start; end--) {
        if ((c_str[end] & 0x80) || !isspace(c_str[end])) {
            break;
        }
    }

    if (start > end) {
        return "";
    }

    return std::string(c_str + start, end - start + 1);
}

std::string StringUtil::sha256_hex(const std::string& src, const std::string& sk) {
    const EVP_MD* evp_md = EVP_sha256();
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int md_len = 0;

    if (HMAC(evp_md,
             reinterpret_cast<const unsigned char*>(sk.data()), sk.size(),
             reinterpret_cast<const unsigned char*>(src.data()), src.size(),
             md, &md_len) == NULL) {
        return "";
    }

    return to_lower(to_hex(std::string(reinterpret_cast<char*>(md), md_len)));
}

int StringUtil::iconv(const std::string& src, const std::string& from_encode,
                      const std::string& to_encode, std::string* dest) {
    dest->assign(src);
    return 0;
}

bool StringUtil::is_hex_char(unsigned char c) {
    if ('0' <= c && c <= '9') {
        return true;
    }

    if ('a' <= c && c <= 'f') {
        return true;
    }

    if ('A' <= c && c <= 'F') {
        return true;
    }

    return false;
}

unsigned char StringUtil::to_bin(unsigned char c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    }

    if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    }

    if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    }

    return 0xff;
}

void TimeUtil::init() {
    time_t now = time(NULL);

    struct tm utc_now_tm;
    gmtime_r(&now, &utc_now_tm);
    char buffer[128];
    int size = strftime(buffer, 128, kAipUtcTimeFormat, &utc_now_tm);

    struct tm local_now_tm;
    strptime(buffer, kAipUtcTimeFormat, &local_now_tm);

    _s_utc_local_time_offset_in_seconds = (int32_t)(mktime(&local_now_tm) - now);
}

int64_t TimeUtil::now_ms() {
#ifdef _WIN32
    _timeb the_time  = {0};
    _ftime_s(&the_time);
    return the_time.time * 1000LL + the_time.millitm;
#else
    struct timeval now;
    gettimeofday(&now, NULL);

    return now.tv_sec * 1000LL + now.tv_usec / 1000;
#endif
}

time_t TimeUtil::now() {
    return time(NULL);
}

std::string TimeUtil::now_utc_time() {
    return timestamp_to_utc_time(now());
}

std::string TimeUtil::now_local_time() {
    time_t now_time = now();
    struct tm *result_tm = localtime(&now_time);
    char buffer[128];
    int size = strftime(buffer, 128, kAipUtcTimeFormat, result_tm);
    return std::string(buffer, size);
}

std::string TimeUtil::now_local_datetime() {
    time_t now_time = now();
    struct tm *result_tm = localtime(&now_time);
    char buffer[128];
    int size = strftime(buffer, 128, kAipUtcTimeAnoFormat, result_tm);
    return std::string(buffer, size);
}

std::string TimeUtil::timestamp_to_utc_time(time_t timestamp) {
    struct tm result_tm;
    gmtime_r(&timestamp, &result_tm);

    char buffer[128];
    int size = strftime(buffer, 128, kAipUtcTimeFormat, &result_tm);

    return std::string(buffer, size);
}

int64_t TimeUtil::utc_time_to_timestamp(const std::string& utc_time) {
    struct tm result_tm;

    if (strptime(utc_time.c_str(), kAipUtcTimeFormat, &result_tm)
            != utc_time.c_str() + kAipUtcTimeFormatLength) {
        return -1;
    }

    return mktime(&result_tm) - _s_utc_local_time_offset_in_seconds;
}
int TimeUtil::utc_time_to_tm(const std::string& utc_time, struct tm* _tm) {
    if (strptime(utc_time.c_str(), kAipUtcTimeFormat, _tm) 
                 != utc_time.c_str() + kAipUtcTimeFormatLength) {
        memset(_tm, 0, sizeof(struct tm));
        return 0;
    }
    time_t timestamp = mktime(_tm) - _s_utc_local_time_offset_in_seconds;

    gmtime_r(&timestamp, _tm);
    
    return 1;
}

int32_t TimeUtil::_s_utc_local_time_offset_in_seconds = 0;

class TimeUtilInitializer {
public:
    TimeUtilInitializer() {
        TimeUtil::init();
    }
};

TimeUtilInitializer g_time_util_initializer_obj;

bool FileUtil::exists(const char* path) {
    struct stat _stat;
    if (stat(path, &_stat) == 0) {
        return true;
    } else {
        AIP_LOG_WARNING("stat failed for '%s'[errno:%d]", path, errno);
        return false;
    }
}

bool FileUtil::is_dir(const char* path) {
    struct stat _stat;
    if (stat(path, &_stat) == 0) {
        return S_ISDIR(_stat.st_mode);
    } else {
        AIP_LOG_WARNING("stat failed for '%s'[errno:%d]", path, errno);
        return false;
    }
}

bool FileUtil::is_file(const char* path) {
    struct stat _stat;
    if (stat(path, &_stat) == 0) {
        return S_ISREG(_stat.st_mode);
    } else {
        AIP_LOG_WARNING("stat failed for '%s'[errno:%d]", path, errno);
        return false;
    }  
}

bool FileUtil::touch(const char* path) {
    std::string str_path(path);
    StringUtil::replace(str_path, '\\', '/');
    
    std::vector<std::string> results;
    StringUtil::split(str_path, '/',  &results);

    std::string name = results.back();
    if (StringUtil::is_empty(name)) {
        AIP_LOG_WARNING("touch file name is empty");
        return false;
    }
    
    results.pop_back();
    
    std::string dir;
    bool first_part = true;
    if (results.size() > 0) {
        for (std::vector<std::string>::const_iterator it = results.begin();
        it != results.end(); 
        it++) {
            if (!first_part && it->size() == 0) {
                first_part = false;
                continue;
            } 
            first_part = false;
            
            dir += *it;
            dir += "/";
        }
    }
    if (dir.size() > 0) {
        if (!create_dir_if_not_exist(dir.c_str())) {
            AIP_LOG_FATAL("create directory failed: %s", dir.c_str());
            return false;
        }
    }
    
    dir += name;
    
    FILE* f = fopen(dir.c_str(), "w+");
    if (f != NULL) {
        fclose(f);
        return true;
    } else {
        AIP_LOG_FATAL("create file failed: %s[errno:%d]", dir.c_str(), errno);
        return false;
    }
}

std::string FileUtil::normalize_file_path(const char* path) {
    std::string str_path(path);
    StringUtil::replace(str_path, '\\', '/');
    
    std::vector<std::string> results;
    StringUtil::split(str_path, '/',  &results);
    
    bool first_part = true;
    std::string dir;
    if (results.size() > 0) {
        for (std::vector<std::string>::const_iterator it = results.begin();
            it != results.end(); 
            it++) {
            if (!first_part && it->size() == 0) {
                first_part = false;
                continue;
            } 
            first_part = false;
                
            dir += *it;
            dir += "/";
        }
    }
    
    if (dir.size() > 0) {
        dir.erase(dir.size() - 1);
    }
    return dir;    
}

bool FileUtil::create_dir_if_not_exist(const char* path) {
    if (FileUtil::exists(path)) {
        if (FileUtil::is_dir(path)) {
            return true;
        } else {
            AIP_LOG_FATAL("%s is not a directory", path);
            return false;
        }
    }
    
    std::string str_path(path);
    StringUtil::replace(str_path, '\\', '/');
    
    std::vector<std::string> results;
    StringUtil::split(str_path, '/',  &results);
    
    while (results.size() > 0 && StringUtil::is_empty(results.back())) {
        results.pop_back();
    }
    
    if (results.size() > 0) {
        std::string dir_name = results.back();
        results.pop_back();
        
        std::string dir;
        bool first_part = true;
        if (results.size() > 0) {
            for (std::vector<std::string>::const_iterator it = results.begin();
            it != results.end(); 
            it++) {
                if (!first_part && it->size() == 0) {
                    first_part = false;
                    continue;
                } 
                first_part = false;
                
                dir += *it;
                dir += "/";
            }
        }
        
        if (dir.size() > 0) {
            if (!create_dir_if_not_exist(dir.c_str())) {
                AIP_LOG_FATAL("create directory failed: %s", dir.c_str());
                return false;
            }
        }

        dir += dir_name;

        if (mkdir(dir.c_str(), 0755) != 0) {
            AIP_LOG_FATAL("create directory failed: %s", dir.c_str());
            return false;
        }
        return true;
    } else {
        return true;
    }
}

char* random_uuid(char buf[37]) {
    std::stringstream ss;
    const boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
    ss << a_uuid;
    snprintf(buf, 37, "%s", ss.str().c_str());
    return buf;
}

void encode_img_to_base64(cv::Mat img, std::string &dst, int quality) {
    std::vector<uchar> data_encode;
    std::vector<int> params;

    params.push_back(CV_IMWRITE_JPEG_QUALITY);
    params.push_back(quality);

    cv::imencode(".jpg", img, data_encode, params);
    std::string str_jpg(data_encode.begin(), data_encode.end());
    base64_encode((unsigned char const *) str_jpg.c_str(), str_jpg.size(), dst);
    str_jpg.clear();
}

int registe_sighup_handle(handle_sig_pfunc func) {
    signal(SIGHUP, func);
    prctl(PR_SET_PDEATHSIG, SIGHUP);

    return 0;
}

int registe_sigterm_handle(handle_sig_pfunc func) {
    signal(SIGTERM, func);

    return 0;
}

int registe_sigusr1_handle(handle_sig_pfunc func) {
    signal(SIGUSR1, func);

    return 0;
}

int registe_sigusr2_handle(handle_sig_pfunc func) {
    signal(SIGUSR2, func);

    return 0;
}
