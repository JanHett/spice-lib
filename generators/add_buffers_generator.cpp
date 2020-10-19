#include <Halide.h>

namespace {

class AddBuffers : public Halide::Generator<AddBuffers> {
public:
    Input<Buffer<float>> lhs{"lhs", 1};
    Input<Buffer<float>> rhs{"rhs", 1};
    Output<Buffer<float>> output{"output", 1};

    void generate() {
        Var i("i");

        output(i) = rhs(i) + lhs(i);
    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(AddBuffers, add_buffers_halide)
