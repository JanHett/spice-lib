#include <gtest/gtest.h>

#include <spice-lib/function.hpp>
#include <spice-lib/convolution.hpp>

// for debugging
#include <numeric>
#include <spice-lib/print.hpp>

using namespace spice;

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

    // print::image(gaussian_filter);

    // std::cout << "gaussian sum: " << std::reduce(
    //     gaussian_data.begin(), gaussian_data.end()) << "\n";

    auto img = load_image<float, 3>("../data/testing/boat.jpg");
    auto blurred = convolve::spatial(img, gaussian_filter);

    // test if result is reasonable

    auto blurred_expectation = load_image<float, 3>(
        "../data/testing/boat_gaussian_deviation_10.jpg");

    ASSERT_EQ(blurred_expectation.width(), blurred.width());
    ASSERT_EQ(img.width(), blurred.width());
    ASSERT_EQ(blurred_expectation.height(), blurred.height());
    ASSERT_EQ(img.height(), blurred.height());
    ASSERT_EQ(img.channels(), blurred.channels());

    for (size_t y = 0; y < blurred.height(); ++y) {
        for (size_t x = 0; x < blurred.width(); ++x) {
            for (size_t c = 0; c < blurred.channels(); ++c) {
                // Assert that difference between expectation and reality lies
                // somewhere in the to-be-expected room for error with JPG
                ASSERT_NEAR(blurred_expectation(x, y, c), blurred(x, y, c),
                    0.016f);
            }
        }
    }
    // for (size_t i = 0; i < blurred.size(); ++i) {
    //     ASSERT_FLOAT_EQ(blurred_expectation.data()[i], blurred.data()[i]);
    // }
    
    // write_image("../data/testing/boat_convolved_spatial.jpg", blurred);
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

    // print::image(gaussian_filter_h);
    // print::image(gaussian_filter_v);

    // std::cout << "gaussian sum: " << std::reduce(
    //     gaussian_data.begin(), gaussian_data.end()) << "\n";

    auto img = load_image<float, 3>("../data/testing/boat.jpg");
    auto blurred = convolve::separable(img,
        gaussian_filter_h, gaussian_filter_v);

    // test if result is reasonable

    auto blurred_expectation = load_image<float, 3>(
        "../data/testing/boat_gaussian_deviation_10.jpg");

    ASSERT_EQ(blurred_expectation.width(), blurred.width());
    ASSERT_EQ(img.width(), blurred.width());
    ASSERT_EQ(blurred_expectation.height(), blurred.height());
    ASSERT_EQ(img.height(), blurred.height());
    ASSERT_EQ(img.channels(), blurred.channels());

    for (size_t y = 0; y < blurred.height(); ++y) {
        for (size_t x = 0; x < blurred.width(); ++x) {
            for (size_t c = 0; c < blurred.channels(); ++c) {
                // Assert that difference between expectation and reality lies
                // somewhere in the to-be-expected room for error with JPG
                ASSERT_NEAR(blurred_expectation(x, y, c), blurred(x, y, c),
                    0.016f);
            }
        }
    }
    // for (size_t i = 0; i < blurred.size(); ++i) {
    //     ASSERT_FLOAT_EQ(blurred_expectation.data()[i], blurred.data()[i]);
    // }
    
    // write_image("../data/testing/boat_convolved_spatial_pre_separated.jpg",
    //     blurred);
}

TEST(convolution, separable_combined) {
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

    // print::image(gaussian_filter);

    // std::cout << "gaussian sum: " << std::reduce(
    //     gaussian_data.begin(), gaussian_data.end()) << "\n";

    auto img = load_image<float, 3>("../data/testing/boat.jpg");
    auto blurred = convolve::separable(img, gaussian_filter);

    // test if result is reasonable

    auto blurred_expectation = load_image<float, 3>(
        "../data/testing/boat_gaussian_deviation_10.jpg");

    ASSERT_EQ(blurred_expectation.width(), blurred.width());
    ASSERT_EQ(img.width(), blurred.width());
    ASSERT_EQ(blurred_expectation.height(), blurred.height());
    ASSERT_EQ(img.height(), blurred.height());
    ASSERT_EQ(img.channels(), blurred.channels());

    for (size_t y = 0; y < blurred.height(); ++y) {
        for (size_t x = 0; x < blurred.width(); ++x) {
            for (size_t c = 0; c < blurred.channels(); ++c) {
                // Assert that difference between expectation and reality lies
                // somewhere in the to-be-expected room for error with JPG
                ASSERT_NEAR(blurred_expectation(x, y, c), blurred(x, y, c),
                    0.016f);
            }
        }
    }
    // for (size_t i = 0; i < blurred.size(); ++i) {
    //     ASSERT_FLOAT_EQ(blurred_expectation.data()[i], blurred.data()[i]);
    // }
    
    // write_image("../data/testing/boat_convolved_spatial_auto_separated.jpg",
    //     blurred);
}

TEST(convolution, frequency_space) {
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

    // print::image(gaussian_filter);

    // std::cout << "gaussian sum: " << std::reduce(
    //     gaussian_data.begin(), gaussian_data.end()) << "\n";

    auto img = load_image<float, 3>("../data/testing/boat.jpg");
    auto blurred = convolve::frequency_space(img, gaussian_filter);

    // test if result is reasonable

    auto blurred_expectation = load_image<float, 3>(
        "../data/testing/boat_dft_gaussian_deviation_10.jpg");

    ASSERT_EQ(blurred_expectation.width(), blurred.width());
    ASSERT_EQ(img.width(), blurred.width());
    ASSERT_EQ(blurred_expectation.height(), blurred.height());
    ASSERT_EQ(img.height(), blurred.height());
    ASSERT_EQ(img.channels(), blurred.channels());

    for (size_t y = 0; y < blurred.height(); ++y) {
        for (size_t x = 0; x < blurred.width(); ++x) {
            for (size_t c = 0; c < blurred.channels(); ++c) {
                // Assert that difference between expectation and reality lies
                // somewhere in the to-be-expected room for error with JPG
                ASSERT_NEAR(blurred_expectation(x, y, c), blurred(x, y, c),
                    0.018f);
            }
        }
    }
    // for (size_t i = 0; i < blurred.size(); ++i) {
    //     ASSERT_FLOAT_EQ(blurred_expectation.data()[i], blurred.data()[i]);
    // }
    
    // write_image("../data/testing/boat_convolved_dft.jpg", blurred);
}
