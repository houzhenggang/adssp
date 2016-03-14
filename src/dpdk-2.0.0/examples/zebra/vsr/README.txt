/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/mnt/hgfs/code/rose/src/dpdk-2.0.0/examples/zebra/vsr\README.txt
#       @date         :2015-08-23 15:25
#       @algorithm    :
=============================================================================*/
software layer:
management:                  file_output:                     business:
vsr_cmd.c -----------------  vsr_file.c   ------------------- vsr_dp.c
    |                           |                                |
    |                           |                                |
    ↓                           ↓                                |
    vsr_cmd_api.c ------------- vsr_cmd_api.c                    |
        |                                                        |
        |                                                        |
        ↓                                                        ↓
        vsr_api.c ---------------------------------------------- vsr_api.c
            |                                                       |
            |                                                       |
            ↓                                                       ↓
            vsr.c ------------------------------------------------- vsr.c
