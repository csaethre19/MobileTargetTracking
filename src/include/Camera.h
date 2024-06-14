#ifndef CAMERA_H
#define CAMERA_H

#include <raspicam/raspicam_cv.h>

class Camera {
public:
    raspicam::RaspiCam_Cv selectVideo();
};

#endif
