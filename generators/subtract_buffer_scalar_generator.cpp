#include <Halide.h>

namespace {

class SubtractBufferScalar : public Halide::Generator<SubtractBufferScalar> {
public:
    Input<Buffer<float>> lhs{"lhs", 1};
    Input<float> rhs{"rhs"};
    Output<Buffer<float>> output{"output", 1};

    void generate() {
        Var i("i");

        output(i) = lhs(i) - rhs;
    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(SubtractBufferScalar, subtract_buffer_scalar_halide)
