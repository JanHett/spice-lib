#include <Halide.h>

namespace {

class SpatialConvolution : public Halide::Generator<SpatialConvolution> {
public:
    Input<Buffer<float>> img{"img", 3};
    // TODO: generalise to two or three-dimensional filter
    // TODO: make sure that repeating the last channel is the wanted behavior
    // (see `min` call)
    Input<Buffer<float>> filter{"filter", 3};
    Output<Buffer<float>> output{"output", 3};

    void generate() {
        // image coordinates
        Var x, y, c;

        Func padded_img("padded_img");
        padded_img = Halide::BoundaryConditions::repeat_edge(img);
        Func padded_filter("padded_filter");
        padded_filter = Halide::BoundaryConditions::repeat_edge(filter);

        RDom r(0, filter.width(),
            0, filter.height());

        // TODO: fix this for even widths and heights?
        Expr radius_h = cast<int>((cast<float>(filter.width()) - 1) / 2);
        Expr radius_v = cast<int>((cast<float>(filter.height()) - 1) / 2);

        output(x, y, c) = sum(
            padded_img(x - r.x + radius_h, y - r.y + radius_v, c) *
            padded_filter(r.x, r.y, min(filter.channels() - 1, c))
        );

        //
        // Schedule
        //

        output
            .parallel(y, 16)
            .vectorize(x, natural_vector_size(Float(32)))
            ;
    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(SpatialConvolution, spatial_convolution_halide)
