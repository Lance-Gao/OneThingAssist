#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP

#include <map>
#include <string>
#include <iostream>

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
    int get_logoff_ms();
    int get_server_port();
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
    void set_logoff_ms(const char* optarg);
    void set_server_port(const char* optarg);
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
    int _log_off_ms = 2000;
    int _server_port = 8005;
    std::string _working_dir;
};

#endif // APP_CONFIG_HPP
