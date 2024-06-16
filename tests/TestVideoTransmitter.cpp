#include <gtest/gtest.h>
#include "VideoTransmitter.h"

class TestVideoTransmitter : public ::testing::Test {

    protected:
    VideoTransmitter uut;

    void SetUp() override {

    }

    void TearDown() override {

    }
};

TEST_F(TestVideoTransmitter, Fbfb) {
    int fbfd = uut.getFbfb();
    EXPECT_NE(fbfd, 0);
}

TEST_F(TestVideoTransmitter, Fbp) {
    char *fbp = uut.getFbp();
    EXPECT_NE((int)fbp, 0);
}

TEST_F(TestVideoTransmitter, Screensize) {
    long int screensize = uut.getScreensize();
    EXPECT_NE(screensize, 0);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}