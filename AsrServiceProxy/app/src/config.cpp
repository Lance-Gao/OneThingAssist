#include "config.h"
#include <sys/stat.h>
#include <getopt.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <aip_log.hpp>
#include <json_util.hpp>
#include <utils.hpp>
#include "url_parser.hpp"

typedef enum _opt_id {
    OPT_HELP,
    OPT_ASRAPI_SOURCE,
    OPT_APP_KEY,
    OPT_APP_SECRETKEY,
    OPT_ASR_SERVER,
    OPT_AUDIO_FORMAT,
    OPT_AUDIO_TYPE,
    OPT_CAPACITY_SCOPE,
    OPT_CONCURRENT_NUMBER,
    OPT_ENABLE_ASR_SERVICE,
} opt_id_t;

typedef struct _option_entry {
    const char* opt_name;
    const char* opt_description;
    const char* opt_defalut_value;
} option_entry_t;

static const int MAX_DELAY = 300000;

const static option_entry_t s_options[] = {
    { "--api-belong-to", "the asr api of which company", "baidu" },
    { "--app-key", "the key of asr app", "kVcnfD9iW2XVZSMaLMrtLYIz" },
    { "--app-secret-key", "the secret key of asr app", "O9o1O213UgG5LFn0bDGNtoRN3VWl2du6" },
    { "--asr-server", "the url of asr server", "http://vop.baidu.com/server_api" },
    { "--audio-format", "the format of input audio data", "pcm" },
    { "--audio-type", "the type of input audio data", "1537" },
    { "--capacity-scope", "the flag of audio capacity, for bd asr api", "audio_voice_assistant_get" },
    { "--concurrent-number", "the concurrent number of calling asr service", "2" },
    { "--enable-asr-service", "enable asr service", "true" },
    { 0, 0, 0 }
};

static struct option s_long_options[] = {
    { "help", no_argument, 0, OPT_HELP},
    { "asr-api-source", required_argument, 0, OPT_ASRAPI_SOURCE},
    { "app-key", required_argument, 0, OPT_APP_KEY},
    { "app-secret-key", required_argument, 0, OPT_APP_SECRETKEY},
    { "asr-server", required_argument, 0, OPT_ASR_SERVER},
    { "audio-format", required_argument, 0, OPT_AUDIO_FORMAT},
    { "audio-type", required_argument, 0, OPT_AUDIO_TYPE},
    { "capacity-scope", required_argument, 0, OPT_CAPACITY_SCOPE},
    { "concurrent-number", required_argument, 0, OPT_CONCURRENT_NUMBER},
    { "enable-asr-service", required_argument, 0, OPT_ENABLE_ASR_SERVICE},
    {0, 0, 0}
    };

Config::Config() {
    init_dir();
    init_default();
    load_from_file("./conf/default.json");
}

Config::~Config() {
}

void Config::init_default() {
    this->_asr_api_source = "baidu";
    this->_app_key = "kVcnfD9iW2XVZSMaLMrtLYIz";
    this->_app_secret_key = "O9o1O213UgG5LFn0bDGNtoRN3VWl2du6";
    this->_asr_server = "http://vop.baidu.com/server_api";
    this->_audio_format = "pcm";
    this->_audio_type = 1537;
    this->_capacity_scope = "audio_voice_assistant_get";
    this->_concurrent_number = 2;
}

const char* Config::get_command_line_help() {
    static std::string s_help_string;
    std::string example = "example:\n\tfacetracer ";
    std::string help_string = "command details:\n";

    const option_entry_t* entry = &s_options[0];

    while (entry->opt_name) {

        example += entry->opt_name;
        example += " ";
        example += entry->opt_defalut_value;
        example += " ";

        help_string += "\t";
        help_string += entry->opt_name;
        help_string += ": ";
        help_string += entry->opt_description;
        help_string += "\n";

        entry++;
    }

    example += "\n\n";
    example += help_string;
    s_help_string = example;

    return s_help_string.c_str();
}

int Config::load_from_file(const char* file) {
    std::string content;
    if (read_file_content(file, content)) {
        Json::Value json;
        std::string msg;
        if (JsonUtils::load_json(content, json, msg)) {
            if (!json.isNull()) {
                Json::Value& conf = json["configuration"];
                if (!conf.isNull()) {
                    Json::Value& enable_asr_service = conf["enable_asr_service"];
                    if (!enable_asr_service.isNull()) {
                        set_enable_asr_service(enable_asr_service.asBool());
                    }
                    Json::Value& asr_api_source = conf["asr_api_source"];
                    if (!asr_api_source.isNull()) {
                        set_asrapi_source(StringUtil::trim(asr_api_source.asString()).c_str());
                    }
                    Json::Value& app_key = conf["app_key"];
                    if (!app_key.isNull()) {
                        set_app_key(StringUtil::trim(app_key.asString()).c_str());
                    }
                    Json::Value& app_secret_key = conf["app_secret_key"];
                    if (!app_secret_key.isNull()) {
                        set_appsecret_key(StringUtil::trim(app_secret_key.asString()).c_str());
                    }
                    Json::Value& asr_server = conf["asr_server"];
                    if (!asr_server.isNull()) {
                        set_asr_server(StringUtil::trim(asr_server.asString()).c_str());
                    }
                    Json::Value& audio_format = conf["audio_format"];
                    if (!audio_format.isNull()) {
                        set_audio_format(StringUtil::trim(audio_format.asString()).c_str());
                    }
                    Json::Value& audio_type = conf["audio_type"];
                    if (!audio_type.isNull()) {
                        set_audio_type(StringUtil::trim(audio_type.asString()).c_str());
                    }
                    Json::Value& capacity_scope = conf["capacity_scope"];
                    if (!capacity_scope.isNull()) {
                        set_capacity_scope(StringUtil::trim(capacity_scope.asString()).c_str());
                    }
                    Json::Value& concurrent_number = conf["concurrent_number"];
                    if (!concurrent_number.isNull()) {
                        set_concurrent_number(StringUtil::trim(concurrent_number.asString()).c_str());
                    }
                }
            }
        } else {
            AIP_LOG_FATAL("parse json failed");

        }
    }
    return 0;
}

void Config::set_enable_asr_service(bool enable_asr_service) {
    this->_enable_asr_service = enable_asr_service;
}

void Config::set_app_key(const char* optarg) {
    this->_app_key = optarg;
}

void Config::set_appsecret_key(const char* optarg) {
    this->_app_secret_key = optarg;
}

void Config::set_asrapi_source(const char* optarg) {
    this->_asr_api_source = optarg;
}

void Config::set_asr_server(const char* optarg) {
    this->_asr_server = optarg;
}

void Config::set_audio_format(const char* optarg) {
    this->_audio_format = optarg;
}

void Config::set_audio_type(const char* optarg) {
    this->_audio_type = string_to_int(optarg);
}

void Config::set_capacity_scope(const char* optarg) {
    this->_capacity_scope = optarg;
}

void Config::set_concurrent_number(const char* optarg) {
    this->_concurrent_number = string_to_int(optarg);;
}

int Config::read_file_content(const char* file, std::string& result) {
    result.clear();
    std::ifstream is(file, std::ifstream::in);
    int len = 0;
    if (is) {
        is.seekg(0, is.end);
        int length = is.tellg();
        is.seekg(0, is.beg);
        if (length <= 0) {
            AIP_LOG_FATAL("read_file_content: file is empty");
        } else if (length > INT_MAX / 4) {
            AIP_LOG_FATAL("read_file_content: file is too large");
        } else {
            char* buffer = new char[length];
            is.read(buffer, length);
            if (is) {
                result.assign(buffer);
                len = length;
            } else {
                AIP_LOG_FATAL("read_file_content: failed to read content, "
                                "need %d. actual %d", length, is.gcount());
            }
            is.close();
            delete [] buffer;
        }
    } else {
        AIP_LOG_FATAL("failed to open file: %s", file);
    }
    return len;
}

int Config::load_command_line(char* const* argv, int argc) {
    int opt = 0;
    int option_index = 0;
    const char* optstring = "h";

    while ((opt = getopt_long_only(argc, argv, optstring, s_long_options, &option_index)) != -1) {
        char* cleaned_optarg = clean_opt(optarg);
        printf("opt: %d: %s\n", opt, cleaned_optarg);

        switch (opt) {
        case 'h':
        case OPT_HELP:
            return 1;

        case OPT_ASRAPI_SOURCE: {
            set_asrapi_source(cleaned_optarg);
        }
        break;

        case OPT_APP_KEY: {
            set_app_key(cleaned_optarg);
        }
        break;

        case OPT_APP_SECRETKEY: {
            set_appsecret_key(cleaned_optarg);
        }
        break;

        case OPT_ASR_SERVER: {
            set_asr_server(cleaned_optarg);
        }
        break;

        case OPT_AUDIO_FORMAT: {
            set_audio_format(cleaned_optarg);
        }
        break;

        case OPT_AUDIO_TYPE: {
            set_audio_type(cleaned_optarg);
        }
        break;

        case OPT_CAPACITY_SCOPE: {
            set_capacity_scope(cleaned_optarg);
        }
        break;

        case OPT_CONCURRENT_NUMBER: {
            set_concurrent_number(cleaned_optarg);
        }
        break;

        case OPT_ENABLE_ASR_SERVICE: {
            set_enable_asr_service(StringUtil::to_bool(cleaned_optarg));
        }
        break;
        default: {
        }
        break;
        }
    }

    return 0;
}

const char* Config::ensure_valid_string(char* input) {
    static const char* s_valid = "_-";
    char* start = input;

    while (*start) {
        if (!isalnum(*start) && !strchr(s_valid, *start)) {
            *start = '_';
        }
        start++;
    }

    return input;
}

const std::string& Config::get_app_key() {
    return this->_app_key;
}

const std::string& Config::get_appsecret_key() {
    return this->_app_secret_key;
}

const std::string& Config::get_asrapi_source() {
    return this->_asr_api_source;
}

const std::string& Config::get_audio_format() {
    return this->_audio_format;
}

int Config::get_audio_type() {
    return this->_audio_type;
}

const std::string& Config::get_asr_server() {
    return this->_asr_server;
}

const std::string& Config::get_capacity_scope() {
    return this->_capacity_scope;
}

int Config::get_concurrent_number() {
    return this->_concurrent_number;
}

bool Config::is_enable_asr_service() {
    return this->_enable_asr_service;
}

const std::string& Config::get_working_dir() {
    return _working_dir;
}

void Config::init_dir() {
    char* buffer = NULL;
    if ((buffer = getcwd(NULL, 0)) == NULL) {
        perror("_getcwd error");
        return;
    }

    char resolved_path[40960] = {0};
    realpath(buffer, resolved_path);
    free(buffer);
    _working_dir = std::string(resolved_path);
}

int Config::string_to_int(const char* input) {
    return atoi(input);
}

float Config::string_to_float(const char* input) {
    return atof(input);
}

int Config::parse_range(const char* input, int* low, int* high) {
    std::string src(input);
    std::vector<std::string> parts;
    StringUtil::split(src, ',', &parts);

    if (parts.size() != 2) {
        AIP_LOG_WARNING("parse range failed: %s", input);
        return -1;
    }
    std::string slow = StringUtil::trim(parts[0]);
    std::string shigh = StringUtil::trim(parts[1]);

    *low = atoi(slow.c_str());
    *high = atoi(shigh.c_str());

    return 0;
}

char* Config::clean_opt(char* opt) {
    if (!opt) {
        return opt;
    }
    while (*opt && (isspace(*opt) || isblank(*opt)
          || *opt == '"' || *opt == '\'')) {
        opt++;
    }

    char* end = opt + strlen(opt) - 1;

    while (end > opt && (isspace(*end) || isblank(*end)
          || *end == '"' || *end == '\'')) {
        end--;
    }
    *(++end) = 0;

    return opt;
}

std::string Config::to_string() {
    std::stringstream builder;
    builder << "app key:              " << get_app_key() << std::endl;
    builder << "app secret key:       " << get_appsecret_key() << std::endl;
    builder << "asr api source:       " << get_asrapi_source() << std::endl;
    builder << "audio format:         " << get_audio_format() << std::endl;
    builder << "audio type:           " << get_audio_type() << std::endl;
    builder << "asr server:           " << get_asr_server() << std::endl;
    builder << "concurrent number:    " << get_concurrent_number() << std::endl;
    return builder.str();
}

int Config::check_server_uri() {
    if (is_enable_asr_service()) {
        UrlParser asr_server_parser(get_asr_server());
        if (asr_server_parser.get_uri().empty()) {
            printf("The uri of asr server is null!\n");
            return -1;
        }
    }

    return 0;
}
