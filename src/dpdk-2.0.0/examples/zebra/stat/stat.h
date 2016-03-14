#ifndef __STAT_H__
#define __STAT_H__
typedef enum {
    ITF_IPKTS,
    ITF_IBYTS,
    ITF_OPKTS,
    ITF_OBYTS,


    /*VSR Module */
    VSR_PKTS, /* VSR module input packets*/
    VSR_UNURLPKTS, /* take no url packet number */
    VSR_URLPKTS, /* take url packet number */
    VSR_UNMATCHPKTS, /* not match packets */
    VSR_MATCHPKTS, /* match packets */
    VSR_RULE_MATCHPKTS, /* rule ip, url all match */
    CNT_MAX
} cnte;

typedef enum {
    cnte idx,
    char *name;
    uint64_t val;
} cnt_t;

#define CNT_DEC (_cnt) {_cnt, #_cnt}
cnt_t cnt_array[CNT_MAX] = {
    CNT_DEC(ITF_IPKTS),
    CNT_DEC(ITF_IBYTS),
    CNT_DEC(ITF_OPKTS),
    CNT_DEC(ITF_OBYTS),

    /* VSR Module */
    CNT_DEC(VSR_PKTS),
    CNT_DEC(VSR_UNURLPKTS),
    CNT_DEC(VSR_URLPKTS),
    CNT_DEC(VSR_UNMATCHPKTS),
    CNT_DEC(VSR_MATCHPKTS),
    CNT_DEC(VSR_RULE_MATCHPKTS),
};

// 其他模块调用
#define CNT_INC(_cnt) rte_atomic64_inc(&cnt_array[_cnt].val)
#define CNT_ADD(_cnt, _val) rte_atomic64_inc(&cnt_array[_cnt].val, _val)

//  // 下面接口只供管理模块和主流程调用
berr cnt_add(cnte idx, uint64_t value);
berr cnt_get(cnte idx, uint32_t number, cnt_t *cnts, uint32_t *sum);
berr cnt_clear(cnte idx, uint32_t number, uint32_t *sum);
berr cnt_int();
#endif /* end of __STAT_H__*/
