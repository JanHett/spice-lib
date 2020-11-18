#include <Halide.h>

namespace {

class DivideBufferScalar : public Halide::Generator<DivideBufferScalar> {
public:
    Input<Buffer<float>> lhs{"lhs", 1};
    Input<float> rhs{"rhs"};
    Output<Buffer<float>> output{"output", 1};

    void generate() {
        Var i("i");

        output(i) = lhs(i) / rhs;
    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(DivideBufferScalar, divide_buffer_scalar_halide)
