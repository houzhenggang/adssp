#include <string.h>
#include "boots.h"
#include "bts_debug.h"

#include "naga_types.h"
#include "naga_util.h"


berr 
naga_action_parse(char *str, uint32_t *actions)
{
    char *tokp = NULL; 
	char *act_str = NULL;

    if ((NULL == str) || (NULL == actions))
    {
        BRET(E_PARAM);
    }

    *actions = 0;

	//act_str = strdup(str);
	act_str = str;

    tokp = strtok(act_str, ",");

    while(tokp != NULL) {
        
        if (!strcmp("log", tokp))
        {
            *actions |= ACT_LOG;
        }
        else if (!strcmp("trace", tokp))
        {
            *actions |= ACT_TRACE;
        }
        else if (!strcmp("drop", tokp))
        {
            *actions |= ACT_DROP;
        }
        else if (!strcmp("push", tokp))
        {
            *actions |= ACT_PUSH;
        }
        else if (!strcmp("redir", tokp))
        {
            *actions |= ACT_REDIR;
        }
        else if (!strcmp("tagdump", tokp))
        {
            *actions |= ACT_TAGDUMP;
        }
        else
        {
            BRET(E_FOUND);
        }

        tokp = strtok(NULL, ","); 
    }

	return E_SUCCESS;
}

void 
naga_action_string(uint32_t *action, char *str)
{
    char *cp = NULL;

    if ((NULL == str) || (NULL == action))
    {
        return ;
    }

    cp = str;

    if (ACT_IS_VAILD(*action, ACT_LOG))
    {
    	if (cp != str)
        {
            cp += sprintf(cp, ",");
        }
        cp += sprintf(cp, "log");
    }

    if (ACT_IS_VAILD(*action, ACT_TRACE))
    {
        if (cp != str)
        {
            cp += sprintf(cp, ",");
        }

        cp += sprintf(cp, "trace");
    }

    if (ACT_IS_VAILD(*action, ACT_DROP))
    {
        if (cp != str)
        {
            cp += sprintf(cp, ",");
        }

        cp += sprintf(cp, "drop");
    }

    if (ACT_IS_VAILD(*action, ACT_PUSH))
    {
        if (cp != str)
        {
            cp += sprintf(cp, ",");
        }

        cp += sprintf(cp, "push");
    }

    if (ACT_IS_VAILD(*action, ACT_TAGDUMP))
    {
        if (cp != str)
        {
            cp += sprintf(cp, ",");
        }

        cp += sprintf(cp, "tagdump");
    }
}

/* End of file */
