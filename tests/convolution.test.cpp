#include <gtest/gtest.h>

#include <spice-lib/function.hpp>
#include <spice-lib/convolution.hpp>

// for debugging
#include <numeric>
#include <spice-lib/print.hpp>

using namespace spice;

// TEST(convolution, separable_pre_seperated) {
//     auto img = load_image<float, 3>("../data/testing/boat.jpg");
//     auto filtered = convolve::separable(img,
//         {filter::sobel_avg<float>()}, {filter::sobel_diff<float>()});
//     // auto filtered_per_channel = convolve::separable(img, gaussian_per_channel);
// }

// TEST(convolution, separable_combined) {
//     auto gaussian
//     auto img = load_image<float, 3>("../data/testing/boat.jpg");
//     auto blurred = convolve::separable(img, gaussian);
//     auto blurred_per_channel = convolve::separable(img, gaussian_per_channel);
// }

TEST(convolution, spatial) {
    float std_deviation = 10;
    float g_width  = 6 * std_deviation + 1;
    float g_height = 6 * std_deviation + 1;
    float begin    = -(g_width / 2);
    float end      =   g_width / 2;
    float step     = 1;

    auto gaussian_data = function::evaluate_binary<float, float>(
        [&](float x, float y){ return function::gaussian(std_deviation, x, y); },
        begin, end, step,
        begin, end, step);
    image<float, 1> gaussian_filter(gaussian_data.data(), g_width, g_height);

    std::cout << "Width: " << g_width << " height: " << g_height << '\n';

    // print::image(gaussian_filter);

    // std::cout << "gaussian sum: " << std::reduce(
    //     gaussian_data.begin(), gaussian_data.end()) << "\n";

    auto img = load_image<float, 3>("../data/testing/boat.jpg");
    auto blurred = convolve::spatial(img, gaussian_filter);

    write_image("../data/testing/boat_convolved_spatial.jpg", blurred);
}

TEST(convolution, separable_pre_seperated) {
    float std_deviation = 10;
    float g_width  = 6 * std_deviation + 1;
    float g_height = 6 * std_deviation + 1;
    float begin    = -(g_width / 2);
    float end      =   g_width / 2;
    float step     = 1;

    auto gaussian_data = function::evaluate_unary<float, float>(
        [&](float x){ return function::gaussian(std_deviation, x); },
        begin, end, step);
    image<float, 1> gaussian_filter_h(gaussian_data.data(), g_width, 1);
    image<float, 1> gaussian_filter_v(gaussian_data.data(), 1, g_height);

    std::cout << "Width: " << g_width << " height: " << g_height << '\n';

    // print::image(gaussian_filter_h);
    // print::image(gaussian_filter_v);

    // std::cout << "gaussian sum: " << std::reduce(
    //     gaussian_data.begin(), gaussian_data.end()) << "\n";

    auto img = load_image<float, 3>("../data/testing/boat.jpg");
    auto blurred = convolve::separable(img,
        gaussian_filter_h, gaussian_filter_v);

    write_image("../data/testing/boat_convolved_spatial_pre_separated.jpg", blurred);
}
