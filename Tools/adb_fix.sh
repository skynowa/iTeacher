#!/usr/bin/env bash
#
# \file  adb_fix.sh
# \brief fix "Incompatible device message when adding the Fairphone (Qt 5.3.0)"
#


echo "******************** Fix adb  ********************"
sudo adb kill-server
sudo adb start-server
sudo adb devices
