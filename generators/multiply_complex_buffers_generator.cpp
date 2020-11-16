#include <Halide.h>

namespace {

/**
 * \brief Helper function to detect GPU targets
 * 
 * Source adapted from the [Halide Tutorial](https://halide-lang.org/tutorials/tutorial_lesson_12_using_the_gpu.html).
 * 
 * \return Halide::Target 
 */
Halide::Target find_gpu_target() {
    // Start with a target suitable for the machine you're running this on.
    Halide::Target target = Halide::get_host_target();

    std::vector<Halide::Target::Feature> features_to_try;
    if (target.os == Halide::Target::Windows) {
        // Try D3D12 first; if that fails, try OpenCL.
        if (sizeof(void*) == 8) {
            // D3D12Compute support is only available on 64-bit systems at present.
            features_to_try.push_back(Halide::Target::D3D12Compute);
        }
        features_to_try.push_back(Halide::Target::OpenCL);
    } else if (target.os == Halide::Target::OSX) {
        // OS X doesn't update its OpenCL drivers, so they tend to be broken.
        // CUDA would also be a fine choice on machines with NVidia GPUs.
        features_to_try.push_back(Halide::Target::Metal);
    } else {
        features_to_try.push_back(Halide::Target::OpenCL);
    }
    // Uncomment the following lines to also try CUDA:
    features_to_try.push_back(Halide::Target::CUDA);

    for (Halide::Target::Feature f : features_to_try) {
        Halide::Target new_target = target.with_feature(f);
        if (host_supports_target_device(new_target)) {
            return new_target;
        }
    }

    printf("Requested GPU(s) are not supported. (Do you have the proper hardware and/or driver installed?)\n");
    return target;
}

class MultiplyComplexBuffers : public Halide::Generator<MultiplyComplexBuffers> {
public:
    Input<Buffer<float>> lhs{"lhs", 2};
    Input<Buffer<float>> rhs{"rhs", 2};
    /**
     * \brief The output of the multiplication.
     * This must not refer to the same memory as rhs!
     */
    Output<Buffer<float>> output{"output", 2};

    void generate() {
        Var i("i"), nothing("nothing");

        // TODO: this is ugly but necessary - avoid this if possible
        output(nothing, i) = lhs(nothing, i);

        // real part: real * real - imag * imag
        output(0, i) = lhs(0, i) * rhs(0, i) - lhs(1, i) * rhs(1, i);
        // imaginary part: real * imag + imag * real
        output(1, i) = lhs(0, i) * rhs(1, i) + lhs(1, i) * rhs(0, i);

        //
        // SCHEDULE
        //

        // slight pessimisation
        // output.parallel(i, 16);

        // Target target = find_gpu_target();
        // if (!target.has_gpu_feature()) {
        //     return;
        // }

        Var block, thread;
        // output.gpu_tile(i, block, thread,  4);
        // output.gpu_tile(i, block, thread,  8);
        // output.gpu_tile(i, block, thread, 16);
        // output.gpu_tile(i, block, thread, 32);
        // output.gpu_tile(i, block, thread, 64);
    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(MultiplyComplexBuffers,
    multiply_complex_buffers_halide)
