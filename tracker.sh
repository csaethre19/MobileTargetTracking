#!/bin/bash

# Stop and disable serial-getty service on ttyS0
# To run: ./disable_serial_getty.sh
sudo systemctl stop serial-getty@ttyS0.service
sudo systemctl disable serial-getty@ttyS0.service
cd Desktop/MobileTargetTracking/build
sudo ./TestMain ../src/walking.mp4
