#ifndef __BTS_CNT_H__
#define __BTS_CNT_H__

#include "boots.h"
#include "boots_custom.h"

typedef enum {
      ITF0_IPKTS,
      ITF1_IPKTS,
      ITF_IPKTS,
      ITF_IBYTS,
      ITF_OPKTS,
      ITF_OBYTS,
      VLAN_PKTS,
	  OUTERL3_HD, /*Head Err, length not compare or infomation err*/
	  OUTERL3_IPV4,
	  OUTERL3_IPV6,
	  OUTERL3_NOIP,
	  
	  OUTERL4_HD,
	  OUTERL4_TCP,
	  OUTERL4_UDP,
	  OUTERL4_SCTP,

	  OUTERL4_OTHER,
	  INNERL3_HD,
	  INNERL3_IPV4,
	  INNERL3_IPV6,

	  INNERL3_NOIP,
	  INNERL4_HD,
	  INNERL4_TCP,
	  INNERL4_UDP,

	  INNERL4_SCTP,
	  INNERL4_OTHER,
	  GTP_U_HD,
	  GTP_U,

	  GTP_U_NOPAYLOAD,
	  GTPC_HD,
	  GTPV1_C,
	  GTPV2_C,
	  
	  APP_HTTP,
	  APP_OTHER,
	  HTTP_METHOD_EXCEED,
	  APP_HTTP_GET,
	  APP_HTTP_OTHER,
	  HTTP_URL_EXCEED,
	  ERR_PACKET,
	  HEADER_UNEXPECT,

#ifdef CNT_CUSTOM
      CNT_CUSTOM,
#endif


      CNT_MAX
} cnte;
 
typedef struct {
      cnte idx;
      char *name;
      bts_atomic64_t val;
} cnt_t;

//extern cnt_t cnt_array[CNT_MAX];
//extern dopt_t dopt_array[DOPT_MAX];
extern cnt_t cnt_array[CNT_MAX];



#define CNT_DEF(_cnt) {_cnt, #_cnt}

#define CNT_INC(_cnt)       bts_atomic64_inc(&(cnt_array[_cnt].val))
#define CNT_DEC(_cnt)       bts_atomic64_dec(&(cnt_array[_cnt].val))
#define CNT_ADD(_cnt, _val) bts_atomic64_add(&(cnt_array[_cnt].val), _val)
#define CNT_SET(_cnt, _val) bts_atomic64_set(&(cnt_array[_cnt].val), _val)
#define CNT_GET(_cnt)       bts_atomic64_get(&(cnt_array[_cnt].val))

berr cnt_add(cnte idx, uint64_t value);
berr cnt_inc(cnte idx);
berr cnt_dec(cnte idx);
berr cnt_get(cnte idx, uint32_t number, cnt_t *vals, uint32_t *total);
berr cnt_clear(cnte idx, uint32_t number, uint32_t *total);
berr cnt_int(void);




#endif
