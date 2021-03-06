#include <Halide.h>

namespace {

class MultiplyBuffers : public Halide::Generator<MultiplyBuffers> {
public:
    Input<Buffer<float>> lhs{"lhs", 1};
    Input<Buffer<float>> rhs{"rhs", 1};
    Output<Buffer<float>> output{"output", 1};

    void generate() {
        Var i("i");

        output(i) = lhs(i) * rhs(i);
    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(MultiplyBuffers, multiply_buffers_halide)
