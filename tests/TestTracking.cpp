#include <gtest/gtest.h>
#include "Tracking.h"

class TestTracking : public ::testing::Test {


    void SetUp() override {

    }

    void TearDown() override {

    }

};

TEST_F(TestTracking, TrackerType) {
    cv::VideoCapture camera;
    Tracking mosse("MOSSE", camera);
    EXPECT_EQ(mosse.getTrackerType(), "MOSSE");
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}