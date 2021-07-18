#ifndef UTILS_UTILS_HPP
#define UTILS_UTILS_HPP

#include "opencv2/imgproc.hpp"
#include "aip_common.hpp"

#define kAipUtcTimeFormat "%Y-%m-%d %H:%M:%S"
#define kAipUtcTimeFormatLength 20

#define kAipUtcTimeAnoFormat "%Y%m%d%H%M%S"

#define kAipRequestHeaderPrefix "x-aip-"
#define kAipRequestHeaderPrefixLength 6

class StringUtil {
public:
    static int split(const std::string &str, char delimiter,
            std::vector<std::string> *result);

    static int split(const std::string &str, char delimiter,
            std::set<std::string> *result);

    static int split(const std::string &str,
            char major_delimiter, char minor_delimiter,
            std::map<std::string, std::string> *result,
            bool accept_minor_delim_in_value = false,
            bool truncate_result = true);

    static int to_lower(const std::string &src, std::string *lower);
    static int to_upper(const std::string &src, std::string *lower);

    static std::string uri_encode(const std::string &input, bool encode_slash = true);
    static int uri_decode(const std::string &input, std::string *out);
    static std::string to_hex(unsigned char c);
    static std::string to_hex(const std::string &src);
    static std::string to_upper(const std::string &src);
    static std::string to_lower(const std::string &src);
    static std::string trim(const std::string &src);
    static std::string space_trim(const std::string &src);
    static std::string int64_to_str(const int64_t ival);
    static bool is_empty(const std::string &src);
    static int to_bool(const char* sval);
    static int replace(std::string& src, char s, char d);

    static std::string sha256_hex(const std::string &src, const std::string &sk);

    static int iconv(const std::string &src, const std::string &from_encode,
            const std::string &to_encode, std::string *dest);

    static bool is_hex_char(unsigned char c);
    static unsigned char to_bin(unsigned char c);
};

template<typename T> std::string to_string(T t) {
    std::stringstream s;
    s << t;
    return s.str();
}

class TimeUtil {
public:
    static void init();
    static int64_t now_ms();
    static time_t now();

    static std::string now_local_datetime();
    static std::string now_local_time();
    static std::string now_utc_time();
    static std::string timestamp_to_utc_time(time_t timestamp);
    static int64_t utc_time_to_timestamp(const std::string &utc_time);
    static int utc_time_to_tm(const std::string& utc_time, struct tm* _tm);

private:
    static int32_t _s_utc_local_time_offset_in_seconds;
};

class FileUtil {
public:
    static bool exists(const char* path);
    static bool is_dir(const char* path);
    static bool is_file(const char* path);
    static bool touch(const char* path);
    static bool create_dir_if_not_exist(const char* path);
    static std::string normalize_file_path(const char* path);
};

#define DECLARE_REFERENCE_COUNT \
public: \
    int refs() const { return _refs; } \
    void ref() { AtomicInc(_refs); } \
    void unref() { \
        int n = AtomicDec(_refs); \
        assert(n >= 0); \
        if (n <= 0) { \
            delete this; \
        } \
    } \
private: \
    volatile int _refs;
    
    
char* random_uuid(char buf[37]);
void encode_img_to_base64(cv::Mat img, std::string &dst, int quality);

typedef void (*handle_sig_pfunc)(int);
int registe_sighup_handle(handle_sig_pfunc func);
int registe_sigterm_handle(handle_sig_pfunc func);
int registe_sigusr1_handle(handle_sig_pfunc func);
int registe_sigusr2_handle(handle_sig_pfunc func);

#endif // UTILS_UTILS_HPP
