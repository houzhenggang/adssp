#include "boots.h"
#include "bts_debug.h"

char *berr_array[E_MAX + 1] = {
    "SUCCESS",
    "FAIL",
    "NULL",
    "MATCH",    // ???
    "EFFECTIVE",
    "MEMORY",   // ???? 
    "PARAM",    // ????
    "EXCEED",   // ?? 
    "OPEN",     // ????
    "FOUND",    // ???
    "EXIST",    // ????
    "BUSY",     // ?? 
    "COMPARE",
    "INIT",
    "FORMAT",
    "UNKOWN",
};

char* berr_msg(berr e)
{
    if (e >= E_MAX)
    {
        return berr_array[E_MAX];
    }

    return berr_array[e];
}

mod_t mod_array[MOD_MAX] = {
    {MOD_DBG, BAS, OFF, OFF, "DBG", "debug module"},
#ifdef MOD_ARRAY_CUSTOM
    MOD_ARRAY_CUSTOM,
#endif
} ;

dopt_t dopt_array[DOPT_MAX] = {
      {DOPT_INFO,       OFF,    "info",         "normal debug message"},
      {DOPT_VERB,       OFF,    "verb",         "verbose debug message"},
      {DOPT_WARN,       ON,     "warn",         "warning message"},
      {DOPT_ERR,        ON,     "err",          "error message"},
      {DOPT_ETRACE,     ON,     "etrace",       "error return trace"},
      {DOPT_TINYSTEP,   OFF,    "tinystep",     "time spand of a section code"},
      {DOPT_TRAP,       OFF,    "trap",         "a trap trigger by condition"},
      {DOPT_CALLSTACK,  OFF,    "callstack",    "dump call stack"},
#ifdef DOPT_ARRAY_CUSTOM
      DOPT_ARRAY_CUSTOM,
#endif
} ;

mod_t*
mod_lookup(char *name)
{
    int i;
    if (NULL == name)
    {
        return NULL;
    }

    for (i = 0; i < MOD_MAX; i++)
    {
        if (!strcasecmp(name, mod_array[i].abbr))
        {
            return &mod_array[i];
        }
    }

    return 0;
}

berr
mod_modify(char *name, bswt enable, bswt debug)
{
    mod_t *module = NULL;
    module = mod_lookup(name);

    if (NULL == module)
    {
        BRET(E_FOUND);
    }

    if (BAS != module->enable)
    {
        module->enable = enable;
    }
 
    module->debug = debug;

    return E_SUCCESS;
}

int mod_is_enable(bmod mod)
{
   return (mod < MOD_MAX) ?  mod_array[mod].enable : OFF;
}


int dopt_is_enable(dopte dop)
{
    return dop < DOPT_MAX ? dopt_array[dop].enable : OFF;
}

dopt_t*
dopt_lookup(char *name)
{
    int i;
    if (NULL == name)
    {
        return NULL;
    }

    for (i = 0; i < MOD_MAX; i++)
    {
        if (!strcasecmp(name, dopt_array[i].name))
        {
            return &dopt_array[i];
        }
    }

    return 0;
}


berr
dopt_modify(char *name, bswt enable)
{
    dopt_t *dopt = NULL;

    dopt = dopt_lookup(name);

    if (NULL == dopt)
    {
        BRET(E_FOUND);
    }

    if (BAS != dopt->enable)
    {
        dopt->enable = enable;
    }

    return E_SUCCESS;
}
/* End of file */
