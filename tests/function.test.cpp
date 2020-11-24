#include <gtest/gtest.h>

#include <numeric>

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

TEST(function, gaussian_2d_symmetric) {
    float std_deviation = 10;
    float g_width  = 3 * std_deviation;
    float g_height = 3 * std_deviation;
    float begin    = -(g_width / 2);
    float end      =   g_width / 2;
    float step     = 1;

    auto gaussian_vals = function::evaluate_binary<float, float>(
        [](float x, float y){ return function::gaussian(2, x, y); },
        begin, end, step,
        begin, end, step);

    size_t width = std::ceil((end - begin) / step);
    size_t height = std::ceil((end - begin) / step);
    
    // print the resulting image for debugging
    // std::cout << "width: " << width << ", height: " << height << "\n";
    // image<float, 1> img(gaussian_vals.data(), width, height);
    // print::image(img);

    EXPECT_EQ(gaussian_vals.size(), width * height);

    // sanity check: sum of all values should approach 1
    auto gaussian_sum = std::accumulate(
        gaussian_vals.begin(), gaussian_vals.end(),
        0.f);
    // std::cout << "gaussian sum: " << gaussian_sum << "\n";
    EXPECT_FLOAT_EQ(gaussian_sum, 1.f);
}

TEST(function, gaussian_2d_asymmetric) {
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
    EXPECT_FLOAT_EQ(gaussian_vals[  0], 0.0026606864 ); // first
    EXPECT_FLOAT_EQ(gaussian_vals[ 25], 0.014271595  ); // end of first row
    EXPECT_FLOAT_EQ(gaussian_vals[194], 0.025086602  ); // somewhere in the middle
    EXPECT_FLOAT_EQ(gaussian_vals[342], 0.00088014739); // in the bottom row
    EXPECT_FLOAT_EQ(gaussian_vals[363], 0.0018720259 ); // last
}
