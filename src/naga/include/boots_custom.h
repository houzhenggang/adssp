#ifndef __BOOTS_CUSTOM_H__
#define __BOOTS_CUSTOM_H__

#define MOD_CUSTOM \
    MOD_AMD, \
    MOD_DPF, \
    MOD_ITF, \
    MOD_PID, \
    MOD_VSR, \
    MOD_DMR, \
    MOD_ACR, \
    MOD_ADP, \
    MOD_NET, \
    MOD_DNET, \
    MOD_HIJACK,\
    MOD_DOMAIN_CNT

#define MOD_ARRAY_CUSTOM \
    {MOD_AMD, BAS, OFF, OFF, "AMD", "Admin Main Daemon"}, \
    {MOD_DPF, BAS, OFF, OFF, "DPF", "Data process flow"}, \
    {MOD_ITF, BAS, OFF, OFF, "ITF", "Interface module"}, \
    {MOD_PID, ON,  OFF, OFF, "PID", "Packet identify module"}, \
    {MOD_VSR, ON,  OFF, OFF, "VSR", "Visted recored module"}, \
    {MOD_DMR, ON,  OFF, OFF, "DMR", "Domain rule module"}, \
    {MOD_ACR, ON,  OFF, OFF, "ACR", "Account rule module"}, \
    {MOD_ADP, ON,  OFF, OFF, "ADP", "Advertising push"}, \
    {MOD_NET, ON,  OFF, OFF, "NET", "Net Segment module"}, \
    {MOD_DNET, ON,  OFF, OFF, "DNET", "Dnet Segment module"},\
    {MOD_HIJACK, ON,  OFF, OFF, "DNET", "Dnet Segment module"},\
	{MOD_DOMAIN_CNT, ON,  OFF, OFF, "Domain", "Domain count"}

#define DOPT_CUSTOM \
    DOPT_TAGMON, \
    DOPT_FAILPKT,\
    DOPT_LOG
	

#define DOPT_ARRAY_CUSTOM \
    {DOPT_TAGMON, OFF,  "tagmon", "Hytag monitor"}, \
    {DOPT_FAILPKT,OFF, "failpkt", "Process fail packet record"},\
    {DOPT_LOG, ON, "log", "log record"}

#define CNT_CUSTOM \
    /* VSR Module counters */ \
    VSR_PKTS, /* VSR module input packets*/ \
    VSR_UNURLPKTS, /* take no url packet number */ \
    VSR_URLPKTS, /* take url packet number */ \
    VSR_UNMATCHPKTS, /* not match packets */ \
    VSR_MATCHPKTS, /* match packets */ \
    VSR_RULE_MATCHPKTS, /* rule ip, url all match */ \
    \
    DMR_PKTS, /* DMR module input packets*/\
    DMR_RULE_MATCH,  /* Match in host rule */\
    DMR_RULE_UNMATCH,/* Unmatch found in host rule*/\
    \
    ACR_PKTS, /* ACR module input packets*/ \
    ACR_TABLE_FOUND, /* Found in account table */ \
    ACR_TABLE_NOTFOUND, /* Not found in account table*/ \
    ACR_RULE_MATCH, /* Match in account rule */ \
    ACR_RULE_UNMATCH, /* Unmatch found in account rule*/ \
    \
    VSR_RULE_NEW_URL, \
    VSR_RULE_LEN_OVERFLOW, /* url len overflow */ \
    VSR_RULE_URL_OVERFLOW,    /* url overflow */ \
	\
	URL_HOMEPAGE, \
	\
    ADP_IPKTS, \
    ADP_DROP_PARAM, \
    ADP_DROP_GET_OR_POST, \
    ADP_DROP_121ZOU, \
    ADP_DROP_NOT_HAO123, \
    ADP_DROP_ACT_PUSH, \
    ADP_DROP_ACT_DROP,\
    ADP_DROP_OUR_SUFFIX, \
    ADP_DROP_BACKSLASH_SUFFIX, \
    ADP_DROP_HTML_SUFFIX, \
    ADP_DROP_ADP_INTERVAL, \
    ADP_DROP_HEAD_GEN1, \
    ADP_DROP_SEND_PACKET1, \
    ADP_DROP_HEAD_GEN2, \
    ADP_DROP_SEND_PACKET2, \
    ADP_DROP_PUSH_PC, \
    ADP_DROP_PUSH_MOBILE, \
    ADP_PUSH_PC, \
    ADP_PUSH_MOBILE, \
    ADP_ALL_CAN_PUSH,\
    ADP_PUSH_TX_SUCCESS, \
    ADP_PUSH_ACK_SUCCESS, \
    ADP_HAO123, \
    \
    NET_PKTS, /* netseg module input packets*/ \
    NET_UNURLPKTS, /* take no url packet number */ \
    NET_URLPKTS, /* take url packet number */ \
    NET_UNMATCHPKTS, /* not match packets */ \
    NET_MATCHPKTS, /* match packets */ \
    NET_RULE_MATCHPKTS, /* rule ip, url all match */ \
    DNET_PKTS, /* dnetseg module input packets*/ \
    DNET_UNURLPKTS, /* take no url packet number */ \
    DNET_URLPKTS, /* take url packet number */ \
    DNET_UNMATCHPKTS, /* not match packets */ \
    DNET_MATCHPKTS, /* match packets */ \
    DNET_RULE_MATCHPKTS, /* rule ip, url all match */ \
    \
    HIJACK_IPKTS,\
    HIJACK_URL_MATCH_DROP,\
    HIJACK_KEY_MATCH_DROP,\
    HIJACK_LONG_URI_DROP,\
    HIJACK_ALL_CAN_PUSH,\
    HIJACK_DROP_ACT_DROP,\
    HIJACK_DROP_GET_OR_POST,\
    HIJACK_IP_SESSION_FAIL,\
    HIJACK_RULE_NOT_MATCH,\
    HIJACK_DROP_HEAD_GEN1,\
    HIJACK_DROP_SEND_PACKET1,\
    HIJACK_PUSH_TX_SUCCESS,\
    HIJACK_SIP_NOT_REPEAT,\
    HIJACK_SIP_REPEAT,\
    HIJACK_PKT_PERCENT_MATCH_FAIL,\
    HIJACK_IP_PERCENT_MATCH_FAIL,\
    HIJACK_IP_PKT_PERCENT_MATCH_FAIL,\
    HIJACK_TIME_PERCENT_MATCH_FAIL,\
    HIJACK_SERVER_HIT,\
    NAGA_CNT_MAX

#define CNT_DEF_CUSTOM \
    /* VSR Module */ \
    CNT_DEF(VSR_PKTS), \
    CNT_DEF(VSR_UNURLPKTS), \
    CNT_DEF(VSR_URLPKTS), \
    CNT_DEF(VSR_UNMATCHPKTS), \
    CNT_DEF(VSR_MATCHPKTS), \
    CNT_DEF(VSR_RULE_MATCHPKTS), \
    \
    /* DMR Module */ \
    CNT_DEF(DMR_PKTS), \
    CNT_DEF(DMR_RULE_MATCH), \
    CNT_DEF(DMR_RULE_UNMATCH),\
    \
    /* ACR Module */ \
    CNT_DEF(ACR_PKTS), \
    CNT_DEF(ACR_TABLE_FOUND), \
    CNT_DEF(ACR_TABLE_NOTFOUND), \
    CNT_DEF(ACR_RULE_MATCH), \
    CNT_DEF(ACR_RULE_UNMATCH),\
    CNT_DEF(VSR_RULE_NEW_URL), \
    CNT_DEF(VSR_RULE_LEN_OVERFLOW), \
    CNT_DEF(VSR_RULE_URL_OVERFLOW), \
    \
    CNT_DEF(URL_HOMEPAGE),\
    CNT_DEF(ADP_IPKTS), \
    CNT_DEF(ADP_DROP_PARAM), \
    CNT_DEF(ADP_DROP_GET_OR_POST), \
    CNT_DEF(ADP_DROP_121ZOU), \
    CNT_DEF(ADP_DROP_NOT_HAO123), \
    CNT_DEF(ADP_DROP_ACT_PUSH), \
    CNT_DEF(ADP_DROP_ACT_DROP),\
    CNT_DEF(ADP_DROP_OUR_SUFFIX), \
    CNT_DEF(ADP_DROP_BACKSLASH_SUFFIX), \
    CNT_DEF(ADP_DROP_HTML_SUFFIX), \
    CNT_DEF(ADP_DROP_ADP_INTERVAL), \
    CNT_DEF(ADP_DROP_HEAD_GEN1), \
    CNT_DEF(ADP_DROP_SEND_PACKET1), \
    CNT_DEF(ADP_DROP_HEAD_GEN2), \
    CNT_DEF(ADP_DROP_SEND_PACKET2), \
    CNT_DEF(ADP_DROP_PUSH_PC), \
    CNT_DEF(ADP_DROP_PUSH_MOBILE), \
    CNT_DEF(ADP_PUSH_PC), \
    CNT_DEF(ADP_PUSH_MOBILE), \
    CNT_DEF(ADP_ALL_CAN_PUSH),\
	CNT_DEF(ADP_PUSH_TX_SUCCESS), \
	CNT_DEF(ADP_PUSH_ACK_SUCCESS), \
    CNT_DEF(ADP_HAO123), \
    CNT_DEF(NET_PKTS), \
    CNT_DEF(NET_UNURLPKTS), \
    CNT_DEF(NET_URLPKTS), \
    CNT_DEF(NET_UNMATCHPKTS), \
    CNT_DEF(NET_MATCHPKTS), \
    CNT_DEF(NET_RULE_MATCHPKTS), \
    CNT_DEF(DNET_PKTS), \
    CNT_DEF(DNET_UNURLPKTS), \
    CNT_DEF(DNET_URLPKTS), \
    CNT_DEF(DNET_UNMATCHPKTS), \
    CNT_DEF(DNET_MATCHPKTS), \
    CNT_DEF(DNET_RULE_MATCHPKTS), \
    \
    CNT_DEF(HIJACK_IPKTS),\
    CNT_DEF(HIJACK_URL_MATCH_DROP),\
    CNT_DEF(HIJACK_KEY_MATCH_DROP),\
    CNT_DEF(HIJACK_LONG_URI_DROP),\
    CNT_DEF(HIJACK_ALL_CAN_PUSH),\
    CNT_DEF(HIJACK_DROP_ACT_DROP),\
    CNT_DEF(HIJACK_DROP_GET_OR_POST),\
    CNT_DEF(HIJACK_IP_SESSION_FAIL),\
    CNT_DEF(HIJACK_RULE_NOT_MATCH),\
    CNT_DEF(HIJACK_DROP_HEAD_GEN1),\
    CNT_DEF(HIJACK_DROP_SEND_PACKET1),\
    CNT_DEF(HIJACK_PUSH_TX_SUCCESS),\
    CNT_DEF(HIJACK_SIP_NOT_REPEAT),\
    CNT_DEF(HIJACK_SIP_REPEAT),\
    CNT_DEF(HIJACK_PKT_PERCENT_MATCH_FAIL),\
    CNT_DEF(HIJACK_IP_PERCENT_MATCH_FAIL),\
    CNT_DEF(HIJACK_IP_PKT_PERCENT_MATCH_FAIL),\
    CNT_DEF(HIJACK_TIME_PERCENT_MATCH_FAIL),\
    CNT_DEF(HIJACK_SERVER_HIT),\
    CNT_DEF(NAGA_CNT_MAX)

/* Custom spinlock */
#define bts_spinlock_t rte_spinlock_t
#define bts_spin_lock(_lock) rte_spinlock_lock(_lock)
#define bts_spin_unlock(_lock) rte_spinlock_unlock(_lock)
#define bts_spin_init(_lock) rte_spinlock_init(_lock)

/* Custom automic opreation */
#define bts_atomic32_inc  rte_atomic32_inc
#define bts_atomic64_inc  rte_atomic64_inc
#define bts_atomic32_add  rte_atomic32_add
#define bts_atomic64_add  rte_atomic64_add
#define bts_atomic32_dec  rte_atomic32_dec
#define bts_atomic64_dec  rte_atomic64_dec
#define bts_atomic32_set  rte_atomic32_set
#define bts_atomic64_set  rte_atomic64_set
#define bts_atomic32_get  rte_atomic32_read
#define bts_atomic64_get  rte_atomic64_read

#endif /* ! __BOOTS_CUSTOM_H__ */
