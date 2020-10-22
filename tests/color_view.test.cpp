#include <gtest/gtest.h>

#include <spice-lib/color_view.hpp>
#include <spice-lib/color.hpp>

using namespace spice;

TEST(color_view, default_constructor) {
    const color_view<float> cv;

    EXPECT_EQ(nullptr, cv.data());
    EXPECT_EQ(0, cv.stride());
    EXPECT_EQ(0, cv.channels());
    EXPECT_EQ(0, cv.size());
}

TEST(color_view, copy_constructor) {
    /* const */ color<float> c1{1.f, 1.1f, 0.42f, .47f};
    const color_view<float> cv1(c1.data(), 1, c1.channels());
    const color<float> cv2(cv1);

    EXPECT_EQ(cv1, cv2);

    EXPECT_EQ(1.f, cv2[0]);
    EXPECT_EQ(1.1f, cv2[1]);
    EXPECT_EQ(0.42f, cv2[2]);
    EXPECT_EQ(0.47f, cv2[3]);
}