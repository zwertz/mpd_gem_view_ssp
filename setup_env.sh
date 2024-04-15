#!/bin/bash -i

source /home/sbs-onl/data_viewer/root/bin/thisroot.sh

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${PWD}/decoder/lib:${PWD}/gem/lib:${PWD}/epics/lib:${PWD}/tracking_dev/lib
