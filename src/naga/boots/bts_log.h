#ifndef __BTS_LOG_H
#define __BTS_LOG_H


#include <syslog.h>
#include <stdarg.h>

#define LOG_BUFFER 4096


void bts_log_init(char *param);
void bts_log_deinit(void);

void bts_log(int priority, const char *fmt, ...);

#define bts_alert(fmt, args...) \
    bts_log(LOG_ALERT, "[ALERT] " fmt, ##args)

#define bts_err(fmt, args...) \
    bts_log(LOG_ERR, "[ERR] " fmt, ##args)

#define bts_warn(fmt, args...) \
    bts_log(LOG_WARNING, "[WARNING]<%s> " fmt, ##args)

#define bts_noti(fmt, args...) \
    bts_log(LOG_NOTICE, "[NOTICE]<%s> " fmt, ##args)

#define bts_info(fmt, args...) \
    bts_log(LOG_INFO, "[INFO]<%s> " fmt, ##args)

#define bts_debug(fmt, args...) \
    bts_log(LOG_DEBUG, "[DEBUG]<%s> %s.%d: "fmt, __func__, __LINE__, ##args); 


#define bts_zlog(priority, format , args...)  zlog(NULL,  priority, format,  ##args)

#endif
