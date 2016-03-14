#ifndef __ADP_TEMPLATE_H__
#define __ADP_TEMPLATE_H__

typedef struct {
    uint8_t idx;
    uint8_t url_set;
    uint16_t len;
    char buff[1024];
    uint64_t cnt;
} acr_template_t;

#endif
