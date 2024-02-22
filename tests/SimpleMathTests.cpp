#include <gtest/gtest.h>
#include "SimpleMath.cpp"

class SimpleMathTest : public ::testing::Test {

    public:
    SimpleMathTest() {

    }

    ~SimpleMathTest() {

    }

    void SetUp() override {

    }

    void TearDown() override {

    }
};

TEST_F(SimpleMathTest, SimpleAdd) {
    SimpleMath uut;
    int result = uut.add(1,2);
    EXPECT_EQ(result, 3);
}

TEST_F(SimpleMathTest, SimpleSub) {
    SimpleMath uut;
    int result = uut.subtract(1,2);
    EXPECT_EQ(result, -1);
}

TEST_F(SimpleMathTest, SimpleMult) {
    SimpleMath uut;
    int result = uut.multiply(1,2);
    EXPECT_EQ(result, 2);
}

TEST_F(SimpleMathTest, SimpleDivide) {
    SimpleMath uut;
    int result = uut.divide(4,2);
    EXPECT_EQ(result, 2);
}

TEST_F(SimpleMathTest, DivideByZero) {
    SimpleMath uut;
    int result = uut.divide(1,0);
    EXPECT_EQ(result, -1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}