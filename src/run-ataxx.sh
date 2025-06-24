#!/bin/sh

set -e

DATE=`date '+%Y-%m-%d'`
SCRIPTFILE="typescript.ataxx_${DATE}"

# script -t 0 -a $SCRIPTFILE ./build_forward ataxx_6x6 2

script -t 0 -a $SCRIPTFILE ./solve_backward ataxx_5x5 10

