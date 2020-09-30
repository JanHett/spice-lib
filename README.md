![spice-lib. an image processing library.](./doc/assets/branding/spice_splash.png)

# spice-lib. An image processing library.

spice-lib, or spice for short, is a set of image processing tools built on top of custom data types. All of those tools are written with an RGB(A) colour model in mind.

spice also provides a small wrapper around [OpenImageIO](https://openimageio.readthedocs.io/) reading and writing operations to facilitate input and output.

## Examples

### Reading an image, calculating its histogram and printing it to the terminal

> **Note:** This will only work on terminals that can display full 8bpc colour.

```c++
using namespace spice;

// read an image from a file
auto boat = load_image<float, 3>("./data/testing/boat.jpg");

// print the image to stdout
std::cout << "Printing images to the console is fast and easy!\n";
print::image<float, 3>(boat, 10);

// calculate a histogram with 50 "buckets"
auto hist = statistics::histogram(boat, 50);

// print the histogram to stdout - the peaks will be scaled to a height of 100 characters
print::histogram(hist, 100);
```

This will result in the following output:

![print_image_to_stdout](./doc/assets/showcase/print_image_to_stdout.png)
![print_image_to_stdout](./doc/assets/showcase/histogram_to_stdout.png)

Not glamorous, but for retro-cool and debugging it does the job.

### Pixel arithmetic

```c++
using namespace spice;

color<float, 3> c1{0.2, 0.47, 1};
auto c2 = c1 * 0.5;
c1 += c2;

std::cout << c1 << ", " << c2 << '\n';
```

Output

```

```

## Building

### Prerequisites

#### For the library

- [C++17](https://en.cppreference.com/w/cpp/compiler_support)
- [CMake 3.10](https://cmake.org/)
- [OpenImageIO](https://github.com/OpenImageIO/oiio)
- [Guideline Support Library](https://github.com/microsoft/GSL) (included as a git submodule, will be pulled by CMake during configuration)

#### For the tests

- [Google Test](https://github.com/google/googletest)

#### For the documentation

- [Doxygen](http://www.doxygen.nl/)
- [m.css](https://github.com/mosra/m.css) (included as a git submodule, will be pulled by CMake during configuration if building documentation is enabled)

### Build It!

```bash
cd <spice repo directory>
mkdir build
cd build
# tests are enabled by default, use option ENABLE_TESTS to override
cmake .. -DCMAKE_INSTALL_PREFIX=<where spice should be installed> [-DENABLE_TESTS=OFF|ON] [-DENABLE_DOCS=OFF|ON]
cmake --build . --config Debug|Release [--target [install] [doc]]
# if you chose to build the tests, you can test if everything worked
tests/spice-test
```

### Notes

spice has been tested to build with clang on macOS and may or may not build in other configurations. I plan to extend support to other operating systems and platforms, but for now, features are the primary focus.

## Contributing

If you found this project useful enough to add your two cents (or lines of code), please do!

Here's a few ways you can have a part in driving this library forward:

- use it, note what's working and what isn't and tell me about it
- review the code
- propose a specific fix or - even better...
- submit a pull request

If you submit a pull request, make sure all added functionality is thoroughly (unit) tested.

If you spot a bug or a vulnerability, please [create an issue](https://github.com/JanHett/spice/issues/new) describing the malfunction. I will do my best to look into it within a few days.

## History

This is the fourth iteration of a "learning-by-failing-and-starting-over" project of mine. The idea is to build an understanding for image processing by building this library from the ground up.

## Credits

The build system has been adapted from cmake init: https://github.com/cginternals/cmake-init

Backing image of spice splash screen and favicon: Crab Nebula by NASA/STScI: https://hubblesite.org/contents/media/images/2005/37/1823-Image.html?news=true