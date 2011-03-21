#!/bin/bash
IP=$1
OID_FILENAME=$2

function get_snmp {
    for i in $(cat $OID_FILENAME)
    do
        echo -n $(snmpget -Ov -Oq -v 2c -c public $IP $i)
        echo -n " "
    done
    echo ""
}

OPTS="--stream --lines --xlen 60 --ymin  0"

while true; do get_snmp; sleep 1; done |  ./feedGnuplot.pl "$OPTS" $3

