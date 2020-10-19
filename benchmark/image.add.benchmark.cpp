#include <benchmark/benchmark.h>

#include <spice-lib/image.hpp>

#include "../tools/test_utils.hpp"

using T = float;

static void BM_image_add_operator_plus (benchmark::State& state) {
    size_t width = state.range(0);
    size_t height = state.range(1);

    // create two gradients
    auto im1 = make_gradient<float, 3>(width, height, 0, 0.5);
    auto im2 = make_gradient<float, 3>(width, height, 0, 1);

    for (auto _ : state) {
        auto im3 = im1 + im2;
    }
}

BENCHMARK(BM_image_add_operator_plus)
    // ->Complexity(benchmark::oN)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond)
    // ->Ranges({{32, 8192}, {32, 8192}})
    ->Ranges({{512, 4096}, {512, 4096}})
    ;

static void BM_image_add_operator_plus_equals (benchmark::State& state) {
    size_t width = state.range(0);
    size_t height = state.range(1);

    // create two gradients
    auto im1 = make_gradient<float, 3>(width, height, 0, 0.5);
    auto im2 = make_gradient<float, 3>(width, height, 0, 1);

    for (auto _ : state) {
        im1 += im2;
    }
}

BENCHMARK(BM_image_add_operator_plus_equals)
    // ->Complexity(benchmark::oN)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond)
    // ->Ranges({{32, 8192}, {32, 8192}})
    ->Ranges({{512, 4096}, {512, 4096}})
    ;
