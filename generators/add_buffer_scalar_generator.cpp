#include <Halide.h>

namespace {

class AddBufferScalar : public Halide::Generator<AddBufferScalar> {
public:
    Input<Buffer<float>> lhs{"lhs", 1};
    Input<float> rhs{"rhs"};
    Output<Buffer<float>> output{"output", 1};

    void generate() {
        Var i("i");

        output(i) = lhs(i) + rhs;
    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(AddBufferScalar, add_buffer_scalar_halide)
