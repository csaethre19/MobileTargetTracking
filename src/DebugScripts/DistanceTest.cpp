#include <iostream>
#include <cmath>

using namespace std;

void calculate_distance(int xc, int yc, double &pixDistance, double &distance, double &angleInDegrees) {
    // Calculate the center of the video frame
    int videoCenterX = 1280 / 2;
    int videoCenterY = 720 / 2;

    // Distance from the center of the video to the center of the bounding box
    int deltaX = xc - videoCenterX;
    int deltaY = videoCenterY - yc; // Invert the y-axis

    // Use Pythagorean's theorem
    pixDistance = std::sqrt(deltaX * deltaX + deltaY * deltaY);

    distance = pixDistance/720 * 250;
    distance /= 12;

    // Calculate the angle in radians
    double angleInRadians = std::atan2(deltaY, deltaX);

    // Convert to degrees
    angleInDegrees = angleInRadians * (180.0 / M_PI);

    // Adjust the angle to make 0 degrees the top of the frame
    // and angles increase clockwise
    angleInDegrees = 90.0 - angleInDegrees;

    // Normalize the angle to be within [0, 360) degrees
    if (angleInDegrees < 0) {
        angleInDegrees += 360.0;
    } else if (angleInDegrees >= 360.0) {
        angleInDegrees -= 360.0;
    }

    // Print the results
    cout << "Distance: " << pixDistance << " pixels" << endl;
    cout << "Distance: " << distance << " feet" << endl;
    cout << "Angle: " << angleInDegrees << " degrees" << endl;
}

int main() {
    // Example bounding box center coordinates
    int xc = 640;
    int yc = 720;

    double pixDistance = 0.0;
    double angleInDegrees = 0.0;
    double distance = 0.0;

    calculate_distance(xc, yc, pixDistance, distance, angleInDegrees);

    return 0;
}

