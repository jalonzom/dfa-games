#!/bin/sh

ls -rt typescript.* | tail -1 | xargs -n 1 tail -F | grep -E '(^ *node|remaining)'
