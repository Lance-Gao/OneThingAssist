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

private:
    std::string _id;
    std::string _source;
    std::string _dst;
    int _delay_in_ms;
    int _face_track_count;
    int _max_track_fps_when_no_face;
    int _enable_face_detect;
    int _save_images_count;
    int _is_save_bkimgs;
    float _bkimg_resize_ratio;
    std::string _bkimg_dir;
    int _enable_push_img;
    int _live;
    int _gpu_id;
    int _heartbeat_interval;
    bool _enable_face_recognize;
    conf_recognize_t _recognize;
    conf_recording_t _original_recording;
    conf_recording_t _faced_recording;
    conf_publish_t _publish;
    conf_face_quality_t _face_quality;
    conf_push_img_t _push_img;
    int _enable_fps_barrier;
    int _savedfaceimg_notext_flag;
    bool _enable_compatiblity_with_previous;
    bool _read_nativefile_flag;
    float _native_read_framerate;
    float _max_framerate_times;
    int _cutapart_video_sum;
    int _cutapart_video_num;
    bool _cutapart_video_flag;
    int64_t _video_starttime_ms;
    int64_t _offset_time_ms;
    bool _readfile_loop_flag;
    bool _enable_save_facesdetected;
    bool _enable_save_framesdetected;
    size_t _imgnum_per_track;
    bool _enable_max_strategy;
    size_t _imgnum_sortby_weightsum;
    bool _enable_weightsum_sort;
    bool _log_forevaluation_flag;
    int _video_frame_rate;
    int _video_frame_interval;
    bool _enable_gat1400;
    std::string _gb_id;
    std::string _gat1400_serverurl;
    std::string _gat1400_username;
    std::string _gat1400_password;
    std::map<int, int> _gpu_decode_conf;

private:
    std::string _working_dir;
private:
    void init_default();

public:
    const char *get_command_line_help();

    std::string get_app_version();

    int load_from_file(const char *file);

    int load_command_line(char *const *argv, int argc);

    const char *ensure_valid_string(char *input);

    int string_to_int(const char *input);

    float string_to_float(const char *input);

    int parse_range(const char *input, int *low, int *high);

    char *clean_opt(char *opt);

    void set_min_frame_interval(const char *optarg);

    void set_quality_duration(const char *optarg);

    void set_quality_size_weight(const char *optarg);

    void set_quality_angle_weight(const char *optarg);

    void set_id(const char *optarg);

    void set_gpu_id(const char *optarg);

    void set_source(const char *optarg);

    void set_dst(const char *optarg);

    void set_delay(const char *optarg);

    void set_is_live(int live);

    void set_mask(const char *optarg);

    void set_recognize_server(const char *optarg);

    void set_face_trail_server(const char* optarg);
    
    void set_worker_server(const char* server);

    void set_recognize_times_per_face(const char *optarg);

    void set_is_original_recording(const char *optarg);

    void set_original_recording_location(const char *optarg);

    void set_original_recording_max_duration(const char *optarg);

    void set_original_recording_hls_duration(const char *optarg);

    void set_is_faced_recording(const char *optarg);

    void set_faced_recording_location(const char *optarg);

    void set_faced_recording_max_duration(const char *optarg);

    void set_faced_recording_hls_duration(const char *optarg);

    void set_minimum_face(const char *optarg);

    void set_pitch_threshold(const char *optarg);

    void set_yaw_threshold(const char *optarg);

    void set_roll_threshold(const char *optarg);

    void set_first_blurry_threshold(const char* optarg);

    void set_blurry_threshold(const char *optarg);

    void set_bright_threshold(const char *optarg);

    void set_occlusion_threshold(const char *optarg);

    void set_confidence_threshold(const char *optarg);

    void set_face_track_count(const char *optarg);

    void set_save_images_count(const char *optarg);
    
    void set_max_track_fps_when_no_face(const char* optarg);

    void set_is_publish(int publish);

    void set_publish_url(const char *optarg);
    
    void set_heartbeat_interval(const char* optarg);
    
    int get_heartbeat_interval();

    void set_enable_face_detect(int enable_face_detect);

    int is_enable_face_detect();
    
    void set_is_save_bkimg(int save);
    void set_bkimg_resize_ratio(float ratio);    
    void set_bkimg_dir(const char* optarg);

    const conf_push_img_t& get_conf_push_img();

    void set_enable_push_img(int enable_push_img);

    int is_enable_push_img();

    void set_server_to_push(const char* optarg);

    const std::string& get_server_to_push();

    void set_duration_push_img_in_ms(const char* optarg);

    int get_duration_push_img_in_ms();

    void set_resize_ratio_push_img(const char* optarg);

    float get_resize_ratio_push_img();

    void set_quality_push_img(const char* optarg);

    int get_quality_push_img();

    void set_enable_compatiblity(bool enable_compatiblity);

    bool is_enable_compatiblity();

    void enable_save_faces_detected(int enable_save_facesdetected);

    int is_save_faces_detected();

    void enable_save_frames_detected(int enable_save_framesdetected);

    int is_save_frames_detected();

    void set_imgnum_per_track(const char* optarg);

    size_t get_imgnum_per_track();

    bool is_enable_max_strategy();

    void set_imgnum_sortby_weightsum(const char* optarg);

    size_t get_imgnum_sortby_weightsum();

    bool is_enable_weightsum_sort();

private:
    void init_dir();

    int read_file_content(const char *file, std::string &result);

    void set_video_frameinterval();

    void* set_gat1400_serverurl(const char* optarg);

    void* set_gat1400_username(const char* optarg);

    void* set_gat1400_password(const char* optarg);

    void* set_gpudecode_configuration(const char* optarg);

public:
    const std::string &get_working_dir();

    const std::string &get_id();

    int get_gpu_id();

    const std::string &get_source();

    const std::string &get_dst();

    int get_delay();

    int get_face_track_count();

    int get_save_images_count();
    
    int get_max_track_fps_when_no_face();

    int is_live();

    const conf_recognize_t &get_recognize();

    const conf_recording_t &get_original_recording();

    const conf_recording_t &get_faced_recording();

    const conf_publish_t &get_publish();

    const conf_face_quality_t &get_face_quality();
    
    int is_save_bkimg();
    
    float get_bkimg_resize_ratio();
    const std::string& get_bkimg_dir(); 

    std::string to_string();

    int check_server_uri();

    void set_enable_face_recognize(int enable_face_recognize);

    int is_enable_face_recognize();

    void set_enable_fps_barrier(int enable_fps_barrier);

    int is_enable_fps_barrier();

    void set_savedfaceimg_notext_flag();

    int get_savedfaceimg_notext_flag();

    // when source is video file, tracer read file using framerate setted
    void set_native_framerate(const char* optarg);

    bool get_nativefile_readflag();

    float get_native_framerate();

    void set_max_framerate_times(const char* optarg);

    void set_video_cutapart_ratio(const char* optarg);

    bool get_video_cutapart_flag();

    int get_video_cutapart_ratio(int *part_sum, int *part_num);

    void set_video_starttime(const char * optarg);

    int64_t get_video_starttime();

    void set_video_offsettime(const char* optarg);

    int64_t get_video_offsettime();

    // if flag is true, we will loop to readfile.
    void set_readfile_loop_flag(int loop_flag);

    bool get_readfile_loop_flag();

    void set_log_forevaluation_flag();

    bool get_log_forevaluation_flag();

    void set_video_framerate(int frame_rate);

    int get_video_framerate();

    int get_video_frameinterval();

    bool is_enable_gat1400();

    std::string get_gat1400_serverurl();

    std::string get_gat1400_username();

    std::string get_gat1400_password();

    int get_gpudecode_id();

    void set_gb_id(const char* optarg);

    const std::string& get_gb_id();
};

#endif // APP_CONFIG_HPP
