# Mobile Tracking System

## Overview

<p>University of Utah Computer Engineering Senior Capstone by Kirra Kotsenburg, Nicholas Ronnie, and Charlotte Saethre. </p>
<hr>
<div style="display: flex; justify-content: space-between;">
<img src="images/opencv.png" width="150" height="200" />
<img src="images/drone.png" width="150" height="200" />
<img src="images/raspi.png" width="250" height="200" />
</div>
<hr>

<p>This project explores the use of a Raspberry Pi 3B+, Camera module, Qt Desktop Application, and OpenCV tracking algorithms to acheive a mobile tracking system that interfaces with a drone.</p>

For video transmission we are using [AKK X2M 5.8Ghz Switchable FPV Transmitter](https://www.amazon.com/gp/product/B0773JVM8M/ref=ox_sc_act_image_1?smid=ADP3MHCS3NLR7&psc=1) and the [SoloGood FPV Receiver](https://www.amazon.com/gp/product/B08YJGCVJS/ref=ox_sc_act_image_2?smid=A2XZ0PQGR3TYBH&psc=1)

This project works along side our TrackerApp project for the user desktop application which you can access [here](https://github.com/KirraKotsenburg/TrackerApp).

For the entire system set up we will be using a custom Gimbal to hold the camera connected to the Raspberry pi. Project can be accessed [here](https://github.com/csaethre19/GimbalProject)

### System Overview
<img src="images/System.png" width="200" height="250" /> 

This system interfaces with two additional hardware modules called swarm-dongles designed and developed by [REDMORE DYNAMICS](https://redmoredynamics.com/). These modules facilitate over the air communication and enable communication to the flight controller of the drone. You can learn more about these modules [here](https://s3.redmoredynamics.com/website/User-Documentation-v1.0.0.pdf). The system is made up of the following components:

<ul>
<li>Raspberry Pi 3B+ with Camera Module</li>
<li>Video Transmitter</li>
<li>Video Receiver</li>
<li>Swarm-dongle module 1 (connected to Raspi)</li>
<li>Swarm-dongle module 2 (connected to Desktop)</li>
</ul>

### Internal System Overview
<img src="images/InternalSystem.png" width="250" height="200" />


### Command Protocol

<p>The following message format is used to communicate between the Raspi, Swarm-dongles, and Desktop app:</p>
<img src="images/MessageFormat.png" width="500" height="100" />

## Usage

To run in the current state:
```shell
sudo systemctl stop serial-getty@ttyS0.service
sudo systemctl disable serial-getty@ttyS0.service
sudo chmod 666 /dev/ttyS0
mkdir build
cd build
make TestMain
sudo ./TestMain
```

This will run our tracking application that opens a UART port and listens for commands. It will start transmitting camera frames right away. Upon receiving a `track-start` command it will enter a new thread that will start up the tracking process and switch to sending tracking frames. 

Note* tracker.sh is a bash script that will automate these commands and allow us to run this on power up for the final product. Run in root project directory:
```shell
./tracker.sh
```
This will execute the stop/disable getty commands for the ttyS0, cd into build folder and run the application. 

## Dev Set Up
1. Create SSH key on Raspberry Pi (Linux based system).
    In a terminal type the following command:
    ```shell
    ssh-keygen
    ```
    a. Press enter for default file location (default directory is at /home/pi/.ssh/id_rsa).

    b. Press enter for passphrase
2. Copy the generated SSH key:
    ```shell
    cat ~/.ssh/id_rsa.pub
    ```
3. With the key copied go to your GitHub account > Settings > SSH and GPG keys > Click New SSH key. Provide a name and copy the key and click Add SSH key. 

4. Once you have your GitHub account set up with your key you can clone the project
    ```shell
    git clone git@github.com:csaethre19/MobileTargetTracking.git
    ```

### Camera Module

Commands to get camera settings on Raspberry pi:
```shell
sudo apt-get install v4l-utils
v4l2-ctl --list-formats-ext
v4l2-ctl --set-fmt-video=width=1920,height=1080,pixelformat=BGR3
v4l2-ctl --set-parm=30
```

### Transmitter
```shell
sudo nano /boot/config.txt
```
Underneath #hdmi_safe=1 add the following: <br>
disable_overscan = 0 <br>
enable_tvout=1 <br>

### OpenCV
It is necessary to build OpenCV from source on the Raspberry Pi itself.
Follow the instructions [here](https://qengineering.eu/install-opencv-on-raspberry-pi.html) to do this if you do not have OpenCV.

### MAVLink (This library is not being used at this time)
```shell
cd third_party
git clone https://github.com/mavlink/c_library_v2.git
```

### UART 
#### Enable UART on Raspberry Pi for serial device ttyS0:
```shell
cd /dev
/dev ls -l
```
This lists devices - we will see ttyAMA0 but we need ttyS0 for our UART interface, so we need to enable this.

```shell
sudo nano /boot/config.txt
```
Scroll to end of file and add: enable_uart=1
save and exit (To save in nano hit CTRL+O then enter and to exit hit CTRL+X)

#### PuTTY Console and UART Device Setup:
Open PuTTY session using COM3 as serial line (or whatever device name UART device comes up as) and leave speed to default (9600) for testing swarm-dongle UART communication. (Testing for gimbal communication has not yet been added and may require a different speed set – 115200).

<img src="images/putty.png" width="300" height="200" />

For serial port ttyS0 wiring:
- Using UART device connect ground to ground pin on Raspberry Pi (pin 6).
- Connect RX pin on UART device to UART0_TXD on Raspberry Pi (pin 8).
- Connect TX pin on UART device to UART0_RXD on Raspberry Pi (pin 10).

<img src="images/pinout.png" width="350" height="400" />

Note* The [TrackerApp desktop application](https://github.com/KirraKotsenburg/TrackerApp) replaces the use of this PuTTY setup but is here in case of testing purposes. 

## Testing

To use gtest testing framework set up with the following commands:

```shell
sudo apt-get update
sudo apt-get install libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /lib
```
To add tests in CMakeLists.txt:

add_executable(\<TestName> tests/<TestFile.cpp>) <br>
target_link_libraries(\<TestName> PRIVATE GTest::gtest GTest::gtest_main) <br>
add_test(NAME my_test COMMAND \<TestName>) <br>

To run: <br>
./\<TestName>

## Logging
This is required to compile the project as we rely on injecting a logger into our different modules (i.e. Camera.cpp, Tracking.cpp, etc.) to better track bugs and issues.  <br><br>
To set up spdlog library:
```shell
mkdir third_party
cd third_party
git clone https://github.com/gabime/spdlog.git
cd spdlog
mkdir build
cd build
cmake ..
make
```
All log files will be found inside a `logs` folder within the build folder.

## Troubleshooting
If you are having linking issues when compiling do the following:
```shell
make clean
cmake .
make 
```
If you see a Permission Denied error for UART in the logs:
```shell
sudo chmod 666 /dev/ttyS0
```
