#ifndef __BTS_DEBUG_H__
#define __BTS_DEBUG_H__

#include "boots.h"
#include <stdio.h>
#include "boots_custom.h"

typedef enum {
      DOPT_INFO,
      DOPT_VERB,
      DOPT_WARN,
      DOPT_ERR,
      DOPT_ETRACE,
      DOPT_TINYSTEP,
      DOPT_TRAP,
      DOPT_CALLSTACK,
#ifdef DOPT_CUSTOM
      DOPT_CUSTOM,
#endif
      DOPT_MAX
} dopte;

typedef struct {
      dopte idx;
      bswt enable;
      char *name;
      char *desc;
} dopt_t;

extern dopt_t dopt_array[DOPT_MAX];

int dopt_is_enable(dopte dop);
dopt_t* dopt_lookup(char *name);
berr dopt_modify(char *name, bswt enable);

typedef enum {
    MOD_DBG,
    MOD_CUSTOM,
    MOD_MAX
} bmod;

typedef struct {
    bmod  mod;      /* Module ID*/
    bswt  enable;      /* Turn on/off this module */
    bswt  debug;      /* Turn on/off the debug dump of this module */
    int   resv;     
    char *abbr;
    char *desc;
} mod_t;

mod_t mod_array[MOD_MAX];
mod_t* mod_lookup(char *name);
berr mod_modify(char *name, bswt enable, bswt debug);

#define BTS_DEBUG_DUMP(_mod, _opt, _fmt, _args...) { \
    if ((_mod < MOD_MAX) || (_opt < DOPT_MAX)) { \
        if (dopt_array[_opt].enable && dopt_array[_mod].enable) { \
            printf("[%s.%s] %s.%d:" _fmt, #_mod, #_opt, __func__, __LINE__, ##_args); \
        } \
    } \
}

#define DBG_INFO(_mod, _fmt, _args...)   \
    BTS_DEBUG_DUMP(_mod, DOPT_INFO, _fmt, ##_args)

#define DBG_VERB(_mod, _fmt, _args...)  \
    BTS_DEBUG_DUMP(_mod, DOPT_VERB, _fmt, ##_args)

#define DBG_WARN(_mod, _fmt, _args...) \
    BTS_DEBUG_DUMP(_mod, DOPT_WARN, _fmt, ##_args)

#define DBG_ERR(_mod, _fmt, _args...) \
    BTS_DEBUG_DUMP(_mod, DOPT_ERR, _fmt, ##_args)

#define BRET(e) \
{ \
    berr _rv = (e);\
    if ((_rv != E_SUCCESS) && dopt_array[DOPT_ETRACE].enable) \
    { \
        printf("[ETRACE] %s.%d: return %s!\n", __FUNCTION__, __LINE__, berr_msg(_rv)); \
    } \
    return _rv; \
}

#endif
