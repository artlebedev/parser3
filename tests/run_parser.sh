#!/bin/sh

SERVER_SOFTWARE=make_test \
PATH_TRANSLATED=$2 \
PATH_INFO=/tests/$2 \
SCRIPT_NAME=/tests/$2 \
$1 2>>parser3.stderr
