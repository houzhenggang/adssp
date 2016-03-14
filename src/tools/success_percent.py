#!/usr/bin/python

import sys

ARGV1=sys.argv[1]


SNET_FILE_NAME=sys.argv[2]
DOMAIN_FILE_NAME=sys.argv[3]

SNET_PUSH_TAB={}
DOMAIN_PUSH_TAB={}

SNET_SUCCESS_TAB={}
DOMAIN_SUCCESS_TAB={}

def snet_push_hit (SNET):
    SNET_PUSH_TAB[SNET]=SNET_PUSH_TAB.get(SNET, 0) + 1

def domain_push_hit (DOMAIN):
    DOMAIN_PUSH_TAB[DOMAIN]=DOMAIN_PUSH_TAB.get(DOMAIN, 0) + 1

def snet_success_hit (SNET):
    SNET_SUCCESS_TAB[SNET]=SNET_SUCCESS_TAB.get(SNET, 0) + 1

def domain_success_hit (DOMAIN):
    DOMAIN_SUCCESS_TAB[DOMAIN]=DOMAIN_SUCCESS_TAB.get(DOMAIN, 0) + 1

LOG_FILE=open(ARGV1)

for LINE in LOG_FILE.readlines():
    LINE=LINE.strip('\n')
    FIELD  = LINE.split('|')
    if len(FIELD) < 8:
        continue
    SNET = FIELD[5]
    DOMAIN = FIELD[6]
    PUSH = FIELD[7]
    TT   = FIELD[0]
    F1 = TT.split(' ');
    SNET   = F1[3]
    SNET   = FIELD[5]
    DOMAIN = FIELD[6]
    PUSH   = FIELD[7][3]

    #if cmp(PUSH, 'P')==0:
    if PUSH.find("P") != -1:
        snet_push_hit(SNET)
        domain_push_hit(DOMAIN)

    #if cmp(PUSH, 'S')==0:
    if PUSH.find("S") != -1:
        snet_success_hit(SNET)
        domain_success_hit(DOMAIN)

LOG_FILE.close()

SNET_FILE=open(SNET_FILE_NAME,"w")
for SNET, SUCCESS_NUM in SNET_SUCCESS_TAB.items():
    PUSH_NUM = SNET_PUSH_TAB.get(SNET, 0)
    if PUSH_NUM != 0:
        PERCENT  = (SUCCESS_NUM * 100) / PUSH_NUM
        print >> SNET_FILE, "%s    %d    %d    %d%%" %(SNET, PUSH_NUM, SUCCESS_NUM, PERCENT)

SNET_FILE.close()

print "Save SNET stat to file ", SNET_FILE

DOMAIN_FILE=open(DOMAIN_FILE_NAME,"w")
for DOMAIN, SUCCESS_NUM in DOMAIN_SUCCESS_TAB.items():
    PUSH_NUM = DOMAIN_PUSH_TAB.get(DOMAIN, 0)
    if PUSH_NUM != 0:
        PERCENT  = (SUCCESS_NUM * 100) / PUSH_NUM
        print >> DOMAIN_FILE, "%s    %d    %d    %d%%" %(DOMAIN, PUSH_NUM, SUCCESS_NUM, PERCENT)

DOMAIN_FILE.close()

print "Save DOMAIN stat to file ", DOMAIN_FILE





