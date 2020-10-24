#include <gtest/gtest.h>

#include <spice-lib/function.hpp>

// for debugging
#include <spice-lib/image.hpp>
#include <spice-lib/print.hpp>

using namespace spice;

TEST(function, gaussian_1d) {
    auto gaussian_vals = function::evaluate_unary<float, float>(
        [](float x){ return function::gaussian(3, x); }, -2.f, 4.2f, 0.5f);

    // logs for debugging
    // for (auto v : gaussian_vals) {
    //     for (size_t i = 0; i < v * 420; ++i) std::cout << "=";
    //     std::cout << " | " << v << '\n';
    // }

    EXPECT_EQ(gaussian_vals.size(), 13);

    EXPECT_FLOAT_EQ(gaussian_vals[ 0], 0.10648267);
    EXPECT_FLOAT_EQ(gaussian_vals[ 1], 0.11735511);
    EXPECT_FLOAT_EQ(gaussian_vals[ 2], 0.12579441);
    EXPECT_FLOAT_EQ(gaussian_vals[ 3], 0.13114657);
    EXPECT_FLOAT_EQ(gaussian_vals[ 4], 0.13298076);
    EXPECT_FLOAT_EQ(gaussian_vals[ 5], 0.13114657);
    EXPECT_FLOAT_EQ(gaussian_vals[ 6], 0.12579441);
    EXPECT_FLOAT_EQ(gaussian_vals[ 7], 0.11735511);
    EXPECT_FLOAT_EQ(gaussian_vals[ 8], 0.10648267);
    EXPECT_FLOAT_EQ(gaussian_vals[12], 0.054670025);
}

TEST(function, gaussian_2d) {
    float begin_x = -4.2f;
    float end_x = 2.1f;
    float step_x = 0.25f;
    float begin_y = -2.f;
    float end_y = 4.7f;
    float step_y = 0.5f;
    auto gaussian_vals = function::evaluate_binary<float, float>(
        [](float x, float y){ return function::gaussian(2, x, y); },
        begin_x, end_x, step_x,
        begin_y, end_y, step_y);

    size_t width = std::ceil((end_x - begin_x) / step_x);
    size_t height = std::ceil((end_y - begin_y) / step_y);
    
    // print the resulting image for debugging
    // std::cout << "width: " << width << ", height: " << height << "\n";
    // image<float, 1> img(gaussian_vals.data(), width, height);
    // print::image(img);

    EXPECT_EQ(gaussian_vals.size(), width * height);

    // checking all values would be too verbose, so we'll just check a few
    EXPECT_FLOAT_EQ(gaussian_vals[  0], 0.013338704 ); // first
    EXPECT_FLOAT_EQ(gaussian_vals[ 25], 0.071547166 ); // end of first row
    EXPECT_FLOAT_EQ(gaussian_vals[194], 0.12576558  ); // somewhere in the middle
    EXPECT_FLOAT_EQ(gaussian_vals[342], 0.0044124047); // in the bottom row
    EXPECT_FLOAT_EQ(gaussian_vals[363], 0.009384946 ); // last
}
