#include <gtest/gtest.h>

#include <spice-lib/image.hpp>
#include <spice-lib/noise_reduction.hpp>

TEST(noise_reduction, non_local_means) {
    auto img = spice::load_image<float, 3>("../data/testing/noisy.jpg");

    auto nr = spice::non_local_means(img, 0.001, 10);

    spice::write_image("../data/testing/spice_denoised.jpg", nr);
}