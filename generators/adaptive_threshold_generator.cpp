#include <Halide.h>

namespace {

class AdaptiveThreshold : public Halide::Generator<AdaptiveThreshold> {
public:
    Input<Buffer<float>> input{"input", 3};
    Input<float> threshold{"threshold"};
    Input<int> radius{"radius"};
    Output<Buffer<float>> output{"output", 3};

    void generate() {
        Var x("x"), y("y"), c("c");

        auto diameter = radius * 2 + 1;

        Func clamped("clamped");

        clamped = Halide::BoundaryConditions::repeat_edge(input);

        // horizontal blur

        RDom r_x(0, diameter);

        Func horizontal_blur("horizontal_blur");

        horizontal_blur(x, y, c) = sum(clamped(x + r_x, y, c)) / diameter;

        // vertical blur

        RDom r_y(0, diameter);

        Func vertical_blur("vertical_blur");

        vertical_blur(x, y, c) = sum(horizontal_blur(x, y + r_y, c)) / diameter;

        // threshold

        output(x, y, c) = Halide::cast<float>(vertical_blur(x, y, c) > threshold);

        //
        // SCHEDULING
        // 

        // setting the memory layout straight
        input.dim(0).set_stride(3);
        input.dim(2).set_stride(1);
        input.dim(2).set_bounds(0, 3);

        output.dim(0).set_stride(3);
        output.dim(2).set_stride(1);
        output.dim(2).set_bounds(0, 3);

        output.reorder(c, x, y).unroll(c);

        // The actual schedule

        horizontal_blur.compute_root();
        horizontal_blur.parallel(y, 16);
        horizontal_blur.vectorize(x, natural_vector_size(Float(32)));

        output.parallel(y, 16);
        output.vectorize(x, natural_vector_size(Float(32)));
    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(AdaptiveThreshold, adaptive_threshold_generator)
