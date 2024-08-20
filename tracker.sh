#!/bin/bash

# Stop and disable serial-getty service on ttyS0
sudo systemctl stop serial-getty@ttyS0.service
sudo systemctl disable serial-getty@ttyS0.service
sudo chmod 666 /dev/ttyS0 # for some reason I get permission issues now with accessing ttyS0
cd build
make TestMain
sudo ./TestMain