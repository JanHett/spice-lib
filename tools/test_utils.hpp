#include <spice-lib/image.hpp>

using namespace spice;

/**
 * \brief Create a checkerboard image. The top-left pixel will be white.
 * 
 * \tparam T 
 * \param width 
 * \param height 
 * \param channel_semantics 
 * \return image<T> 
 */
template<typename T, size_t Channels = 3>
image<T, Channels> make_checkerboard(size_t width = 1,
    size_t height = 1)
{
    image<T, Channels> im(width, height);
    for (size_t y = 0; y < im.height(); ++y)
        for (size_t x = 0; x < im.width(); ++x)
            im(x, y) = (x + y) % 2 == 0 ?
                image<T>::max :
                image<T>::min;
    // for (size_t pxl = 0; pxl < im.data().size(); ++pxl)
    //     im.data()[pxl] = pxl % 2 == 0 ?
    //         image<T>::max :
    //         image<T>::min;

    return im;
}

/**
 * \brief Calculate a value of a diagonal monochrome gradient that begins with
 * value `min` in the top-left corner and ends with value `max` in the
 * bottom-right
 * 
 * \tparam T 
 * \param width The width of the gradient window
 * \param height The height of the gradient window
 * \param min 
 * \param max 
 * \param x The x-coordinate to calculate the gradient for
 * \param y The y-coordinate to calculate the gradient for
 * \return color<T, Channels> 
 */
template<typename T, size_t Channels>
T calculate_gradient(
    size_t width, size_t height,
    T const & min, T const & max,
    size_t x, size_t y)
{
    return (static_cast<float>(x) / width
        + static_cast<float>(y) / height)
        / 2
        * (max - min) + min;
}

/**
 * \brief Create an image containing a diagonal monochrome gradient that begins
 * with value `min` in the top-left corner and ends with value `max` in the
 * bottom-right
 * 
 * \tparam T 
 * \tparam Channels 
 * \param width The width of the gradient window
 * \param height The height of the gradient window
 * \param min 
 * \param max 
 * \return color<T, Channels> 
 */
template<typename T, size_t Channels>
image<T, Channels> make_gradient(
    size_t width, size_t height,
    T const & min, T const & max
)
{
    image<T, Channels> gradient(width, height);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            gradient(x, y) = calculate_gradient<T, Channels>(width, height,
                min, max, x, y);;
        }
    }

    return gradient;
}
