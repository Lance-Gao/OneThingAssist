#ifndef UTILS_AIP_LOG_HPP
#define UTILS_AIP_LOG_HPP

#include "aip_common.hpp"
#include <logapi.hpp>

#define AIP_DEBUG(...) \
  printf("%s: Line %d: \t", __FILE_, __LINE__);\
  printf(## __VA_ARGS__);\
  printf("\n");
  
#if 0
#define AIP_LOG_DEBUG(fmt, ...) \
    printf(fmt, ## __VA_ARGS__); \
    printf("\n")

#define AIP_LOG_TRACE(fmt, ...) \
    printf(fmt, ## __VA_ARGS__); \
    printf("\n")

#define AIP_LOG_NOTICE(fmt, ...) \
    printf(fmt, ## __VA_ARGS__); \
    printf("\n")

#define AIP_LOG_WARNING(fmt, ...) \
    printf(fmt, ## __VA_ARGS__); \
    printf("\n")

#define AIP_LOG_FATAL(fmt, ...) \
    printf(fmt, ## __VA_ARGS__); \
    printf("\n")
#else 
#define AIP_LOG_DEBUG(fmt, ...) \
    log_debug("facetracer", "facetracer", fmt, ## __VA_ARGS__)
    
#define AIP_LOG_TRACE(fmt, ...) \
    log_info("facetracer", "facetracer", fmt, ## __VA_ARGS__)
 
#define AIP_LOG_NOTICE(fmt, ...) \
    log_notice("facetracer", "facetracer", fmt, ## __VA_ARGS__)

#define AIP_LOG_WARNING(fmt, ...) \
    log_warning("facetracer", "facetracer", fmt, ## __VA_ARGS__)

#define AIP_LOG_FATAL(fmt, ...) \
    log_fatal("facetracer", "facetracer", fmt, ## __VA_ARGS__)
    
#endif 
#endif  // UTILS_AIP_LOG_HPP
