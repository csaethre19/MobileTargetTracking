#include <gtest/gtest.h>
#include "UART.h"

class TestUART : public ::testing::Test {

    void SetUp() override {

    }

    void TearDown() override {

    }
};

TEST_F(TestUART, OpenValidPort) {
    int uart_fd = openUART("/dev/ttyS0");
    EXPECT_EQ(uart_fd, 3);
}

TEST_F(TestUART, OpenInvalidPort) {
    int uart_fd = openUART("/dev/some_port");
    EXPECT_EQ(uart_fd, -1);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}