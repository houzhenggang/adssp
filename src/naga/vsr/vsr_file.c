/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/mnt/hgfs/code/rose/src/dpdk-2.0.0/examples/zebra/vsr\vsr_file.c
#       @date         :2015/08/21 22:26
#       @algorithm    :
=============================================================================*/

#include "vsr_cmd_api.h"
#include "stdio.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

void vsr_write_file(char *name, char *buff, uint32_t size)
{
    int fdsrc;
    int flags = O_CREAT | O_TRUNC | O_WRONLY;

    /* open the source file, /dev/null, and /tmp/foo.bar */
    if ((fdsrc = open(name, flags, 0644)) < 0) {
        perror("open file fail");
        return;
    }

    if (write(fdsrc, buff, size) < 0) {
        perror("write file fail");
        return;
    }

    /* close files and exit */
    close(fdsrc);
    return;
}

void vsr_tab_file(void)
{
    int i;
    int rv;
    uint8_t buff[512*1024];
    char ip_str[20];
    uint32_t ip = 0;
    for ( i = 0; i < VSR_RULE_NUM_MAX; i++)
    {
        memset(buff, 0, sizeof(buff));
        rv = rule_vsr_cmd_dump(i, buff, sizeof(buff));
        if(rv)
        {
            /* not effective */
            continue;
        }
        rv = rule_vsr_cmd_get_ip(i, &ip);
        if(rv)
        {
            /* not effective */
            continue;
        }
        /*write to file */
        sprintf(ip_str, "%d.%d.%d.%d.log", (ip>>24) & 0xff,
                (ip >>16) & 0xff,
                (ip >> 8) & 0xff,
                (ip >> 0) & 0xff);
        vsr_write_file(ip_str, (char *)buff, sizeof(buff));
    }

    return ;

}

