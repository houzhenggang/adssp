#!/usr/bin/python

import sys

ARGV1=sys.argv[1]

DOMAIN_FILE_NAME=sys.argv[2]

DOMAIN_TAB={}


def domain_hit(DOMAIN):
    DOMAIN_TAB[DOMAIN]=DOMAIN_TAB.get(DOMAIN, 0) + 1



LOG_FILE=open(ARGV1)

for LINE in LOG_FILE.readlines():
    LINE=LINE.strip('\n')
    FIELD  = LINE.split('|')
    if len(FIELD) < 7:
        continue
    DOMAIN = FIELD[6]
    domain_hit(DOMAIN)

LOG_FILE.close()

DOMAIN_ITEMS = sorted(DOMAIN_TAB.items(), key=lambda T:T[1], reverse = True)

DOMAIN_FILE=open(DOMAIN_FILE_NAME,"w")
for DOMAIN,NUM in DOMAIN_ITEMS:
    print >> DOMAIN_FILE, "%s    %d" %(DOMAIN, NUM)

DOMAIN_FILE.close()

print "Save DOMAIN stat to file ", DOMAIN_FILE
