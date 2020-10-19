#include <benchmark/benchmark.h>

#include <spice-lib/image.hpp>

using T = float;

static void BM_read_image_small_jpg_manual_scramble(benchmark::State& state) {
    auto file = OIIO::ImageInput::open("../data/testing/boat.jpg");
    if (!file)
        return;

    const OIIO::ImageSpec & spec = file->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);

        file->read_image(spice::helpers::type_to_typedesc<T>(), img_data.data());

        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        for (auto c = 0; c < spec.nchannels; ++c) {
            for (auto y = 0; y < spec.height; ++y) {
                for (auto x = 0; x < spec.width; ++x) {
                    img_data_planar[
                        c * spec.width * spec.height +
                        y * spec.width +
                        x
                    ] = img_data[
                        y * spec.width * spec.nchannels +
                        x * spec.nchannels +
                        c
                    ];
                }
            }
        }
    }
}

BENCHMARK(BM_read_image_small_jpg_manual_scramble);

static void BM_read_image_small_jpg_separate_passes(benchmark::State& state) {
    auto file = OIIO::ImageInput::open("../data/testing/boat.jpg");
    if (!file)
        return;

    const OIIO::ImageSpec & spec = file->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        for (size_t channel = 0; channel < spec.nchannels; ++channel) {
            file->read_image(
                channel, channel + 1,
                spice::helpers::type_to_typedesc<T>(),
                &img_data_planar.data()[spec.width * spec.height * channel]);
        }
    }
}

BENCHMARK(BM_read_image_small_jpg_separate_passes);

static void BM_read_image_75mp_tiff_c1_simple_read(benchmark::State& state) {
    auto file_c1 = OIIO::ImageInput::open("../data/testing/eibsee_c1.tif");

    if (!file_c1)
        return;

    const OIIO::ImageSpec & spec = file_c1->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);

        file_c1->read_image(spice::helpers::type_to_typedesc<T>(), img_data.data());
    }

    file_c1->close();
}

BENCHMARK(BM_read_image_75mp_tiff_c1_simple_read);

static void BM_read_image_75mp_tiff_c1_manual_scramble(benchmark::State& state) {
    auto file_c1 = OIIO::ImageInput::open("../data/testing/eibsee_c1.tif");
    if (!file_c1)
        return;

    const OIIO::ImageSpec & spec = file_c1->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);

        file_c1->read_image(spice::helpers::type_to_typedesc<T>(), img_data.data());

        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        for (auto c = 0; c < spec.nchannels; ++c) {
            for (auto y = 0; y < spec.height; ++y) {
                for (auto x = 0; x < spec.width; ++x) {
                    img_data_planar[
                        c * spec.width * spec.height +
                        y * spec.width +
                        x
                    ] = img_data[
                        y * spec.width * spec.nchannels +
                        x * spec.nchannels +
                        c
                    ];
                }
            }
        }
    }

    file_c1->close();
}

BENCHMARK(BM_read_image_75mp_tiff_c1_manual_scramble);

static void BM_read_image_75mp_tiff_c1_parallel_convert_image_scramble(benchmark::State& state) {
    auto file_c1 = OIIO::ImageInput::open("../data/testing/eibsee_c1.tif");
    if (!file_c1)
        return;

    const OIIO::ImageSpec & spec = file_c1->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);

        file_c1->read_image(spice::helpers::type_to_typedesc<T>(), img_data.data());

        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        int dst_stride_x = sizeof(T);
        int dst_stride_y = dst_stride_x * spec.width;
        int dst_stride_z = dst_stride_y * spec.height;

        auto converted_r = OIIO::parallel_convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            img_data.data(),
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            img_data_planar.data(),
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);

        auto converted_g = OIIO::parallel_convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            &img_data.data()[1],
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            &img_data_planar.data()[spec.width * spec.height * spec.depth],
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);

        auto converted_b = OIIO::parallel_convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            &img_data.data()[2],
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            &img_data_planar.data()[spec.width * spec.height * spec.depth * 2],
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);
    }

    file_c1->close();
}

BENCHMARK(BM_read_image_75mp_tiff_c1_parallel_convert_image_scramble);

static void BM_read_image_75mp_tiff_c1_convert_image_scramble(benchmark::State& state) {
    auto file_c1 = OIIO::ImageInput::open("../data/testing/eibsee_c1.tif");
    if (!file_c1)
        return;

    const OIIO::ImageSpec & spec = file_c1->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);

        file_c1->read_image(spice::helpers::type_to_typedesc<T>(), img_data.data());

        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        int dst_stride_x = sizeof(T);
        int dst_stride_y = dst_stride_x * spec.width;
        int dst_stride_z = dst_stride_y * spec.height;

        auto converted_r = OIIO::convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            img_data.data(),
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            img_data_planar.data(),
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);

        auto converted_g = OIIO::convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            &img_data.data()[1],
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            &img_data_planar.data()[spec.width * spec.height * spec.depth],
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);

        auto converted_b = OIIO::convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            &img_data.data()[2],
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            &img_data_planar.data()[spec.width * spec.height * spec.depth * 2],
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);
    }

    file_c1->close();
}

BENCHMARK(BM_read_image_75mp_tiff_c1_convert_image_scramble);

static void BM_read_image_75mp_tiff_c1_separate_passes(benchmark::State& state) {
    auto file_c1 = OIIO::ImageInput::open("../data/testing/eibsee_c1.tif");
    if (!file_c1)
        return;

    const OIIO::ImageSpec & spec = file_c1->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        for (size_t channel = 0; channel < spec.nchannels; ++channel) {
            file_c1->read_image(
                channel, channel + 1,
                spice::helpers::type_to_typedesc<T>(),
                &img_data_planar.data()[spec.width * spec.height * channel]);
        }
    }

    file_c1->close();
}

BENCHMARK(BM_read_image_75mp_tiff_c1_separate_passes);

static void BM_read_image_75mp_tiff_ps_simple_read(benchmark::State& state) {
    auto file_ps = OIIO::ImageInput::open("../data/testing/eibsee_ps.tif");
    if (!file_ps)
        return;

    const OIIO::ImageSpec & spec = file_ps->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);

        file_ps->read_image(spice::helpers::type_to_typedesc<T>(), img_data.data());
    }

    file_ps->close();
}

BENCHMARK(BM_read_image_75mp_tiff_ps_simple_read);

static void BM_read_image_75mp_tiff_ps_manual_scramble(benchmark::State& state) {
    auto file_ps = OIIO::ImageInput::open("../data/testing/eibsee_ps.tif");
    if (!file_ps)
        return;

    const OIIO::ImageSpec & spec = file_ps->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);

        file_ps->read_image(spice::helpers::type_to_typedesc<T>(), img_data.data());

        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        for (auto c = 0; c < spec.nchannels; ++c) {
            for (auto y = 0; y < spec.height; ++y) {
                for (auto x = 0; x < spec.width; ++x) {
                    img_data_planar[
                        c * spec.width * spec.height +
                        y * spec.width +
                        x
                    ] = img_data[
                        y * spec.width * spec.nchannels +
                        x * spec.nchannels +
                        c
                    ];
                }
            }
        }
    }

    file_ps->close();
}

BENCHMARK(BM_read_image_75mp_tiff_ps_manual_scramble);

static void BM_read_image_75mp_tiff_ps_parallel_convert_image_scramble(benchmark::State& state) {
    auto file_ps = OIIO::ImageInput::open("../data/testing/eibsee_ps.tif");
    if (!file_ps)
        return;

    const OIIO::ImageSpec & spec = file_ps->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);

        file_ps->read_image(spice::helpers::type_to_typedesc<T>(), img_data.data());

        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        int dst_stride_x = sizeof(T);
        int dst_stride_y = dst_stride_x * spec.width;
        int dst_stride_z = dst_stride_y * spec.height;

        auto converted_r = OIIO::parallel_convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            img_data.data(),
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            img_data_planar.data(),
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);

        auto converted_g = OIIO::parallel_convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            &img_data.data()[1],
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            &img_data_planar.data()[spec.width * spec.height * spec.depth],
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);

        auto converted_b = OIIO::parallel_convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            &img_data.data()[2],
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            &img_data_planar.data()[spec.width * spec.height * spec.depth * 2],
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);
    }

    file_ps->close();
}

BENCHMARK(BM_read_image_75mp_tiff_ps_parallel_convert_image_scramble);

static void BM_read_image_75mp_tiff_ps_convert_image_scramble(benchmark::State& state) {
    auto file_ps = OIIO::ImageInput::open("../data/testing/eibsee_ps.tif");
    if (!file_ps)
        return;

    const OIIO::ImageSpec & spec = file_ps->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);

        file_ps->read_image(spice::helpers::type_to_typedesc<T>(), img_data.data());

        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        int dst_stride_x = sizeof(T);
        int dst_stride_y = dst_stride_x * spec.width;
        int dst_stride_z = dst_stride_y * spec.height;

        auto converted_r = OIIO::convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            img_data.data(),
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            img_data_planar.data(),
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);

        auto converted_g = OIIO::convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            &img_data.data()[1],
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            &img_data_planar.data()[spec.width * spec.height * spec.depth],
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);

        auto converted_b = OIIO::convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            &img_data.data()[2],
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            &img_data_planar.data()[spec.width * spec.height * spec.depth * 2],
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);
    }

    file_ps->close();
}

BENCHMARK(BM_read_image_75mp_tiff_ps_convert_image_scramble);

static void BM_read_image_75mp_tiff_ps_separate_passes(benchmark::State& state) {
    auto file_ps = OIIO::ImageInput::open("../data/testing/eibsee_ps.tif");
    if (!file_ps)
        return;

    const OIIO::ImageSpec & spec = file_ps->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        for (size_t channel = 0; channel < spec.nchannels; ++channel) {
            file_ps->read_image(
                channel, channel + 1,
                spice::helpers::type_to_typedesc<T>(),
                &img_data_planar.data()[spec.width * spec.height * channel]);
        }
    }

    file_ps->close();
}

BENCHMARK(BM_read_image_75mp_tiff_ps_separate_passes);

static void BM_read_image_75mp_tiff_separate_data_simple_read(benchmark::State& state) {
    auto file_ps_separate = OIIO::ImageInput::open("../data/testing/eibsee_separate.tif");
    if (!file_ps_separate)
        return;

    const OIIO::ImageSpec & spec = file_ps_separate->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);

        file_ps_separate->read_image(spice::helpers::type_to_typedesc<T>(), img_data.data());
    }

    file_ps_separate->close();
}

BENCHMARK(BM_read_image_75mp_tiff_separate_data_simple_read);

static void BM_read_image_75mp_tiff_separate_data_manual_scramble(benchmark::State& state) {
    auto file_ps_separate = OIIO::ImageInput::open("../data/testing/eibsee_separate.tif");
    if (!file_ps_separate)
        return;

    const OIIO::ImageSpec & spec = file_ps_separate->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);

        file_ps_separate->read_image(spice::helpers::type_to_typedesc<T>(), img_data.data());

        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        for (auto c = 0; c < spec.nchannels; ++c) {
            for (auto y = 0; y < spec.height; ++y) {
                for (auto x = 0; x < spec.width; ++x) {
                    img_data_planar[
                        c * spec.width * spec.height +
                        y * spec.width +
                        x
                    ] = img_data[
                        y * spec.width * spec.nchannels +
                        x * spec.nchannels +
                        c
                    ];
                }
            }
        }
    }

    file_ps_separate->close();
}

BENCHMARK(BM_read_image_75mp_tiff_separate_data_manual_scramble);

static void BM_read_image_75mp_tiff_separate_data_parallel_convert_image_scramble(benchmark::State& state) {
    auto file_ps_separate = OIIO::ImageInput::open("../data/testing/eibsee_separate.tif");
    if (!file_ps_separate)
        return;

    const OIIO::ImageSpec & spec = file_ps_separate->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);

        file_ps_separate->read_image(spice::helpers::type_to_typedesc<T>(), img_data.data());

        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        int dst_stride_x = sizeof(T);
        int dst_stride_y = dst_stride_x * spec.width;
        int dst_stride_z = dst_stride_y * spec.height;

        auto converted_r = OIIO::parallel_convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            img_data.data(),
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            img_data_planar.data(),
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);

        auto converted_g = OIIO::parallel_convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            &img_data.data()[1],
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            &img_data_planar.data()[spec.width * spec.height * spec.depth],
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);

        auto converted_b = OIIO::parallel_convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            &img_data.data()[2],
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            &img_data_planar.data()[spec.width * spec.height * spec.depth * 2],
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);
    }

    file_ps_separate->close();
}

BENCHMARK(BM_read_image_75mp_tiff_separate_data_parallel_convert_image_scramble);

static void BM_read_image_75mp_tiff_separate_data_convert_image_scramble(benchmark::State& state) {
    auto file_ps_separate = OIIO::ImageInput::open("../data/testing/eibsee_separate.tif");
    if (!file_ps_separate)
        return;

    const OIIO::ImageSpec & spec = file_ps_separate->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);

        file_ps_separate->read_image(spice::helpers::type_to_typedesc<T>(), img_data.data());

        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        int dst_stride_x = sizeof(T);
        int dst_stride_y = dst_stride_x * spec.width;
        int dst_stride_z = dst_stride_y * spec.height;

        auto converted_r = OIIO::convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            img_data.data(),
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            img_data_planar.data(),
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);

        auto converted_g = OIIO::convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            &img_data.data()[1],
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            &img_data_planar.data()[spec.width * spec.height * spec.depth],
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);

        auto converted_b = OIIO::convert_image(
            spec.nchannels, spec.width, spec.height, spec.depth,

            &img_data.data()[2],
            spice::helpers::type_to_typedesc<T>(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,

            &img_data_planar.data()[spec.width * spec.height * spec.depth * 2],
            spice::helpers::type_to_typedesc<T>(),
            dst_stride_x, dst_stride_y, dst_stride_z);
    }

    file_ps_separate->close();
}

BENCHMARK(BM_read_image_75mp_tiff_separate_data_convert_image_scramble);

static void BM_read_image_75mp_tiff_separate_data_separate_passes(benchmark::State& state) {
    auto file_ps_separate = OIIO::ImageInput::open("../data/testing/eibsee_separate.tif");
    if (!file_ps_separate)
        return;

    const OIIO::ImageSpec & spec = file_ps_separate->spec();
    
    for (auto _ : state) {
        std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

        for (int channel = 0; channel < spec.nchannels; ++channel) {
            file_ps_separate->read_image(
                channel, channel + 1,
                spice::helpers::type_to_typedesc<T>(),
                &img_data_planar.data()[spec.width * spec.height * channel]);
        }
    }

    file_ps_separate->close();
}

BENCHMARK(BM_read_image_75mp_tiff_separate_data_separate_passes);
