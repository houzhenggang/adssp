#ifndef __BOOTS_H__
#define __BOOTS_H__
#include "stdint.h"
#include "rte_atomic.h"
#include "rte_spinlock.h"

#if 0
typedef unsigned char  int8_t;
typedef unsigned short int16_t;
typedef unsigned int   int32_t;
typedef unsigned long long int64_t;
#endif

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
//typedef unsigned long long uint64_t;
typedef unsigned char mac_t[6];
typedef uint32_t ipv4_t;
typedef uint16_t ipv6_t[8];
typedef unsigned long long  ULL;
//typedef rte_spinlock_t    bts_spinlock_t;
typedef rte_atomic64_t    bts_atomic64_t;
typedef rte_atomic32_t    bts_atomic32_t;
#define CMD_NODE          VIEW_NODE

typedef enum {
    OFF,
    ON,
    BAS
} bswt;

#define FALSE   0
#define TRUE    1

typedef enum {
    E_SUCCESS,  // 成功
    E_FAIL,     // 失败
    E_NULL,     // 表项为空
    E_MATCH,    // 不匹配
    E_EFFECTIVE,// 表项无效
    E_MEMORY,   // 内存不足 
    E_PARAM,    // 参数错误
    E_EXCEED,   // 越界 
    E_OPEN,     // 打开失败
    E_FOUND,    // 找不到
    E_EXIST,    // 已经存在
    E_BUSY,     // 繁忙 
    E_COMPARE,
    E_INIT,
    E_FORMAT,
    E_MAX,      // 错误码的最大值
} berr;

typedef struct {
    uint8_t major;
    uint8_t minor;
    uint16_t revison;
} version_t;

typedef uint32_t bts_ipaddr_t;

char* berr_msg(berr e);

#endif
