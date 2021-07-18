#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP

#include <map>
#include <string>
#include <iostream>

typedef struct _conf_recognize {
    std::string mask;
    std::string recognize_server;
    std::string face_trail_server;
    std::string worker_server;
    int times_per_face;
} conf_recognize_t;

typedef struct _conf_recording {
    int is_recording;
    std::string location;
    long max_duration_in_seconds;
    int hls_slice_duration_in_seconds;
} conf_recording_t;

typedef struct _conf_publish {
    int is_publish;
    std::string publish_url;
} conf_publish_t;

typedef struct _int_range {
    int low;
    int high;
} int_range_t;

typedef struct _conf_face_quality {
    int minimum_face_in_pixels;
    int_range_t pitch_threshold;
    int_range_t yaw_threshold;
    int_range_t roll_threshold;
    float first_blurry_threshold;
    float blurry_threshold;
    float bright_threshold;
    float occlusion_threshold;
    float confidence_threshold;
    int min_frame_interval;
    int time_to_wait_in_ms;
    float size_weight;
    float angle_weight;
} conf_face_quality_t;

typedef struct _conf_push_img {
    std::string push_img_server;
    long duration_in_mseconds;
    float img_resize_ratio;
    int img_quality;
} conf_push_img_t;

class Config {
public:
    Config();
    ~Config();

    const std::string& get_app_key();
    const std::string& get_appsecret_key();
    const std::string& get_asrapi_source();
    const std::string& get_asr_server();
    const std::string& get_audio_format();
    int get_audio_type();
    const std::string& get_capacity_scope();
    int get_concurrent_number();
    const char *get_command_line_help();
    bool is_enable_asr_service();
    int load_from_file(const char *file);
    int load_command_line(char *const *argv, int argc);
    const char *ensure_valid_string(char *input);
    int string_to_int(const char *input);
    float string_to_float(const char *input);
    int parse_range(const char *input, int *low, int *high);
    char *clean_opt(char *opt);
    std::string to_string();
    int check_server_uri();
    const std::string &get_working_dir();

private:
    void init_dir();
    void set_app_key(const char* optarg);
    void set_appsecret_key(const char* optarg);
    void set_asrapi_source(const char* optarg);
    void set_asr_server(const char* optarg);
    void set_audio_format(const char* optarg);
    void set_audio_type(const char* optarg);
    void set_capacity_scope(const char* optarg);
    void set_concurrent_number(const char* optarg);
    void set_enable_asr_service(bool enable_asr_service);
    int read_file_content(const char *file, std::string &result);
    void init_default();

    std::string _asr_api_source;
    std::string _app_key;
    std::string _app_secret_key;
    std::string _asr_server;
    std::string _audio_format;
    int _audio_type = 1537;
    std::string _capacity_scope;
    int _concurrent_number = 2;
    bool _enable_asr_service = true;
    std::string _working_dir;
};

#endif // APP_CONFIG_HPP
