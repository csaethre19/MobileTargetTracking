#include <gtest/gtest.h>
#include "Camera.h"

class TestCamera : public ::testing::Test {

    protected:
    Camera uut;

    void SetUp() override {

    }

    void TearDown() override {

    }
};

TEST_F(TestCamera, AccessCamera) {
    cv::VideoCapture camera = uut.selectVideo("");
    ASSERT_TRUE(camera.isOpened());
    EXPECT_EQ(camera.get(CAP_PROP_FRAME_WIDTH), 1280);
    EXPECT_EQ(camera.get(CAP_PROP_FRAME_HEIGHT), 720);
    EXPECT_EQ(camera.get(CAP_PROP_FPS), 30);
}

TEST_F(TestCamera, AccessVideo) {
    cv::VideoCapture video = uut.selectVideo("../src/DebugScripts/walking.mp4");
    ASSERT_TRUE(video.isOpened());
    EXPECT_EQ(video.get(CAP_PROP_FRAME_WIDTH), 1280);
    EXPECT_EQ(video.get(CAP_PROP_FRAME_HEIGHT), 720);
    EXPECT_EQ(video.get(CAP_PROP_FPS), 25);
}

TEST_F(TestCamera, InvalidPath) {
    string videoPath = "path/to/invalid/video.mp4";
    // Expect std::exit to be called
    EXPECT_EXIT(uut.selectVideo(videoPath), ::testing::ExitedWithCode(1), "");
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}