#ifndef SPICE_IMAGE
#define SPICE_IMAGE

#include <vector>
#include <array>
#include <stdexcept>
#include <type_traits>
#include <limits>

#include <OpenImageIO/imageio.h>

#include "color.hpp"
#include "color_view.hpp"

/**
 * Contains the entire public interface of the library.
 */
namespace spice
{

/**
 * This namespace contains utility functionality like adapters between
 * interfaces.
 */
namespace helpers
{
    /**
     * Wraps `std::enable_if_t` in such a way that the template is enabled
     * when the first two template arguments are equal as determined by
     * `std::is_same`. When enabled, this type becomes equivalent to the
     * third argument `T_result`.
     */
    template<typename T_lhs, typename T_rhs, typename T_result>
    using enable_if_same_t = std::enable_if_t<std::is_same<
        T_lhs,
        T_rhs>::value,
    T_result>;

    /** Converts a C++ double to OIIO::TypeDesc::DOUBLE. */
    template<typename T>
    constexpr enable_if_same_t<T, double, OIIO::TypeDesc>
    type_to_typedesc()
    {
        return OIIO::TypeDesc::DOUBLE;
    }
    /** Converts a C++ float to OIIO::TypeDesc::FLOAT. */
    template<typename T>
    constexpr enable_if_same_t<T, float, OIIO::TypeDesc>
    type_to_typedesc()
    {
        return OIIO::TypeDesc::FLOAT;
    }
    /** Converts a C++ uint32_t to OIIO::TypeDesc::UINT. */
    template<typename T>
    constexpr enable_if_same_t<T, uint32_t, OIIO::TypeDesc>
    type_to_typedesc()
    {
        return OIIO::TypeDesc::UINT;
    }
    /** Converts a C++ uint16_t to OIIO::TypeDesc::UINT16. */
    template<typename T>
    constexpr enable_if_same_t<T, uint16_t, OIIO::TypeDesc>
    type_to_typedesc()
    {
        return OIIO::TypeDesc::UINT16;
    }
    /** Converts a C++ uint8_t to OIIO::TypeDesc::UINT8. */
    template<typename T>
    constexpr enable_if_same_t<T, uint8_t, OIIO::TypeDesc>
    type_to_typedesc()
    {
        return OIIO::TypeDesc::UINT8;
    }
}

/**
 * \brief Represents an image
 * 
 * \tparam T 
 * \tparam Channels 
 */
template<typename T = float, size_t Channels = 4>
class image {
private:
    size_t m_width;
    size_t m_height;
    std::vector<T> m_data;
public:
    ////////////////////////////////////////////////////////////////////////////
    // MEMBER TYPES
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Alias for the type used to represent individual pixel values
     */
    using value_type = T;

    /**
     * \brief Default-construct a new image object
     * 
     */
    image(): m_width(0), m_height(0), m_data() {}

    /**
     * \brief Copy-construct a new image object
     * 
     * \param other 
     */
    image(image const & other):
        m_width(other.m_width),
        m_height(other.m_height),
        m_data(other.m_data) {}

    /**
     * \brief Move an image object
     * 
     * \param other 
     */
    image(image && other):
        m_width(other.m_width),
        m_height(other.m_height),
        m_data(std::move(other.m_data)) {}  

    /**
     * \brief Construct a new image object with the given width and height,
     * initialising all values with `T{}`
     * 
     * \param width 
     * \param height 
     */
    image(size_t width, size_t height):
        m_width(width),
        m_height(height),
        m_data(width * height * Channels, T{}) {}

    /**
     * \brief Construct a new image object with the given width and height and
     * initialise it with the provided data
     * 
     * The data array is assumed to have  the following properties:
     * - Planar memory layout (i.e. all red samples before all green samples
     * before all blue samples etc...)
     * - RGB[A[X...]] channel order
     * - length `width * height * Channels`
     * 
     * \param data pointer to data array in planar memory layout
     * \param width 
     * \param height 
     */
    image(T const * const data, size_t width, size_t height):
        m_width(width),
        m_height(height),
        m_data(data, data + width * height * Channels)
    {}

    /**
     * \brief Get direct access to the underlying data
     * 
     * \return T * const
     */
    [[nodiscard]] T * const data() noexcept {
        return m_data.data();
    }

    /**
     * \brief Get direct access to the underlying data
     * 
     * \return T * const& 
     */
    [[nodiscard]] T const * const data() const noexcept {
        return m_data.data();
    }

    /**
     * \brief Get the width of the image
     * 
     * \return size_t 
     */
    [[nodiscard]] size_t width() const {
        return m_width;
    }

    /**
     * \brief Get the height of the image
     * 
     * \return size_t 
     */
    [[nodiscard]] size_t height() const {
        return m_height;
    }

    /**
     * \brief Get the number of channels in the image
     * 
     * \return constexpr size_t 
     */
    [[nodiscard]] constexpr size_t channels() const {
        return Channels;
    }

    /**
     * \brief Get the size of the data array containing the image
     * 
     * \return constexpr size_t 
     */
    [[nodiscard]] constexpr size_t size() const {
        return m_data.size();
    }

    /**
     * \brief The value representing no emission or occlusion for this image
     * type (i.e. "black")
     */
    static const constexpr T min = color<T, Channels>::min;

    /**
     * \brief The value representing the maximum renderable emission or
     * occlusion for this image type (i.e. "white")
     */
    static const constexpr T max = color<T, Channels>::max;

    /**
     * \brief Get a reference to the sample at the specified offset
     * 
     * \param offset 
     * \return T & 
     */
    [[nodiscard]] T & operator[] (size_t offset) {
        return m_data[offset];
    }

    /**
     * \brief Get a constant reference to the sample at the specified offset
     * 
     * \param offset 
     * \return T & 
     */
    [[nodiscard]] T const & operator[] (size_t offset) const {
        return m_data[offset];
    }

    /**
     * \brief Get pixel value at specified coordinates
     * 
     * \param x The x-coordinate
     * \param y The y-coordinate
     * \return color<T, Channels>& A reference to the image's color at the given
     * coordinates
     */
    [[nodiscard]] color_view<T> operator() (int x, int y) {
        // std::cout << "Colorview starting at address " << &m_data[y * m_width + x] << "\n";
        return color_view<T>(&m_data[y * m_width + x],
            width() * height(),
            channels());
    }

    /**
     * \brief Get pixel value at specified coordinates
     * 
     * \param x The x-coordinate
     * \param y The y-coordinate
     * \return color<T, Channels>& A constant reference to the image's color at
     * the given coordinates
     */
    [[nodiscard]] color_view<T const> const operator() (int x, int y) const {
        // std::cout << "Colorview starting at address " << &m_data[y * m_width + x] << "\n";
        return color_view<T const>(&m_data[y * m_width + x],
            width() * height(),
            channels());
    }

    /**
     * \brief Compares two images
     *
     * \param lhs 
     * \param rhs 
     * \return true if the two have the same number of elements with
     * corresponding values, false otherwise
     */
    [[nodiscard]] friend bool operator==(image const & lhs, image const & rhs)
    {
        if (lhs.width() != rhs.width()
            || lhs.height() != rhs.height()
            || lhs.channels() != rhs.channels())
            return false;

        for (size_t i = 0; i < rhs.size(); ++i) {
            if (lhs[i] != rhs[i])
                return false;
        }

        return true;
    }

    /**
     * \brief Compares two images for inequality
     * 
     * \tparam T_other A type implementing the subscript operator as well as
     * `width`, `height` and `channels` member functions
     * \param lhs 
     * \param rhs 
     * \return true if the two have an unequal number of elements or
     * non-corresponding values, false otherwise
     */
    [[nodiscard]] friend bool operator!=(image const & lhs, image const & rhs)
    {
        return !(lhs == rhs);
    }

    /**
     * \brief Compares two images
     * 
     * \tparam T_other A type implementing the subscript operator as well as
     * `width`, `height` and `channels` member functions
     * \param lhs 
     * \param rhs 
     * \return true if the two have the same number of elements with
     * corresponding values, false otherwise
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<!std::is_same<image, T_other>::value, bool>
    operator==(T_other const & lhs,
        image const & rhs) {
        if (lhs.width() != rhs.width()
            || lhs.height() != rhs.height()
            || lhs.channels() != rhs.channels())
            return false;

        for (size_t i = 0; i < rhs.size(); ++i) {
            if (lhs[i] != rhs[i])
                return false;
        }

        return true;
    }

    /**
     * \brief Compares two images for inequality
     * 
     * \tparam T_other A type implementing the subscript operator as well as
     * `width`, `height` and `channels` member functions
     * \param lhs 
     * \param rhs 
     * \return true if the two have an unequal number of elements or
     * non-corresponding values, false otherwise
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<!std::is_same<image, T_other>::value, bool>
    operator!=(T_other const & lhs,
        image const & rhs) {
        return !(lhs == rhs);
    }

    /**
     * \brief Compares two images
     * 
     * \tparam T_other A type implementing the subscript operator as well as
     * `width`, `height` and `channels` member functions
     * \param lhs 
     * \param rhs 
     * \return true if the two have the same number of elements with
     * corresponding values, false otherwise
     */
    template<typename T_other>
    [[nodiscard]] friend bool operator==(image const & lhs,
        T_other const & rhs) {
        return rhs == lhs;
    }

    /**
     * \brief Compares two images for inequality
     * 
     * \tparam T_other A type implementing the subscript operator as well as
     * `width`, `height` and `channels` member functions
     * \param lhs 
     * \param rhs 
     * \return true if the two have an unequal number of elements or
     * non-corresponding values, false otherwise
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<!std::is_same<image, T_other>::value, bool>
    operator!=(image const & lhs,
        T_other const & rhs)
    {
        return !(rhs == lhs);
    }
};

// /**
//  * Transposes the image pixel-wise
//  *
//  * \param img The image to be rotated
//  * \returns a transposed copy of the image
//  */
// template<typename T>
// [[nodiscard]] image<T> transpose (image<T> const & img)
// {
//     image<T> new_i(img.height(), img.width(), img.channel_semantics());

//     for (size_t x = 0; x < img.width(); ++x)
//         for (size_t y = 0; y < img.height(); ++y)
//             new_i(y, x) = img(x, y);

//     return new_i;
// }

template<typename T, size_t Channels>
[[nodiscard]] image<T, Channels> load_image(char const * filename,
    const OIIO::ImageSpec * config = nullptr)
{
    auto file = OIIO::ImageInput::open(filename, config);
    // TODO more expressive error handling
    if (!file)
        return image<T, Channels>();

    const OIIO::ImageSpec & spec = file->spec();

    std::vector<T> img_data_planar(spec.width * spec.height * spec.nchannels);

    for (auto channel = 0; channel < spec.nchannels; ++channel) {
        file->read_image(
            channel, channel + 1,
            helpers::type_to_typedesc<T>(),
            &img_data_planar.data()[spec.width * spec.height * channel]);

    }

    image<T, Channels> result(
        img_data_planar.data(),
        spec.width,
        spec.height
        );

    file->close();

    return result;
}

// /**
//  * Writes an image to a file at the specified location.
//  *
//  * \param filename The path to write to relative to the current working
//  * directory
//  * \param data The image to save to disk
//  * \param format The data format the file should be written with
//  * \returns `true` if the image was successfully written, `false` if an
//  * error occurred.
//  */
template<typename T, size_t Channels>
bool write_image(
    char const * filename,
    image<T, Channels> const & data,
    OIIO::TypeDesc const & format = helpers::type_to_typedesc<T>())
{
    std::unique_ptr<OIIO::ImageOutput> out =
        OIIO::ImageOutput::create(filename);
    if (!out)
        return false;

    // interleave image before writing
    std::vector<T> interleaved(data.width() * data.height() * data.channels());
    for (size_t c = 0; c < data.channels(); ++c) {
        for (size_t y = 0; y < data.height(); ++y) {
            for (size_t x = 0; x < data.width(); ++x) {
                interleaved[
                    y * data.width() * data.channels() +
                    x * data.channels() +
                    c
                ] = data[
                    c * data.width() * data.height() +
                    y * data.width() +
                    x
                ];
            }
        }
    }


    OIIO::ImageSpec spec(
        data.width(),
        data.height(),
        data.channels(),
        format);
    out->open(filename, spec);
    out->write_image(helpers::type_to_typedesc<T>(),
        interleaved.data());
    out->close();

    return true;
}

}

#endif // SPICE_IMAGE
