#!/bin/sh

set -e

DATE=`date '+%Y-%m-%d'`
SCRIPTFILE="typescript.breakthrough_${DATE}"

script -t 0 -a $SCRIPTFILE ./build_forward_backward breakthrough_7x5 71 0
