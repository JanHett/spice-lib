#include <Halide.h>

namespace {

class MultiplyBufferScalar : public Halide::Generator<MultiplyBufferScalar> {
public:
    Input<Buffer<float>> lhs{"lhs", 1};
    Input<float> rhs{"rhs"};
    Output<Buffer<float>> output{"output", 1};

    void generate() {
        Var i("i");

        output(i) = lhs(i) * rhs;
    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(MultiplyBufferScalar, multiply_buffer_scalar_halide)
