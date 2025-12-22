#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;

struct IService {
    virtual ~IService() = default;
    virtual int GetValue() = 0;
};

struct MockService : IService {
    MOCK_METHOD(int, GetValue, (), (override));
};

TEST(SmokeTest, BasicAssertions) {
    EXPECT_EQ(1 + 1, 2);
    EXPECT_TRUE(true);
}

TEST(SmokeTest, GMockWorks) {
    MockService mock;
    EXPECT_CALL(mock, GetValue()).WillOnce(Return(123));

    EXPECT_EQ(mock.GetValue(), 123);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}