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
