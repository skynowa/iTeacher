#!/usr/bin/env bash
#
# \file  rebuild_unix.sh
# \brief full rebuild
#


make clean && qmake && make
