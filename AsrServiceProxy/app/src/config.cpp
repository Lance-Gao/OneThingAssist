#include "config.hpp"
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
#include "git_version.hpp"
#include "url_parser.hpp"

typedef enum _opt_id {
    OPT_HELP,
    OPT_ID,
    OPT_SOURCE,
    OPT_DST,
    OPT_REC_MASK,
    OPT_REC_SRV,
    OPT_FACE_TRAIL_SRV,
    OPT_WORKER_SRV,
    OPT_REC_TIMES_PER_FACE,
    OPT_IS_ORG_REC,
    OPT_ORG_REC_LOC,
    OPT_ORG_REC_MAX_DUR,
    OPT_ORG_REC_HLS_DUR,
    OPT_IS_FACED_REC,
    OPT_FACED_REC_LOC,
    OPT_FACED_REC_MAX_DUR,
    OPT_FACED_REC_HLS_DUR,
    OPT_IS_PUBLISH,
    OPT_PUBLISH_URL,
    OPT_MIN_FACE,
    OPT_PITCH_THRES,
    OPT_YAW_THRES,
    OPT_ROLL_THRES,
    OPT_BLURRY_THRES,
    OPT_BRIGHT_THRES,
    OPT_OCCLUSION_THRES,
    OPT_CONFIDENCE_THRES,
    OPT_GPU_ID,
    OPT_LIVE,
    OPT_SAVE_IMAGE_COUNT,
    OPT_FACE_TRACK_COUNT,
    OPT_MIN_FRAME_INTERVAL,
    OPT_DELAY_IN_MS,
    OPT_QUALITY_DURATION_IN_MS,
    OPT_QUALITY_SIZE_WEIGHT,
    OPT_QUALITY_ANGLE_WEIGHT,
    OPT_MAX_TRACK_FPS_WHEN_NO_FACE,
    OPT_HEARTBEAT_INTERVAL,
    OPT_SAVE_BKIMG,
    OPT_BKIMG_RESIZE_RATIO,
    OPT_BKIMG_DIR,
    OPT_PUSH_IMG,
    OPT_SERVER_PUSH_IMG,
    OPT_DURATION_PUSH_IMG,
    OPT_RESIZE_RATIO_PUSH_IMG,
    OPT_QULITY_PUSH_IMG,
    OPT_ENABLE_FACE_DETECT,
    OPT_ENABLE_FPS_BARRIER,
    OPT_VERSION,
    OPT_SAVED_FACEIMG_NOTEXT,
    OPT_COMPATIBLITY,
    OPT_NATIVE_FRAME_RATE,
    OPT_MAX_FRAMERATE_TIMES,
    OPT_VIDEO_CUTAPART_RATIO,
    OPT_VIDEO_START_TIME,
    OPT_VIDEO_OFFSET_TIME,
    OPT_READ_FILE_ONCE,
    OPT_ENABLE_SAVE_FACESDETECTED,
    OPT_ENABLE_SAVE_FRAMESDETECTED,
    OPT_IMGNUM_PER_TRACK,
    OPT_IMGNUM_SORTBY_WEIGHTSUM,
    OPT_LOG_FOR_EVALUATION,
    OPT_GAT1400_SERVERURL,
    OPT_GAT1400_USERNAME,
    OPT_GAT1400_PASSWORD,
    OPT_GPU_DECODECONFIGRATION,
    OPT_GAT1400_GBID,
} opt_id_t;

typedef struct _option_entry {
    const char* opt_name;
    const char* opt_description;
    const char* opt_defalut_value;
} option_entry_t;

static const int MAX_DELAY = 300000;

const static option_entry_t s_options[] = {
    { "--id", "unique id, only one process could be running for same id.", "default_id" },
    { "--source", "stream source: rtsp or local file.", "\"rtsp://xxxxx\"" },
    { "--dst", "stream destination: rtmp only.", "\"rtmp://127.0.0.1/live/livestream\"" },
    { "--gpu-id", "which gpu will be used", "0" },
    { "--delay-in-ms", "how long will delay for recognition[max 300000]", "500" },
    { "--live", "whether the source is live stream, which couldn't be blocked", "true" },
    {
        "--mask",
        "ROI, base64 string of jpg encoded picture,it can be "
            "scaled for fitting to video resolution",
        "[base64 string]"
    },
    { "--recognize-server", "recognize server url", "\"http://xxxxx\"" },
    { "--face-trail-server", "face trail server url", "\"http://xxxxx\"" },
    { "--worker-server", "worker server url", "\"http://xxxxxx\""},
    { "--recognize-times-per-face", "maximum recognition times for every face", "3" },
    { "--is-original-recording", "whether the original video should be recorded", "false" },
    { 
        "--original-recording-location", "location for original video recording", 
        "/recording/original" },
    {
        "--original-recording-max-duration-in-seconds",
        "maximum duration for each recording file, in seconds", "3600"
    },
    {
        "--original-recording-hls-slice-duration-in-seconds",
        "duration for each hls slice, in seconds", "10"
    },
    { 
        "--is-faced-recording", "whether the face-info-overlapped-video should be recorded", 
        "false" },
    { 
        "--faced-recording-location", " location for face-info-overlapped-video recording",
        "/recording/faced" },
    {
        "--faced-recording-max-duration-in-seconds",
        "maximum duration for each recording file, in seconds", "3600"
    },
    {
        "--faced-recording-hls-slice-duration-in-seconds",
        "duration for each hls slice, in seconds", "10"
    },
    { "--is-publish", "whether publish the video stream to media server", "false" },
    { "--publish-url", "destination url to push video stream", "\"rtmp://xxxxxxx\"" },
    { "--minimum-face-in-pixels", "minimum face for face detection", "80"},
    { "--pitch-threshold", "pitch threshold for recognition", "\"-60,60\"" },
    { "--yaw-threshold", "yaw threshold for recognition", "\"-60,60\"" },
    { "--roll-threshold", "roll threshold for recognition", "\"-60,60\""},
    { "--blurry-threshold", "blurry threshold for recognition", "1" },
    { "--bright-threshold", "bright threshold for recognition", "0" },
    { "--occlusion-threshold", "occlusion threshold for recognition", "1" },
    { "--confidence-threshold", "confidence threshold for face detection", "0.3" },
    { "--save-images-count", "how many images will be saved", "150" },
    { "--face-track-count", "max faces to track in one picture", "10"},
    { "--min-frame-interval", "minimum frames between two faces", "5"},
    { 
        "--quality-duration-in-ms", 
        "waiting duration in order to pick up a best quality face", "2000"
    },
    { "--quality-size-weight", "size weight to eveluate face quality", "1"},
    { "--quality-angle-weight", "angle weight to eveluate face quality", "1"},
    { "--max-fps-when-no-face", "max frames to track per seconds when there's no faces", "5"},
    { 
        "--heartbeat-interval", 
        "duration betweent two heartbeats in milliseconds;[5000~300000]", "15000"
    },
    { "--is-save-bkimg", "whether save bkimgs", "false" },
    { "--bkimg-size-ratio", "resize bkimg to the ratio", "1.0"},
    { "--bkimg-dir", "directory where app will put bkimgs", "bkimages"},
    { "--enable-face-detect", "whether enable the face detection", "true" },
    { "--enable-push-img", "whether push image", "false" },
    { "--push-img-server", "push image server url", "\"http://xxxxx\"" },
    { "--push-img-msduration", "ms duration of pushing image", "2000" },
    { "--push-img-size-ratio", "resize the image pushed to the ratio", "1.0" },
    { "--push-img-quality", "quality of image pushed", "80" },
    { "--version", "the version of facetracer", "" },
    { "--saved-faceimg-notext", "not to write pos in face image saved", "" },
    { "--compatiblity", "whether enable compatiblity with sfgv1.1", "false" },
    { "--read_frame_rate", "if source is a video file, read file by rate setted,"
      " 0 for origin frame rate and may discard frames, 1... for times rate and "
      " not discard frames",
      ""},
    { "--max_read_framerate", "max rate of reading video file", ""},
    { "--video_cutapart_ratio", "cut the video apart", ""},
    { "--video_start_time", "set the start time of video", "0"},
    { "--video_offset_time", "set the offset time of video", "0"},
    { "--read_file_once", "if source is a video file, stop reading when arrived the end of file",
      ""},
    { "--enable-save-facesdetected", "whether to save all faces detected", "false" },
    { "--enable-save-framesdetected", "whether to save all frames detected", "false" },
    { "--imgnum-per-track", "the realize of max strategy; face image number per track", "1"},
    { "--log-for-evaluation", "print log for evaluation", "" },
    { "--gat1400-server-url", "gat1400 server url", "" },
    { "--gat1400-user-name", "gat1400 user name", "" },
    { "--gat1400-user-password", "gat1400 user password", "" },
    { "--gpu-decode-configuration", "gpu configuration for decoding",
      "decoding 0 stream per card in 4 cards device, such as \"0, 0, 0, 0\"" },
    { "--gbid", "gat1400 gd id of device", "default_gbid" },
    { 0, 0, 0 }
};

static struct option s_long_options[] = {
    { "help", no_argument, 0, OPT_HELP},
    { "id", required_argument, 0, OPT_ID},
    { "gpu-id", required_argument, 0, OPT_GPU_ID},
    { "live", required_argument, 0, OPT_LIVE},
    { "source", required_argument, 0, OPT_SOURCE},
    { "dst", required_argument, 0, OPT_DST},
    { "mask", required_argument, 0, OPT_REC_MASK},
    { "recognize-server", required_argument, 0, OPT_REC_SRV},
    { "face-trail-server", required_argument, 0, OPT_FACE_TRAIL_SRV},
    { "worker-server", required_argument, 0, OPT_WORKER_SRV},
    { "recognize-times-per-face", required_argument, 0, OPT_REC_TIMES_PER_FACE},
    { "is-original-recording", required_argument, 0, OPT_IS_ORG_REC},
    { "original-recording-location", required_argument, 0, OPT_ORG_REC_LOC},
    { "original-recording-max-duration-in-seconds", required_argument, 0, OPT_ORG_REC_MAX_DUR},
    { 
        "original-recording-hls-slice-duration-in-seconds", required_argument, 0,
        OPT_ORG_REC_HLS_DUR},
    { "is-faced-recording", required_argument, 0, OPT_IS_FACED_REC},
    { "faced-recording-location", required_argument, 0, OPT_FACED_REC_LOC},
    { "faced-recording-max-duration-in-seconds", required_argument, 0, OPT_FACED_REC_MAX_DUR},
    { 
        "faced-recording-hls-slice-duration-in-seconds", required_argument, 0, 
        OPT_FACED_REC_HLS_DUR},
    { "is-publish", required_argument, 0, OPT_IS_PUBLISH},
    { "publish-url", required_argument, 0, OPT_PUBLISH_URL},
    { "minimum-face-in-pixels", required_argument, 0, OPT_MIN_FACE},
    { "pitch-threshold", required_argument, 0, OPT_PITCH_THRES},
    { "yaw-threshold", required_argument, 0, OPT_YAW_THRES},
    { "roll-threshold", required_argument, 0, OPT_ROLL_THRES},
    { "blurry-threshold", required_argument, 0, OPT_BLURRY_THRES},
    { "bright-threshold", required_argument, 0, OPT_BRIGHT_THRES},
    { "occlusion-threshold", required_argument, 0, OPT_OCCLUSION_THRES},
    { "confidence-threshold", required_argument, 0, OPT_CONFIDENCE_THRES},
    { "save-images-count", required_argument, 0, OPT_SAVE_IMAGE_COUNT},
    { "face-track-count", required_argument, 0, OPT_FACE_TRACK_COUNT},
    { "min-frame-interval", required_argument, 0, OPT_MIN_FRAME_INTERVAL},
    { "delay-in-ms", required_argument, 0, OPT_DELAY_IN_MS},
    { "quality-duration-in-ms", required_argument, 0, OPT_QUALITY_DURATION_IN_MS}, 
    { "quality-size-weight", required_argument, 0, OPT_QUALITY_SIZE_WEIGHT},
    { "quality-angle-weight", required_argument, 0, OPT_QUALITY_ANGLE_WEIGHT},
    { "max-fps-when-no-face", required_argument, 0, OPT_MAX_TRACK_FPS_WHEN_NO_FACE},
    { "heartbeat-interval", required_argument, 0, OPT_HEARTBEAT_INTERVAL},
    { "is-save-bkimg", required_argument, 0, OPT_SAVE_BKIMG},
    { "bkimg-size-ratio", required_argument, 0, OPT_BKIMG_RESIZE_RATIO},
    { "bkimg-dir", required_argument, 0, OPT_BKIMG_DIR},
    { "enable-push-img", required_argument, 0, OPT_PUSH_IMG},
    { "push-img-server", required_argument, 0, OPT_SERVER_PUSH_IMG},
    { "push-img-msduration", required_argument, 0, OPT_DURATION_PUSH_IMG},
    { "push-img-size-ratio", required_argument, 0, OPT_RESIZE_RATIO_PUSH_IMG},
    { "push-img-quality", required_argument, 0, OPT_QULITY_PUSH_IMG},
    { "enable-face-detect", required_argument, 0, OPT_ENABLE_FACE_DETECT},
    { "enable-fps-barrier", required_argument, 0, OPT_ENABLE_FPS_BARRIER},
    { "version", no_argument, 0, OPT_VERSION},
    { "saved-faceimg-notext", no_argument, 0, OPT_SAVED_FACEIMG_NOTEXT},
    { "compatiblity", required_argument, 0, OPT_COMPATIBLITY},
    { "read_frame_rate", required_argument, 0, OPT_NATIVE_FRAME_RATE},
    { "max_read_framerate", required_argument, 0, OPT_MAX_FRAMERATE_TIMES},
    { "video_cutapart_ratio", required_argument, 0, OPT_VIDEO_CUTAPART_RATIO},
    { "video_start_time", required_argument, 0, OPT_VIDEO_START_TIME},
    { "video_offset_time", required_argument, 0, OPT_VIDEO_OFFSET_TIME},
    { "read_file_once", required_argument, 0, OPT_READ_FILE_ONCE},
    { "enable-save-facesdetected", required_argument, 0, OPT_ENABLE_SAVE_FACESDETECTED},
    { "enable-save-framesdetected", required_argument, 0, OPT_ENABLE_SAVE_FRAMESDETECTED},
    { "imgnum-per-track", required_argument, 0, OPT_IMGNUM_PER_TRACK},
    { "imgnum-sortby_weightsum", required_argument, 0, OPT_IMGNUM_SORTBY_WEIGHTSUM},
    { "log-for-evaluation", no_argument, 0, OPT_LOG_FOR_EVALUATION},
    { "gat1400-server-url", required_argument, 0, OPT_GAT1400_SERVERURL},
    { "gat1400-user-name", required_argument, 0, OPT_GAT1400_USERNAME},
    { "gat1400-user-password", required_argument, 0, OPT_GAT1400_PASSWORD},
    { "gpu-decode-configuration", required_argument, 0, OPT_GPU_DECODECONFIGRATION},
    { "gbid", required_argument, 0, OPT_GAT1400_GBID},
    {0, 0, 0}
    };

Config::Config() {
    init_dir();
    init_default();
    load_from_file("./conf/facetracer.json");
}

Config::~Config() {
}

void Config::init_default() {
    this->_id = "default_id";
    this->_gpu_id = 0; 
    this->_face_track_count = 10;
    this->_save_images_count = 100;
    this->_max_track_fps_when_no_face = 5;
    this->_heartbeat_interval = 15000;
    this->_live = 1;
    this->_source = "/home/idl-face/test.mp4";
    this->_dst = "rtmp://127.0.0.1/live/livestream";

    this->_enable_face_detect = 1;
    
    this->_is_save_bkimgs = 0;
    this->_bkimg_resize_ratio = 1.0;
    this->_bkimg_dir = "bkimages";
    
    this->_delay_in_ms = 0;
    this->_recognize.mask = "";
    this->_recognize.recognize_server = "";
    this->_recognize.face_trail_server = "";
    this->_recognize.worker_server = "http://127.0.0.1:8080";
    this->_recognize.times_per_face = 1;

    this->_original_recording.is_recording = 0;
    this->_original_recording.location = "/recording/original";
    this->_original_recording.max_duration_in_seconds = 3600;
    this->_original_recording.hls_slice_duration_in_seconds = 10;

    this->_faced_recording.is_recording = 0;
    this->_faced_recording.location = "/recording/faced";
    this->_faced_recording.max_duration_in_seconds = 3600;
    this->_faced_recording.hls_slice_duration_in_seconds = 10;

    this->_publish.is_publish = 0;
    this->_publish.publish_url = "";

    this->_face_quality.minimum_face_in_pixels = 80;
    this->_face_quality.pitch_threshold.low = -60;
    this->_face_quality.pitch_threshold.high = 60;
    this->_face_quality.yaw_threshold.low = -60;
    this->_face_quality.yaw_threshold.high = 60;
    this->_face_quality.roll_threshold.low = -60;
    this->_face_quality.roll_threshold.high = 60;
    this->_face_quality.first_blurry_threshold = 0.3f;
    this->_face_quality.blurry_threshold = 0.5f;
    this->_face_quality.bright_threshold = 256.0f;
    this->_face_quality.occlusion_threshold = 1.0f;
    this->_face_quality.confidence_threshold = 0.5f;
    this->_face_quality.min_frame_interval = 5;
    this->_face_quality.time_to_wait_in_ms = 5000;
    this->_face_quality.size_weight = 1.0f;
    this->_face_quality.angle_weight = 1.0f;

    this->_enable_push_img = 0;
    this->_push_img.push_img_server = "";
    this->_push_img.duration_in_mseconds = 2000;
    this->_push_img.img_resize_ratio = 0.5f;
    this->_push_img.img_quality = 80;

    this->_enable_fps_barrier = 0;
    this->_savedfaceimg_notext_flag = 0;
    this->_enable_compatiblity_with_previous = false;
    this->_read_nativefile_flag = false;
    this->_native_read_framerate = 0.0f;
    this->_max_framerate_times = 32;
    this->_cutapart_video_sum = 0;
    this->_cutapart_video_num = 0;
    this->_cutapart_video_flag = false;
    this->_video_starttime_ms = 0;
    this->_offset_time_ms = 0;
    this->_readfile_loop_flag = true;
    this->_enable_save_facesdetected = false;
    this->_enable_save_framesdetected = false;
    this->_imgnum_per_track = 1;
    this->_enable_max_strategy = false;
    this->_imgnum_sortby_weightsum = 5;
    this->_enable_weightsum_sort = true;
    this->_log_forevaluation_flag = false;
    this->_video_frame_rate = 0;
    this->_video_frame_interval = 0;
    this->_enable_gat1400 = false;
    this->_gb_id = "default_gb_id";
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

std::string Config::get_app_version() {
    std::string version("version: ");

    version += GIT_VERSION;

    return version;
}

static void set_angle_threshold(int_range_t& angle, Json::Value& value) {
    Json::Value& from = value["from"];
    if (!from.isNull()) {
        angle.low =  atoi(StringUtil::trim(from.asString()).c_str());
    }
    
    Json::Value& to = value["to"];
    if (!to.isNull()) {
        angle.high =  atoi(StringUtil::trim(to.asString()).c_str());
    }
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
                    Json::Value& id = conf["id"];
                    if (!id.isNull()) {
                        set_id(StringUtil::trim(id.asString()).c_str());
                    }
                    Json::Value& gpu_id = conf["gpu_id"];
                    if (!gpu_id.isNull()) {
                        set_gpu_id(StringUtil::trim(gpu_id.asString()).c_str());
                    }
                    Json::Value& source = conf["source"];                
                    if (!source.isNull()) {
                        set_source(StringUtil::trim(source.asString()).c_str());
                    }
                    Json::Value& dst = conf["dst"];
                    if (!dst.isNull()) {
                        set_dst(StringUtil::trim(dst.asString()).c_str());
                    }
                    Json::Value& delay = conf["delay_in_ms"];                
                    if (!delay.isNull()) {
                        set_delay(StringUtil::trim(delay.asString()).c_str());
                    }
                    Json::Value& live = conf["live"];
                    if (!live.isNull()) {
                        set_is_live(live.asBool());
                    }
                    Json::Value& image_count = conf["save_images_count"];
                    if (!image_count.isNull()) {
                        set_save_images_count(
                                StringUtil::trim(image_count.asString()).c_str());
                    }
                    
                    Json::Value& is_save_bkimg = conf["is_save_bkimg"];
                    if (!is_save_bkimg.isNull()) {
                        set_is_save_bkimg(is_save_bkimg.asBool());
                    }
                    Json::Value& bkimg_size = conf["bkimg_resize_ratio"];
                    if (!bkimg_size.isNull()) {
                        set_bkimg_resize_ratio(bkimg_size.asFloat());
                    }
                    Json::Value& bkimg_dir = conf["bkimg_dir"];
                    if (!bkimg_dir.isNull()) {
                        set_bkimg_dir(StringUtil::trim(bkimg_dir.asString()).c_str());
                    }

                    Json::Value& enable_face_detect = conf["enable_face_detect"];
                    if (!enable_face_detect.isNull()) {
                        set_enable_face_detect(enable_face_detect.asBool());
                    }

                    Json::Value& enable_save_facesdetected = conf["enable_save_facesdetected"];
                    if (!enable_save_facesdetected.isNull()) {
                        enable_save_faces_detected(enable_save_facesdetected.asBool());
                    }

                    Json::Value& enable_save_framesdetected = conf["enable_save_framesdetected"];
                    if (!enable_save_framesdetected.isNull()) {
                        enable_save_frames_detected(enable_save_framesdetected.asBool());
                    }

                    Json::Value& enable_push_img = conf["enable_push_img"];
                    if (!enable_push_img.isNull()) {
                        set_enable_push_img(enable_push_img.asBool());
                    }
                    
                    Json::Value& track_count = conf["face_track_count"];
                    if (!track_count.isNull()) {
                        set_face_track_count(
                                StringUtil::trim(track_count.asString()).c_str());
                    }
                    Json::Value& max_track_fps_when_no_face = conf["max_track_fps_when_no_face"];
                    if (!max_track_fps_when_no_face.isNull()) {
                        set_max_track_fps_when_no_face(
                        StringUtil::trim(max_track_fps_when_no_face.asString()).c_str());
                    }
                    Json::Value& heartbeat_interval = conf["heartbeat_interval"];
                    if (!heartbeat_interval.isNull()) {
                        set_heartbeat_interval(
                        StringUtil::trim(heartbeat_interval.asString()).c_str());
                    }
                    Json::Value& recognize = conf["recognize"];
                    if (!recognize.isNull()) {
                        Json::Value& times_per_face = recognize["recognize_times_per_face"];
                        Json::Value& server = recognize["recognize_server"];
                        Json::Value& face_trail_server = recognize["face_trail_server"];
                        Json::Value& worker_server = recognize["worker_server"];
                        if (!times_per_face.isNull()) {
                            set_recognize_times_per_face(
                                 StringUtil::trim(times_per_face.asString()).c_str());
                        }
                        if (!server.isNull()) {
                            set_recognize_server(
                                StringUtil::trim(server.asString()).c_str());
                        }
                        if (!face_trail_server.isNull()) {
                            set_face_trail_server(
                                StringUtil::trim(face_trail_server.asString()).c_str());
                        }
                        if (!worker_server.isNull()) {
                            set_worker_server(StringUtil::trim(server.asString()).c_str());
                        }
                    }
                    Json::Value& original_recording = conf["original_recording"];
                    if (!original_recording.isNull()) {

                    }

                    Json::Value& face_recording = conf["faced_recording"];
                    if (!face_recording.isNull()) {

                    }
                    Json::Value& publish = conf["publish"];
                    if (!publish.isNull()) {
                        Json::Value& is_publish = publish["is_publish"]; 
                        Json::Value& publish_url = publish["publish_url"];
                        if (!is_publish.isNull()) {
                            set_is_publish(is_publish.asBool());
                        }
                        if (!publish_url.isNull()) {
                            set_publish_url(
                                    StringUtil::trim(publish_url.asString()).c_str());
                        }
                    }
                    
                    Json::Value& face_quality = conf["face_quality"];
                    if (!face_quality.isNull()) {
                        Json::Value& minimum_face = face_quality["minimum_face_in_pixels"];
                        if (!minimum_face.isNull()) {
                            set_minimum_face(
                                    StringUtil::trim(minimum_face.asString()).c_str());
                        }
                        Json::Value& pitch = face_quality["pitch_threshold"];
                        if (!pitch.isNull()) {
                            set_angle_threshold(_face_quality.pitch_threshold, pitch);
                        }                        
                        Json::Value& yaw = face_quality["yaw_threshold"];
                        if (!yaw.isNull()) {
                            set_angle_threshold(_face_quality.yaw_threshold, yaw);
                        }                        
                        Json::Value& roll = face_quality["roll_threshold"];
                        if (!roll.isNull()) {
                            set_angle_threshold(_face_quality.roll_threshold, roll);
                        }
                        Json::Value& first_blurry_threshold = 
                                            face_quality["first_blurry_threshold"];
                        if (!first_blurry_threshold.isNull()) {
                            set_first_blurry_threshold(
                                    StringUtil::trim(first_blurry_threshold.asString()).c_str());
                        }
                        Json::Value& blurry_threshold = face_quality["blurry_threshold"];
                        if (!blurry_threshold.isNull()) {
                            set_blurry_threshold(
                                    StringUtil::trim(blurry_threshold.asString()).c_str());
                        }
                        Json::Value& bright_threshold = face_quality["bright_threshold"];
                        if (!bright_threshold.isNull()) {
                            set_bright_threshold(
                                    StringUtil::trim(bright_threshold.asString()).c_str());
                        }
                        Json::Value& occlusion_threshold = face_quality["occlusion_threshold"];
                        if (!occlusion_threshold.isNull()) {
                            set_occlusion_threshold(
                                    StringUtil::trim(occlusion_threshold.asString()).c_str());
                        }
                        Json::Value& confidence_threshold = face_quality["confidence_threshold"];
                        if (!confidence_threshold.isNull()) {
                            set_confidence_threshold(
                                    StringUtil::trim(confidence_threshold.asString()).c_str());
                        }
                        Json::Value& interval = face_quality["min_frame_interval"];
                        if (!interval.isNull()) {
                            set_min_frame_interval(
                                   StringUtil::trim(interval.asString()).c_str());
                        }
                        Json::Value& quality_duration = face_quality["quality_duration_in_ms"];
                        if (!quality_duration.isNull()) {
                            set_quality_duration(
                                   StringUtil::trim(quality_duration.asString()).c_str());
                        }
                        Json::Value& size_weight = face_quality["quality_size_weight"];
                        if (!size_weight.isNull()) {
                            set_quality_size_weight(
                                   StringUtil::trim(size_weight.asString()).c_str());
                        }
                        Json::Value& angle_weight = face_quality["quality_angle_weight"];
                        if (!angle_weight.isNull()) {
                            set_quality_angle_weight(
                                   StringUtil::trim(angle_weight.asString()).c_str());
                        }
                    }

                    Json::Value& push_img = conf["push_img"];
                    if (!push_img.isNull()) {
                        Json::Value& server_to_push = recognize["server_to_push"];
                        Json::Value& resize_ratio = recognize["resize_ratio"];
                        Json::Value& periold_in_ms = recognize["periold_in_ms"];
                        Json::Value& quality = recognize["quality"];
                        if (!server_to_push.isNull()) {
                            set_server_to_push(
                                 StringUtil::trim(server_to_push.asString()).c_str());
                        }
                        if (!resize_ratio.isNull()) {
                            set_resize_ratio_push_img(
                                StringUtil::trim(resize_ratio.asString()).c_str());
                        }
                        if (!periold_in_ms.isNull()) {
                            set_duration_push_img_in_ms(
                                StringUtil::trim(periold_in_ms.asString()).c_str());
                        }
                        if (!quality.isNull()) {
                            set_quality_push_img(StringUtil::trim(quality.asString()).c_str());
                        }
                    }
                    Json::Value& enable_fps_barrier = conf["enable_fps_barrier"];
                    if (!enable_fps_barrier.isNull()) {
                        set_enable_fps_barrier(enable_fps_barrier.asBool());
                    }
                }
            }
        } else {
            AIP_LOG_FATAL("parse json failed");

        }
    }
    return 0;
}

void Config::set_is_save_bkimg(int save){
    this->_is_save_bkimgs = save;
}

void Config::set_bkimg_resize_ratio(float ratio) {
    this->_bkimg_resize_ratio = ratio;
}

void Config::set_enable_face_detect(int enable_face_detect){
    this->_enable_face_detect = enable_face_detect;
}

int Config::is_enable_face_detect(){
    return this->_enable_face_detect;
}

void Config::enable_save_faces_detected(int enable_save_facesdetected){
    this->_enable_save_facesdetected = enable_save_facesdetected;
}

int Config::is_save_faces_detected(){
    return this->_enable_save_facesdetected;
}

void Config::enable_save_frames_detected(int enable_save_framesdetected){
    this->_enable_save_framesdetected = enable_save_framesdetected;
}

int Config::is_save_frames_detected(){
    return this->_enable_save_framesdetected;
}

void Config::set_enable_compatiblity(bool enable_compatiblity){
    this->_enable_compatiblity_with_previous = enable_compatiblity;
}

bool Config::is_enable_compatiblity(){
    return this->_enable_compatiblity_with_previous;
}

void Config::set_enable_push_img(int enable_push_img){
    this->_enable_push_img = enable_push_img;
}

void Config::set_is_publish(int publish) {
    this->_publish.is_publish = publish;
}

void Config::set_publish_url(const char* optarg) {
    this->_publish.publish_url = optarg;
}

void Config::set_face_track_count(const char* optarg) {
    this->_face_track_count = string_to_int(optarg);
}

void Config::set_max_track_fps_when_no_face(const char* optarg) {
    int val = string_to_int(optarg);
    if (val <= 0) {
        val = 5;
    }
    if (val >= 25) {
        val = 25;
    }
    this->_max_track_fps_when_no_face = val;
}

int Config::get_max_track_fps_when_no_face() {
    return this->_max_track_fps_when_no_face;
}

void Config::set_save_images_count(const char* optarg) {
    this->_save_images_count = string_to_int(optarg);        
}

int Config::get_face_track_count() {
    return this->_face_track_count;
}
    
int Config::get_save_images_count() {
    return this->_save_images_count;
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
        case OPT_VERSION:
            return 1;

        case OPT_ID: {
            set_id(cleaned_optarg);
        }
        break;
        case OPT_GPU_ID: {
            set_gpu_id(cleaned_optarg);
        }
        break;
        case OPT_SOURCE: {
            set_source(cleaned_optarg);
        }
        break;
        case OPT_DST: {
            set_dst(cleaned_optarg);
        }
        break;
        case OPT_DELAY_IN_MS: {
            set_delay(cleaned_optarg);
        }
        break;
        case OPT_REC_MASK: {
            set_mask(cleaned_optarg);
        }
        break;
        case OPT_REC_SRV: {
            set_recognize_server(cleaned_optarg);
        }
        break;
        case OPT_FACE_TRAIL_SRV: {
            set_face_trail_server(cleaned_optarg);
        }
        break;
        case OPT_WORKER_SRV: {
            set_worker_server(cleaned_optarg);
        }
        break;
        case OPT_REC_TIMES_PER_FACE: {
            set_recognize_times_per_face(cleaned_optarg);
        }
        break;
        case OPT_IS_ORG_REC: {
            set_is_original_recording(cleaned_optarg);
        }
        break;
        case OPT_ORG_REC_LOC: {
            set_original_recording_location(cleaned_optarg);
        }
        break;
        case OPT_ORG_REC_MAX_DUR: {
            set_original_recording_max_duration(cleaned_optarg);
        }
        break;
        case OPT_ORG_REC_HLS_DUR: {
            set_original_recording_hls_duration(cleaned_optarg);
        }
        break;
        case OPT_IS_FACED_REC: {
            set_is_faced_recording(cleaned_optarg);
        }
        break;
        case OPT_FACED_REC_LOC: {
            set_faced_recording_location(cleaned_optarg);
        }
        break;
        case OPT_FACED_REC_MAX_DUR: {
            set_faced_recording_max_duration(cleaned_optarg);
        }
        break;
        case OPT_FACED_REC_HLS_DUR: {
            set_faced_recording_hls_duration(cleaned_optarg);
        }
        break;
        case OPT_IS_PUBLISH: {
            set_is_publish(StringUtil::to_bool(cleaned_optarg));
        }
        break;
        case OPT_PUBLISH_URL: {
            set_publish_url(cleaned_optarg);
        }
        break;
        case OPT_MIN_FACE: {
            set_minimum_face(cleaned_optarg);
        }
        break;
        case OPT_PITCH_THRES: {
            set_pitch_threshold(cleaned_optarg);
        }
        break;
        case OPT_YAW_THRES: {
            set_yaw_threshold(cleaned_optarg);
        }
        break;
        case OPT_ROLL_THRES: {
            set_roll_threshold(cleaned_optarg);
        }
        break;
        case OPT_BLURRY_THRES: {
            set_blurry_threshold(cleaned_optarg);
        }
        break;
        case OPT_BRIGHT_THRES: {
            set_bright_threshold(cleaned_optarg);
        }
        break;
        case OPT_OCCLUSION_THRES: {
            set_occlusion_threshold(cleaned_optarg);
        }
        break;
        case OPT_CONFIDENCE_THRES: {
            set_confidence_threshold(cleaned_optarg);
        }
        break;
        case OPT_LIVE: {
            set_is_live(StringUtil::to_bool(cleaned_optarg));
        }
        break;
        case OPT_FACE_TRACK_COUNT: {
            set_face_track_count(cleaned_optarg);
        }
        break;
        case OPT_MAX_TRACK_FPS_WHEN_NO_FACE: {
            set_max_track_fps_when_no_face(cleaned_optarg);
        }
        break;
        case OPT_SAVE_IMAGE_COUNT: {
            set_save_images_count(cleaned_optarg);
        }
        break;
        case OPT_MIN_FRAME_INTERVAL: {
            set_min_frame_interval(cleaned_optarg);
        }
        break;
        case OPT_QUALITY_DURATION_IN_MS: {
            set_quality_duration(cleaned_optarg);                           
        }
        break;
        case OPT_QUALITY_SIZE_WEIGHT:{
            set_quality_size_weight(cleaned_optarg);
        }
        break;
        case OPT_QUALITY_ANGLE_WEIGHT: {
            set_quality_angle_weight(cleaned_optarg);
        }
        break;
        case OPT_HEARTBEAT_INTERVAL: {
            set_heartbeat_interval(cleaned_optarg);
        }
        break;
        case OPT_SAVE_BKIMG: {
            set_is_save_bkimg(StringUtil::to_bool(cleaned_optarg));
        }
        break;
        case OPT_BKIMG_RESIZE_RATIO: {
            set_bkimg_resize_ratio(string_to_float(cleaned_optarg));
        }
        break;
        case OPT_BKIMG_DIR: {
            set_bkimg_dir(cleaned_optarg);
        }
        break;
        case OPT_PUSH_IMG: {
            set_enable_push_img(StringUtil::to_bool(cleaned_optarg));
        }
        break;
        case OPT_SERVER_PUSH_IMG: {
            set_server_to_push(cleaned_optarg);
        }
        break;
        case OPT_DURATION_PUSH_IMG: {
            set_duration_push_img_in_ms(cleaned_optarg);
        }
        break;
        case OPT_RESIZE_RATIO_PUSH_IMG: {
            set_resize_ratio_push_img(cleaned_optarg);
        }
        break;
        case OPT_QULITY_PUSH_IMG: {
            set_quality_push_img(cleaned_optarg);
        }
        break;
        case OPT_ENABLE_FACE_DETECT: {
            set_enable_face_detect(StringUtil::to_bool(cleaned_optarg));
        }
        break;
        case OPT_ENABLE_FPS_BARRIER: {
            set_enable_fps_barrier(StringUtil::to_bool(cleaned_optarg));
        }
        break;
        case OPT_SAVED_FACEIMG_NOTEXT: {
            set_savedfaceimg_notext_flag();
        }
        break;
        case OPT_COMPATIBLITY: {
            set_enable_compatiblity(StringUtil::to_bool(cleaned_optarg));
        }
        break;
        case OPT_NATIVE_FRAME_RATE: {
            set_native_framerate(cleaned_optarg);
        }
        break;
        case OPT_MAX_FRAMERATE_TIMES: {
            set_max_framerate_times(cleaned_optarg);
        }
        break;
        case OPT_VIDEO_CUTAPART_RATIO: {
            set_video_cutapart_ratio(cleaned_optarg);
        }
        break;
        case OPT_VIDEO_START_TIME: {
            set_video_starttime(cleaned_optarg);
        }
        break;
        case OPT_VIDEO_OFFSET_TIME: {
            set_video_offsettime(cleaned_optarg);
        }
        break;
        case OPT_READ_FILE_ONCE: {
            set_readfile_loop_flag(StringUtil::to_bool(cleaned_optarg));
        }
        break;
        case OPT_ENABLE_SAVE_FACESDETECTED: {
            enable_save_faces_detected(StringUtil::to_bool(cleaned_optarg));
        }
        break;
        case OPT_ENABLE_SAVE_FRAMESDETECTED: {
            enable_save_frames_detected(StringUtil::to_bool(cleaned_optarg));
        }
        break;
        case OPT_IMGNUM_PER_TRACK: {
            set_imgnum_per_track(cleaned_optarg);
        }
        break;
        case OPT_IMGNUM_SORTBY_WEIGHTSUM: {
            set_imgnum_sortby_weightsum(cleaned_optarg);
        }
        break;
        case OPT_LOG_FOR_EVALUATION: {
            set_log_forevaluation_flag();
        }
        break;
        case OPT_GAT1400_SERVERURL: {
            set_gat1400_serverurl(cleaned_optarg);
        }
        break;
        case OPT_GAT1400_USERNAME: {
            set_gat1400_username(cleaned_optarg);
        }
        break;
        case OPT_GAT1400_PASSWORD: {
            set_gat1400_password(cleaned_optarg);
        }
        break;
        case OPT_GPU_DECODECONFIGRATION: {
            set_gpudecode_configuration(cleaned_optarg);
        }
        break;
        case OPT_GAT1400_GBID: {
            set_gb_id(cleaned_optarg);
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

void Config::set_id(const char* optarg) {
    _id = ensure_valid_string((char*)optarg);
}

void Config::set_gpu_id(const char* optarg) {
    _gpu_id = string_to_int(optarg);
}

void Config::set_source(const char* optarg) {
    _source = optarg;
}

void Config::set_dst(const char *optarg) {
    _dst = optarg;
}

void Config::set_delay(const char* optarg) {
    int opt = string_to_int(optarg);
    if (opt < 0) {
        opt = 0;
    } else if (opt > MAX_DELAY) {
        opt = MAX_DELAY;
    }
    _delay_in_ms = opt;
}

void Config::set_is_live(int live) {
    _live = live;
}

int Config::is_live() {
    return _live;
}

void Config::set_mask(const char* optarg) {

}

void Config::set_recognize_server(const char* optarg) {
    _recognize.recognize_server = optarg;
}

void Config::set_face_trail_server(const char* optarg) {
    _recognize.face_trail_server = optarg;
}

void Config::set_recognize_times_per_face(const char* optarg) {
    int opt = string_to_int(optarg);

    if (opt <= 0) {
        opt = 3;
    }

    _recognize.times_per_face = opt;
}

void Config::set_min_frame_interval(const char* optarg) {
    int opt = string_to_int(optarg);

    if (opt < 0) {
        opt = 5;
    }
    this->_face_quality.min_frame_interval = opt;
}

void Config::set_quality_duration(const char* optarg) {
    int opt = string_to_int(optarg);

    this->_face_quality.time_to_wait_in_ms = opt;
}

void Config::set_quality_size_weight(const char* optarg) {
    float opt = string_to_float(optarg);

    if (opt < 0) {
        opt = 0.0f;
    } else if (opt > 1.0f) {
        opt = 1.0f;
    }

    this->_face_quality.size_weight = opt;
}

void Config::set_quality_angle_weight(const char* optarg) {
    float opt = string_to_float(optarg);

    if (opt < 0) {
        opt = 0.0f;
    } else if (opt > 1.0f) {
        opt = 1.0f;
    }

    this->_face_quality.angle_weight = opt;
}

void Config::set_is_original_recording(const char* optarg) {

}
void Config::set_original_recording_location(const char* optarg) {

}
void Config::set_original_recording_max_duration(const char* optarg) {

}
void Config::set_original_recording_hls_duration(const char* optarg) {

}
void Config::set_is_faced_recording(const char* optarg) {

}
void Config::set_faced_recording_location(const char* optarg) {

}
void Config::set_faced_recording_max_duration(const char* optarg) {

}
void Config::set_faced_recording_hls_duration(const char* optarg) {

}

void Config::set_minimum_face(const char* optarg) {
    int size = string_to_int(optarg);

    if (size <= 0) {
        size = 80;
    }

    _face_quality.minimum_face_in_pixels = size;
}
void Config::set_pitch_threshold(const char* optarg) {
    int low = 0;
    int high = 0;

    if (parse_range(optarg, &low, &high) == 0) {
        _face_quality.pitch_threshold.low = low;
        _face_quality.pitch_threshold.high = high;
    }
}

void Config::set_yaw_threshold(const char* optarg) {
    int low = 0;
    int high = 0;

    if (parse_range(optarg, &low, &high) == 0) {
        _face_quality.yaw_threshold.low = low;
        _face_quality.yaw_threshold.high = high;
    }
}
void Config::set_roll_threshold(const char* optarg) {
    int low = 0;
    int high = 0;

    if (parse_range(optarg, &low, &high) == 0) {
        _face_quality.roll_threshold.low = low;
        _face_quality.roll_threshold.high = high;
    }
}

void Config::set_blurry_threshold(const char* optarg) {
    float opt = string_to_float(optarg);

    if (opt < 0) {
        opt = 0.5f;
    }

    if (opt > 1.0f) {
        opt = 1.0f;
    }

    _face_quality.blurry_threshold = opt;
}

void Config::set_first_blurry_threshold(const char* optarg) {
    float opt = string_to_float(optarg);

    if (opt < 0) {
        opt = 0.3f;
    }

    if (opt > 1.0f) {
        opt = 1.0f;
    }

    _face_quality.first_blurry_threshold = opt;
}

void Config::set_bright_threshold(const char* optarg) {
    float opt = string_to_float(optarg);

    if (opt < 0) {
        opt = 40.0f;
    }

    if (opt > 256.0f) {
        opt = 256.0f;
    }

    _face_quality.bright_threshold = opt;
}
void Config::set_occlusion_threshold(const char* optarg) {
    float opt = string_to_float(optarg);

    if (opt < 0) {
        opt = 0.2f;
    }

    if (opt > 1.0f) {
        opt = 1.0f;
    }

    _face_quality.occlusion_threshold = opt;
}

void Config::set_confidence_threshold(const char* optarg) {
    float opt = string_to_float(optarg);

    if (opt < 0) {
        opt = 0.3f;
    }

    if (opt > 1.0f) {
        opt = 1.0f;
    }

    _face_quality.confidence_threshold = opt;
}

const std::string& Config::get_id() {
    return this->_id;
}

const std::string& Config::get_source() {
    return this->_source;
}

const std::string &Config::get_dst() {
    return this->_dst;
}

int Config::get_delay() {
    return this->_delay_in_ms;
}

const conf_recognize_t& Config::get_recognize() {
    return this->_recognize;
}

int Config::get_gpu_id() {
    return this->_gpu_id;
}

const conf_recording_t& Config::get_original_recording() {
    return this->_original_recording;
}

const conf_recording_t& Config::get_faced_recording() {
    return this->_faced_recording;
}

const conf_publish_t& Config::get_publish() {
    return this->_publish;
}

const conf_face_quality_t& Config::get_face_quality() {
    return this->_face_quality;
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

std::string Config::to_string() {
    std::stringstream builder;
    builder << "camera id:            " << get_id() << std::endl;
    builder << "gpu id:               " << get_gpu_id() << std::endl;
    builder << "source:               " << get_source() << std::endl;
    builder << "delay:                " << get_delay() << std::endl;
    builder << "max fps when no face: " << get_max_track_fps_when_no_face() << std::endl;
    builder << "heartbeat interval:   " << get_heartbeat_interval() << std::endl;
    builder << "recognize:" << std::endl;
    builder << "\t: server:           " << get_recognize().recognize_server << std::endl;
    builder << "\t: face_trail_server:" << get_recognize().face_trail_server << std::endl;
    builder << "\t: worker server     " << get_recognize().worker_server << std::endl;
    builder << "\t: times per face:   " << get_recognize().times_per_face << std::endl;
    
    builder << "publish:" << std::endl;
    builder << "\t: is_publish:       " << get_publish().is_publish << std::endl;
    builder << "\t: publish_url:      " << get_publish().publish_url << std::endl;
    
    builder << "face quality:" << std::endl;
    builder << "\t: face size:        " << get_face_quality().minimum_face_in_pixels << std::endl;
    builder << "\t: pitch:            " << get_face_quality().pitch_threshold.low << "," 
            << get_face_quality().pitch_threshold.high << std::endl;
    builder << "\t: yaw:              " << get_face_quality().yaw_threshold.low << ","
            << get_face_quality().yaw_threshold.high << std::endl;
    builder << "\t: roll:             " << get_face_quality().roll_threshold.low << ","
            << get_face_quality().roll_threshold.high << std::endl;
    builder << "\t: first blurry:     " << get_face_quality().first_blurry_threshold << std::endl;
    builder << "\t: blurry:           " << get_face_quality().blurry_threshold << std::endl;
    builder << "\t: bright:           " << get_face_quality().bright_threshold << std::endl;
    builder << "\t: occlusion:        " << get_face_quality().occlusion_threshold << std::endl;
    builder << "\t: confidence:       " << get_face_quality().confidence_threshold << std::endl;
    builder << "\t: frame interval    " << get_face_quality().min_frame_interval << std::endl;
    builder << "\t: wait time         " << get_face_quality().time_to_wait_in_ms << std::endl;
    builder << "\t: size weight       " << get_face_quality().size_weight << std::endl;
    builder << "\t: angle weight      " << get_face_quality().angle_weight << std::endl;
    builder << "save_bkimg           \t" << is_save_bkimg() << std::endl;
    builder << "bkimg_size           \t" << _bkimg_resize_ratio << std::endl;
    builder << "bkimg_dir            \t" << _bkimg_dir << std::endl;
    return builder.str();
}

void Config::set_heartbeat_interval(const char* optarg) {
    int val = string_to_int(optarg);
    if (val <= 0) {
        val = 15000;
    } else if (val > 300000) {
        val = 300000;
    }
    
    if (val <= 5000) {
        val = 5000;
    }
    
    this->_heartbeat_interval= val;
}
    
int Config::get_heartbeat_interval() {
    return this->_heartbeat_interval;
}

int Config::is_save_bkimg() {
    return this->_is_save_bkimgs;
}
    
float Config::get_bkimg_resize_ratio() {
    return this->_bkimg_resize_ratio;
}

void Config::set_bkimg_dir(const char* dir) {
    this->_bkimg_dir = FileUtil::normalize_file_path(dir);    
}

const conf_push_img_t & Config::get_conf_push_img() {
    return this->_push_img;
}

int Config::is_enable_push_img(){
    return this->_enable_push_img;
}

void Config::set_server_to_push(const char* optarg) {
    _push_img.push_img_server = optarg;
}

const std::string& Config::get_server_to_push() {
    return _push_img.push_img_server;
}

void Config::set_duration_push_img_in_ms(const char* optarg) {
    int val = string_to_int(optarg);
    // TODO(if val)
    if (val <= 0) {
        val = 2000;
    }

    _push_img.duration_in_mseconds = val;
}

int Config::get_duration_push_img_in_ms() {
    return _push_img.duration_in_mseconds;
}

void Config::set_resize_ratio_push_img(const char* optarg) {
    int val = string_to_float(optarg);
    // TODO(if val)
    if (val <= 0) {
        val = 0.5f;
    } else if (val > 1.0f) {
        val = 1.0f;
    }

    _push_img.img_resize_ratio = val;
}

float Config::get_resize_ratio_push_img() {
    return _push_img.img_resize_ratio;
}

void Config::set_quality_push_img(const char* optarg) {
    int val = string_to_int(optarg);
    // TODO(if val is too small)
    if (val <= 0) {
        val = 80;
    } else if (val > 100) {
        val = 100;
    }

    _push_img.img_quality = val;
}

int Config::get_quality_push_img() {
    return _push_img.img_quality;
}

const std::string& Config::get_bkimg_dir() {
    return this->_bkimg_dir;
}

void Config::set_worker_server(const char* server) {
    this->_recognize.worker_server = server;
}

int Config::check_server_uri() {
    if (is_enable_push_img() == 1) {
        UrlParser push_img_server_parser(get_server_to_push());
        if (push_img_server_parser.get_uri().empty()) {
            printf("The uri of push image server is null!\n");
            return -1;
        }
    }

    if (!is_enable_gat1400() &&
        is_enable_face_detect() == 1) {
        UrlParser face_recog_server_parser(get_recognize().recognize_server);
        UrlParser face_trail_server_parser(get_recognize().face_trail_server);
        if (!face_recog_server_parser.get_uri().empty()) {
            set_enable_face_recognize(1);
        } else if (!face_trail_server_parser.get_uri().empty()) {
            set_enable_face_recognize(0);
        } else {
            printf("The uri of face server is null!\n");
            return -1;
        }
    }

    UrlParser heartbeat_server_parser(get_recognize().worker_server);
    if (!is_enable_gat1400() &&
        heartbeat_server_parser.get_uri().empty()) {
        printf("The uri of heartbeat server is null!\n");
        return -1;
    }

    return 0;
}

void Config::set_enable_face_recognize(int enable_face_recognize){
    this->_enable_face_recognize = enable_face_recognize;
}

int Config::is_enable_face_recognize() {
    return _enable_face_recognize;
}

void Config::set_enable_fps_barrier(int enable_fps_barrier) {
    this->_enable_fps_barrier = enable_fps_barrier;
}

int Config::is_enable_fps_barrier() {
    return _enable_fps_barrier;
}

void Config::set_savedfaceimg_notext_flag() {
    this->_savedfaceimg_notext_flag = 1;
}

int Config::get_savedfaceimg_notext_flag() {
    return this->_savedfaceimg_notext_flag;
}

void Config::set_native_framerate(const char* optarg) {
    float val = string_to_float(optarg);

    if (val <= FLT_EPSILON) {
        val = 0.0f;
    } else if (val > _max_framerate_times) {
        val = _max_framerate_times;
    }

    this->_native_read_framerate = val;
}

bool Config::get_nativefile_readflag() {
    return this->_read_nativefile_flag;
}

float Config::get_native_framerate() {
    return this->_native_read_framerate;
}

void Config::set_max_framerate_times(const char* optarg) {
    float val = string_to_float(optarg);

    if (val <= FLT_EPSILON) {
        val = 0.0f;
    }

    this->_max_framerate_times = val;
}

void Config::set_video_cutapart_ratio(const char* optarg) {
    std::vector<std::string> results;
    StringUtil::split(std::string(optarg), '/',  &results);
    if (results.size() != 2) {
        AIP_LOG_FATAL("set_video_cutapart_ratio failed, input %s!",
                      optarg);
        return;
    }

    std::string part_sum_str = StringUtil::trim(results[0]);
    std::string part_num_str = StringUtil::trim(results[1]);
    this->_cutapart_video_sum = atoi(part_sum_str.c_str());
    this->_cutapart_video_num = atoi(part_num_str.c_str());

    if (this->_cutapart_video_sum > 0 &&
        this->_cutapart_video_num > 0 &&
        this->_cutapart_video_sum >= this->_cutapart_video_num) {
        AIP_LOG_NOTICE("set_video_cutapart_ratio successfully!");
        this->_cutapart_video_flag = true;
    } else {
        AIP_LOG_FATAL("set_video_cutapart_ratio failed, sum %d num %d!",
                      this->_cutapart_video_sum,
                      this->_cutapart_video_num);
    }
}

bool Config::get_video_cutapart_flag() {
    return this->_cutapart_video_flag;
}

int Config::get_video_cutapart_ratio(int *part_sum, int *part_num) {
    *part_sum = this->_cutapart_video_sum;
    *part_num = this->_cutapart_video_num;

    return 0;
}

void Config::set_video_starttime(const char * optarg) {
    int val = string_to_int(optarg);

    if (val <= 0) {
        val = 0;
    }

    this->_video_starttime_ms = val;
}

int64_t Config::get_video_starttime() {
    return this->_video_starttime_ms;
}

void Config::set_video_offsettime(const char * optarg) {
    int val = string_to_int(optarg);

    if (val <= 0) {
        val = 0;
    }

    this->_offset_time_ms = val;
}

int64_t Config::get_video_offsettime() {
    return this->_offset_time_ms;
}

void Config::set_readfile_loop_flag(int loop_flag) {
    if (loop_flag == 1) {
        this->_readfile_loop_flag = false;
        this->_read_nativefile_flag = true;
    }
}

bool Config::get_readfile_loop_flag() {
    return this->_readfile_loop_flag;
}

void Config::set_imgnum_per_track(const char* optarg) {
    int val = string_to_int(optarg);

    if (val <= 0) {
        val = 1;
    }

    this->_enable_max_strategy = true;
    this->_imgnum_per_track = val;
}

size_t Config::get_imgnum_per_track() {
    return this->_imgnum_per_track;
}

bool Config::is_enable_max_strategy() {
    return this->_enable_max_strategy;
}

void Config::set_imgnum_sortby_weightsum(const char* optarg) {
    int val = string_to_int(optarg);

    if (val <= 0) {
        this->_enable_weightsum_sort = false;
        this->_imgnum_sortby_weightsum = 0;
    } else {
        this->_enable_weightsum_sort = true;
        this->_imgnum_sortby_weightsum = val;
    }
}

size_t Config::get_imgnum_sortby_weightsum() {
    return this->_imgnum_sortby_weightsum;
}

bool Config::is_enable_weightsum_sort() {
    return this->_enable_weightsum_sort;
}

void Config::set_log_forevaluation_flag() {
    this->_log_forevaluation_flag = true;
}

bool Config::get_log_forevaluation_flag() {
    return this->_log_forevaluation_flag;
}

void Config::set_video_framerate(int frame_rate) {
    this->_video_frame_rate = frame_rate;
    if (frame_rate > 0) {
        set_video_frameinterval();
    }
}

int Config::get_video_framerate() {
    return this->_video_frame_rate;
}

// ms
void Config::set_video_frameinterval() {
    this->_video_frame_interval = 1000 / this->_video_frame_rate;
}

int Config::get_video_frameinterval() {
    return this->_video_frame_interval;
}

bool Config::is_enable_gat1400() {
    return _enable_gat1400;
}

void* Config::set_gat1400_serverurl(const char* optarg) {
    _gat1400_serverurl = optarg;
    if (!_gat1400_serverurl.empty()) {
        _enable_gat1400 = true;
    }
}

std::string Config::get_gat1400_serverurl() {
    return _gat1400_serverurl;
}

void* Config::set_gat1400_username(const char* optarg) {
    _gat1400_username = optarg;
}

std::string Config::get_gat1400_username() {
    return _gat1400_username;
}

void* Config::set_gat1400_password(const char* optarg) {
    _gat1400_password = optarg;
}

std::string Config::get_gat1400_password() {
    return _gat1400_password;
}

void* Config::set_gpudecode_configuration(const char* optarg) {
    int gpu_id = 0;
    std::vector<std::string> split_result;

    StringUtil::split(std::string(optarg), ',', &split_result);
    std::vector<std::string>::iterator iter = split_result.begin();
    for (; iter != split_result.end(); ++iter, ++gpu_id) {
        int num = atoi(StringUtil::space_trim(*iter).c_str());
        if (num > 0) {
            _gpu_decode_conf.insert(std::make_pair(gpu_id, num));
        }
    }
}

static int lock_gpudecode_reg(int fd, int cmd, int type, off_t offset,
                                   int whence, off_t len) {
    struct flock lock = {0};
    lock.l_type = type; /* F_RDLCK, F_WRLCK, F_UNLCK */
    lock.l_start = offset; /* byte offset, relative to l_whence */
    lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END */
    lock.l_len = len; /* #bytes (0 means to EOF) */
    return (fcntl(fd, cmd, &lock));
}

static int gpu_decode_lock(int gpu_id, int index, int *loadgpu_fd) {
    int fd = -1;
    std::stringstream ss;
    std::string path("gpu_decode_lock");
    mkdir(path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH);
    ss << "gpu_decode_lock/gpu" << gpu_id << "_" << index << "_lock";
    fd = open(ss.str().c_str(), O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        return -1;
    }

    if (lock_gpudecode_reg(fd, F_SETLK, F_WRLCK, 0, SEEK_SET, 0) < 0) {
        close(fd);
        printf("lock_gpudecode failed!\n");
        return 1;
    }
    *loadgpu_fd = fd;

    return 0;
}

static int gpu_decode_unlock(int *loadgpu_fd) {
    if (*loadgpu_fd <= 0) {
        return -1;
    }

    if (lock_gpudecode_reg(*loadgpu_fd, F_SETLK, F_UNLCK, 0, SEEK_SET, 0) < 0) {
        close(*loadgpu_fd);
        *loadgpu_fd = 0;
        printf("unlock_gpudecode failed!\n");
        return 1;
    }
    close(*loadgpu_fd);
    *loadgpu_fd = 0;

    return 0;
}

int Config::get_gpudecode_id() {
    int gpu_id = -1;
    int max_index = 0;
    int lock_fd = 0;

    if (_gpu_decode_conf.empty()) {
        return gpu_id;
    }

    std::map<int, int>::iterator iter = _gpu_decode_conf.begin();
    for (; iter != _gpu_decode_conf.end(); ++iter) {
        max_index = std::max(max_index, iter->second);
    }

    for (int i = 0; i < max_index; ++i) {
        std::map<int, int>::iterator iter_lock = _gpu_decode_conf.begin();
        for (; iter_lock != _gpu_decode_conf.end(); ++iter_lock) {
            if (i < iter_lock->second &&
                gpu_decode_lock(iter_lock->first, i, &lock_fd) == 0) {
                gpu_id = iter_lock->first;
                return gpu_id;
            }
        }
    }

    return gpu_id;
}

void Config::set_gb_id(const char* optarg) {
    _gb_id = ensure_valid_string((char*)optarg);
}

const std::string& Config::get_gb_id() {
    return this->_gb_id;
}
