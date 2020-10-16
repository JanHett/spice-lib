#include <gtest/gtest.h>

#include <spice-lib/color.hpp>

using namespace spice;

TEST(color, default_constructor) {
    const color<float, 5> c;

    EXPECT_EQ(0.f, c[0]);
    EXPECT_EQ(0.f, c[1]);
    EXPECT_EQ(0.f, c[2]);
    EXPECT_EQ(0.f, c[3]);
    EXPECT_EQ(0.f, c[4]);
}

TEST(color, copy_constructor) {
    const color<float> c1{1.f, 1.1f, 0.42f, .47f};
    const color<float> c2(c1);

    EXPECT_EQ(c1, c2);

    EXPECT_EQ(1.f, c2[0]);
    EXPECT_EQ(1.1f, c2[1]);
    EXPECT_EQ(0.42f, c2[2]);
    EXPECT_EQ(0.47f, c2[3]);
}

TEST(color, single_value_constructor) {
    const color<uint8_t, 3> c(static_cast<uint8_t>(42));

    EXPECT_EQ(42, c[0]);
    EXPECT_EQ(42, c[1]);
    EXPECT_EQ(42, c[2]);
}

TEST(color, multiple_values_constructor) {
    const color<float> c{1, 1.1, 0.42, .47};

    EXPECT_EQ(1.f, c[0]);
    EXPECT_EQ(1.1f, c[1]);
    EXPECT_EQ(0.42f, c[2]);
    EXPECT_EQ(0.47f, c[3]);
}

TEST(color, multiple_values_pointer_constructor) {
    float values[] = {1, 1.1, 0.42, .47};
    const color<float> c(values);

    EXPECT_EQ(1.f, c[0]);
    EXPECT_EQ(1.1f, c[1]);
    EXPECT_EQ(0.42f, c[2]);
    EXPECT_EQ(0.47f, c[3]);
}