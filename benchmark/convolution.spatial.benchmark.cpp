#include <benchmark/benchmark.h>

#include <spice-lib/image.hpp>
#include <spice-lib/function.hpp>
#include <spice-lib/convolution.hpp>

#include "../tools/test_utils.hpp"

using T = float;

static void BM_convolution_spatial (benchmark::State& state) {
    float std_deviation = state.range(0);
    size_t width = state.range(1);
    size_t height = state.range(2);

    auto img = make_gradient<float, 3>(width, height, 0, 1);

    // generate a filter
    float g_width  = 6 * std_deviation + 1;
    float g_height = 6 * std_deviation + 1;
    float begin    = -(g_width / 2);
    float end      =   g_width / 2;
    float step     = 1;

    auto gaussian_data = function::evaluate_binary<float, float>(
        [&](float x, float y){ return function::gaussian(std_deviation, x, y); },
        begin, end, step,
        begin, end, step);
    image<float, 1> filter(gaussian_data.data(), g_width, g_height);

    for (auto _ : state) {
        auto res = spice::convolve::spatial(img, filter);
    }
}

BENCHMARK(BM_convolution_spatial)
    // ->Complexity(benchmark::oN)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond)
    // ->Ranges({{32, 8192}, {32, 8192}})
    // ->Ranges({{32, 2048}, {512, 4096}, {512, 4096}})
    ->Ranges({{8, 128}, {32, 1024}, {32, 1024}})
    ;

static void BM_convolution_pre_separated (benchmark::State& state) {
    float std_deviation = state.range(0);
    size_t width = state.range(1);
    size_t height = state.range(2);

    auto img = make_gradient<float, 3>(width, height, 0, 1);

    // generate a filter
    float g_width  = 6 * std_deviation + 1;
    float g_height = 6 * std_deviation + 1;
    float begin    = -(g_width / 2);
    float end      =   g_width / 2;
    float step     = 1;

    auto gaussian_data = function::evaluate_unary<float, float>(
        [&](float x){ return function::gaussian(std_deviation, x); },
        begin, end, step);
    image<float, 1> filter_h(gaussian_data.data(), g_width, 1);
    image<float, 1> filter_v(gaussian_data.data(), 1, g_height);

    for (auto _ : state) {
        auto res = spice::convolve::separable(img, filter_h, filter_v);
    }
}

BENCHMARK(BM_convolution_pre_separated)
    // ->Complexity(benchmark::oN)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond)
    // ->Ranges({{32, 8192}, {32, 8192}})
    // ->Ranges({{32, 2048}, {512, 4096}, {512, 4096}})
    ->Ranges({{8, 128}, {32, 1024}, {32, 1024}})
    ;
