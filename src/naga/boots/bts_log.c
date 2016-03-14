#include "bts_log.h"
#include <stdio.h>
#include <sys/types.h>

#include "bts_log.h"


static int log_opened = 0;

void bts_log(int priority, const char *fmt, ...) {

    static char buffer[1 + LOG_BUFFER];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buffer, LOG_BUFFER, fmt, ap);
    buffer[LOG_BUFFER] = '\0';
    va_end(ap);

    if ((priority < LOG_DEBUG)) {
        if (log_opened) {
            syslog(priority, "%s", buffer);
        } else {
            fprintf(stderr, "%s\n", buffer);
            fflush(stderr);
        }
    }
}

static void open_log(char *parm) {
    openlog(parm, LOG_CONS, LOG_LOCAL0);
    log_opened = 1;
}

static void close_log(void) {
    if(log_opened) {
        closelog();
        log_opened = 0;
    }
}

void bts_log_init(char *parm) {
    open_log(parm);
}

void bts_log_deinit(void) {
    close_log();
}




