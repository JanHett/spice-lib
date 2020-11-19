#include <gtest/gtest.h>

#include <spice-lib/algorithm.hpp>

TEST(algorithm, min) {
    auto min1 = spice::min(2, 3);
    EXPECT_EQ (2, min1);
    auto min2 = spice::min(4, 3);
    EXPECT_EQ (3, min2);
    auto min3 = spice::min(4, -3);
    EXPECT_EQ (-3, min3);
    auto min4 = spice::min(-4, 3);
    EXPECT_EQ (-4, min4);
    auto min5 = spice::min(-4, -3);
    EXPECT_EQ (-4, min5);
    auto min6 = spice::min(-3, 4);
    EXPECT_EQ (-3, min6);
    auto min7 = spice::min(3, -4);
    EXPECT_EQ (-4, min7);
    auto min8 = spice::min(-3, -4);
    EXPECT_EQ (-4, min8);
    auto min9 = spice::min(-4, -4);
    EXPECT_EQ (-4, min9);

    auto min1f = spice::min<float>(2, 3.2);
    EXPECT_FLOAT_EQ (2, min1f);
    auto min2f = spice::min<float>(4, 3.2);
    EXPECT_FLOAT_EQ (3.2, min2f);
    auto min3f = spice::min<float>(4, -3.2);
    EXPECT_FLOAT_EQ (-3.2, min3f);
    auto min4f = spice::min<float>(-4, 3.2);
    EXPECT_FLOAT_EQ (-4, min4f);
    auto min5f = spice::min<float>(-4, -3.2);
    EXPECT_FLOAT_EQ (-4, min5f);
    auto min6f = spice::min<float>(-3, 4.2);
    EXPECT_FLOAT_EQ (-3, min6f);
    auto min7f = spice::min<float>(3, -4.2);
    EXPECT_FLOAT_EQ (-4.2, min7f);
    auto min8f = spice::min<float>(-3, -4.2);
    EXPECT_FLOAT_EQ (-4.2, min8f);
    auto min9f = spice::min<float>(-4, -4);
    EXPECT_FLOAT_EQ (-4, min9f);
}

TEST(algorithm, max) {
    auto max1 = spice::max(2, 3);
    EXPECT_EQ (3, max1);
    auto max2 = spice::max(4, 3);
    EXPECT_EQ (4, max2);
    auto max3 = spice::max(4, -3);
    EXPECT_EQ (4, max3);
    auto max4 = spice::max(-4, 3);
    EXPECT_EQ (3, max4);
    auto max5 = spice::max(-4, -3);
    EXPECT_EQ (-3, max5);
    auto max6 = spice::max(-3, 4);
    EXPECT_EQ (4, max6);
    auto max7 = spice::max(3, -4);
    EXPECT_EQ (3, max7);
    auto max8 = spice::max(-3, -4);
    EXPECT_EQ (-3, max8);
    auto max9 = spice::max(-4, -4);
    EXPECT_EQ (-4, max9);

    auto max1f = spice::max<float>(2, 3.2);
    EXPECT_FLOAT_EQ (3.2, max1f);
    auto max2f = spice::max<float>(4, 3.2);
    EXPECT_FLOAT_EQ (4, max2f);
    auto max3f = spice::max<float>(4, -3.2);
    EXPECT_FLOAT_EQ (4, max3f);
    auto max4f = spice::max<float>(-4, 3.2);
    EXPECT_FLOAT_EQ (3.2, max4f);
    auto max5f = spice::max<float>(-4, -3.2);
    EXPECT_FLOAT_EQ (-3.2, max5f);
    auto max6f = spice::max<float>(-3, 4.2);
    EXPECT_FLOAT_EQ (4.2, max6f);
    auto max7f = spice::max<float>(3, -4.2);
    EXPECT_FLOAT_EQ (3, max7f);
    auto max8f = spice::max<float>(-3, -4.2);
    EXPECT_FLOAT_EQ (-3, max8f);
    auto max9f = spice::max<float>(-4, -4);
    EXPECT_FLOAT_EQ (-4, max9f);
}

TEST(algorithm, clamp) {
    auto clamp1 = spice::clamp(1, 2, 4);
    EXPECT_EQ(2, clamp1);
    auto clamp2 = spice::clamp(2, 2, 4);
    EXPECT_EQ(2, clamp2);
    auto clamp3 = spice::clamp(3, 2, 4);
    EXPECT_EQ(3, clamp3);
    auto clamp4 = spice::clamp(4, 2, 4);
    EXPECT_EQ(4, clamp4);
    auto clamp5 = spice::clamp(5, 2, 4);
    EXPECT_EQ(4, clamp5);
    auto clamp6 = spice::clamp(-5, 0, 4);
    EXPECT_EQ(0, clamp6);
}
