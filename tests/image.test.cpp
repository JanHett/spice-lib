#include <gtest/gtest.h>

#include <type_traits>
#include <limits>
#include <numeric>
#include <cstdint>
#include <fstream>

#include <gsl/span>

#include <spice/image.hpp>
#include <spice/statistics.hpp>
#include <spice/print.hpp>

#include "test_utils.hpp"

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

TEST(image, default_constructor) {
    image<float> im;

    EXPECT_EQ(0, im.data().size());
    EXPECT_EQ(0, im.width());
    EXPECT_EQ(0, im.height());
    EXPECT_EQ(4, im.channels());
}

TEST(image, size_constructor) {
    image<float> im1(2, 3);

    EXPECT_EQ(6, im1.data().size());
    for (auto d : im1.data()) {
        EXPECT_EQ(0.f, d);
    }
    EXPECT_EQ(2, im1.width());
    EXPECT_EQ(3, im1.height());
    EXPECT_EQ(4, im1.channels());
}

TEST(image, copy_constructor) {
    image<float> im1(2, 3);
    image<float> im2(im1);

    EXPECT_TRUE(im1 == im2);

    EXPECT_EQ(6, im2.data().size());
    for (auto d : im2.data()) {
        EXPECT_EQ(0.f, d);
    }
    EXPECT_EQ(2, im2.width());
    EXPECT_EQ(3, im2.height());
    EXPECT_EQ(4, im2.channels());
}

TEST(image, operator_equals) {
    image<float> im1(2, 3);
    image<float> im2(im1);

    EXPECT_TRUE(im1 == im2);
    EXPECT_FALSE(im1 != im2);

    EXPECT_EQ(6, im2.data().size());
    for (auto d : im2.data()) {
        EXPECT_EQ(0.f, d);
    }
    EXPECT_EQ(2, im2.width());
    EXPECT_EQ(3, im2.height());
    EXPECT_EQ(4, im2.channels());

    im2(0, 0)[0] = 0.42;

    EXPECT_FALSE(im1 == im2);
    EXPECT_TRUE(im1 != im2);

    image<float> im3(4, 1);
    image<float> im4(1, 4);

    EXPECT_FALSE(im3 == im4);
    EXPECT_TRUE(im3 != im4);

    image<float> im5(2, 2);

    EXPECT_TRUE(im3 != im5);
}

TEST(image, operator_call_two_arg) {
    auto im = make_checkerboard<float, 3>(2, 2);

    EXPECT_EQ(im(0, 0)[0], 1);
    EXPECT_EQ(im(0, 0)[1], 1);
    EXPECT_EQ(im(0, 0)[2], 1);
    EXPECT_EQ(im(1, 0)[0], 0);
    EXPECT_EQ(im(1, 0)[1], 0);
    EXPECT_EQ(im(1, 0)[2], 0);
    EXPECT_EQ(im(0, 1)[0], 0);
    EXPECT_EQ(im(0, 1)[1], 0);
    EXPECT_EQ(im(0, 1)[2], 0);
    EXPECT_EQ(im(1, 1)[0], 1);
    EXPECT_EQ(im(1, 1)[1], 1);
    EXPECT_EQ(im(1, 1)[2], 1);
}

TEST(image, operator_call_two_arg_const) {
    auto im = make_checkerboard<float, 3>(2, 2);

    auto const im_const(im);

    EXPECT_EQ(im_const(0, 0)[0], 1);
    EXPECT_EQ(im_const(0, 0)[1], 1);
    EXPECT_EQ(im_const(0, 0)[2], 1);
    EXPECT_EQ(im_const(1, 0)[0], 0);
    EXPECT_EQ(im_const(1, 0)[1], 0);
    EXPECT_EQ(im_const(1, 0)[2], 0);
    EXPECT_EQ(im_const(0, 1)[0], 0);
    EXPECT_EQ(im_const(0, 1)[1], 0);
    EXPECT_EQ(im_const(0, 1)[2], 0);
    EXPECT_EQ(im_const(1, 1)[0], 1);
    EXPECT_EQ(im_const(1, 1)[1], 1);
    EXPECT_EQ(im_const(1, 1)[2], 1);
}

// TEST(image, operator_call_three_arg) {
//     auto im = make_checkerboard<float>(2, 2);

//     EXPECT_EQ(im(0, 0, 0), 1);
//     EXPECT_EQ(im(0, 0, 1), 1);
//     EXPECT_EQ(im(0, 0, 2), 1);
//     EXPECT_EQ(im(1, 0, 0), 1);
//     EXPECT_EQ(im(1, 0, 1), 1);
//     EXPECT_EQ(im(1, 0, 2), 1);
//     EXPECT_EQ(im(0, 1, 0), 0);
//     EXPECT_EQ(im(0, 1, 1), 0);
//     EXPECT_EQ(im(0, 1, 2), 0);
//     EXPECT_EQ(im(1, 1, 0), 0);
//     EXPECT_EQ(im(1, 1, 1), 0);
//     EXPECT_EQ(im(1, 1, 2), 0);
// }

// TEST(image, operator_call_three_arg_const) {
//     auto im = make_checkerboard<float>(2, 2);

//     auto const im_const(im);

//     EXPECT_EQ(im_const(0, 0, 0), 1);
//     EXPECT_EQ(im_const(0, 0, 1), 1);
//     EXPECT_EQ(im_const(0, 0, 2), 1);
//     EXPECT_EQ(im_const(1, 0, 0), 1);
//     EXPECT_EQ(im_const(1, 0, 1), 1);
//     EXPECT_EQ(im_const(1, 0, 2), 1);
//     EXPECT_EQ(im_const(0, 1, 0), 0);
//     EXPECT_EQ(im_const(0, 1, 1), 0);
//     EXPECT_EQ(im_const(0, 1, 2), 0);
//     EXPECT_EQ(im_const(1, 1, 0), 0);
//     EXPECT_EQ(im_const(1, 1, 1), 0);
//     EXPECT_EQ(im_const(1, 1, 2), 0);
// }

TEST(image, intensity_range) {
    // check that the intensity range is indeed constexpr
    static_assert(image<float>::min == 0,
        "image<float> intensity range does not bottom out at 0.");
    static_assert(image<float>::max == 1.f,
        "image<float> intensity range does not max out at 1.");

    // check various types' intensity ranges for correctness
    EXPECT_EQ(image<float>::min, 0.f);
    EXPECT_EQ(image<float>::max, 1.f);

    EXPECT_EQ(image<double>::min, 0.0);
    EXPECT_EQ(image<double>::max, 1.0);

    EXPECT_EQ(image<uint16_t>::min, std::numeric_limits<uint16_t>::min());
    EXPECT_EQ(image<uint16_t>::max, std::numeric_limits<uint16_t>::max());
}

TEST(image_helpers, type_to_typedesc) {
    static_assert(helpers::type_to_typedesc<double>() ==
        OIIO::TypeDesc::DOUBLE,
        "type_to_typedesc does not return DOUBLE for input of double");

    static_assert(helpers::type_to_typedesc<float>() ==
        OIIO::TypeDesc::FLOAT,
        "type_to_typedesc does not return FLOAT for input of float");

    static_assert(helpers::type_to_typedesc<uint32_t>() ==
        OIIO::TypeDesc::UINT,
        "type_to_typedesc does not return UINT for input of uint32_t");

    static_assert(helpers::type_to_typedesc<uint16_t>() ==
        OIIO::TypeDesc::UINT16,
        "type_to_typedesc does not return UINT16 for input of uint16_t");

    static_assert(helpers::type_to_typedesc<uint8_t>() ==
        OIIO::TypeDesc::UINT8,
        "type_to_typedesc does not return UINT8 for input of uint8_t");
}

// TEST(image_support, transpose) {
//     int * data_horizontal = new int[18] {
//          0,  1,  2,
//          3,  4,  5,
//          6,  7,  8,
//          9, 10, 11,
//         12, 13, 14,
//         15, 16, 17,
//     };
//     int * data_vertical = new int[18] {
//          0,  1,  2,
//          3,  4,  5,
//          6,  7,  8,
//          9, 10, 11,
//         12, 13, 14,
//         15, 16, 17,
//     };
//     image<int> i_horizontal(data_horizontal, 3,2, {"R","G","B"});
//     image<int> i_vertical(data_vertical, 2,3, {"R","G","B"});

//     auto tp_horizontal = transpose(i_horizontal);
//     auto tp_vertical = transpose(i_vertical);

//     EXPECT_EQ(2, tp_horizontal.width());
//     EXPECT_EQ(3, tp_horizontal.height());

//     EXPECT_EQ(3, tp_vertical.width());
//     EXPECT_EQ(2, tp_vertical.height());
    
//     int * tp_data_horizontal = new int[18] {
//          0,  1,  2,
//          6,  7,  8,
//         12, 13, 14,
//          3,  4,  5,
//          9, 10, 11,
//         15, 16, 17
//     };
//     int * tp_data_vertical = new int[18] {
//          0,  1,  2,
//          9, 10, 11,
//          3,  4,  5,
//         12, 13, 14,
//          6,  7,  8,
//         15, 16, 17
//     };
//     image<int> i_control_horizontal(tp_data_horizontal, 2,3, {"R","G","B"});
//     image<int> i_control_vertical(tp_data_vertical, 3,2, {"R","G","B"});

//     EXPECT_EQ(i_control_horizontal, tp_horizontal);
//     EXPECT_EQ(i_control_vertical, tp_vertical);
// }

TEST(image_support, load_image) {
    auto boat = load_image<float, 3>("../data/testing/boat.jpg");

    auto hist = statistics::histogram(boat, 50);

    // print histogram for debugging
    print::histogram(hist, 100);

    // print image for debugging
    print::image<float, 3>(boat, 10);

    EXPECT_EQ(3, boat.channels());
    EXPECT_EQ(boat.width(), 512);
    EXPECT_EQ(boat.height(), 410);

    size_t red_average = std::accumulate(
        hist[0].begin(),
        hist[0].end(),
        0)/hist[0].size();
    EXPECT_GT(red_average, 0);
    EXPECT_LT(red_average, 1500);
    size_t green_average = std::accumulate(
        hist[1].begin(),
        hist[1].end(),
        0)/hist[1].size();
    EXPECT_GT(green_average, 0);
    EXPECT_LT(green_average, 1500);
    size_t blue_average = std::accumulate(
        hist[2].begin(),
        hist[2].end(),
        0)/hist[2].size();
    EXPECT_GT(blue_average, 0);
    EXPECT_LT(blue_average, 1500);
}

// TEST(image_support, write_image) {
//     auto boat = load_image<float>("../data/testing/boat.jpg");

//     std::string out_path("../data/testing/boat_2.jpg");
//     bool written = write_image(out_path.c_str(),
//         boat, OIIO::TypeDesc::UINT8);

//     EXPECT_TRUE(written);
//     std::ifstream f(out_path.c_str());
//     EXPECT_TRUE(f.good());

//     std::remove(out_path.c_str());
// }
