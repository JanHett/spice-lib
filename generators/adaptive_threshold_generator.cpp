#include <Halide.h>

namespace {

class AdaptiveThreshold : public Halide::Generator<AdaptiveThreshold> {
public:
    Input<Buffer<float>> input{"input", 3};
    Input<float> threshold{"threshold"};
    Input<size_t> radius{"radius"};
    Output<Buffer<float>> output{"output", 3};

    void generate() {
        Var x("x"), y("y"), c("c");

        // TODO

        output(x, y, c) = input(x, y, c);
    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(AdaptiveThreshold, adaptive_threshold_generator)
