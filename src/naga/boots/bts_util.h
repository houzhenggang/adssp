#ifndef  __BTS_UTIL_H__
#define  __BTS_UTIL_H__
int bts_ipaddr_cmp(void *val1, void *val2);
int bts_str_cmp(void *val1, void * val2);
void bts_ip_string(uint32_t ip, char *str);

#define BSWT_STR_SZ 6
berr bswt_parse(char *str, bswt *val);
void bswt_string(bswt *val, char *str);

#endif /* end of __BTS_UTIL_H__ */
