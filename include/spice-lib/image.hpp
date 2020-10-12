#ifndef SPICE_IMAGE
#define SPICE_IMAGE

#include <vector>
#include <array>
#include <stdexcept>
#include <type_traits>
#include <limits>

#include <OpenImageIO/imageio.h>

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
 * Represents a color.
 */
template<typename T = float, size_t Channels = 4>
class color {
private:
    std::array<T, Channels> m_data;
public:
    ////////////////////////////////////////////////////////////////////////////
    // STATIC MEMBERS
    ////////////////////////////////////////////////////////////////////////////
    /**
     * \brief The value representing no emission or occlusion for this image
     * type (i.e. "black")
     */
    static const constexpr T min = ([]() -> T {
        if (std::is_floating_point<T>::value) return T{};
        return std::numeric_limits<T>::min();
    })();

    /**
     * \brief The value representing the maximum renderable emission or
     * occlusion for this image type (i.e. "white")
     */
    static const constexpr T max = ([]() -> T {
        if (std::is_floating_point<T>::value) return T(1.f);
        return std::numeric_limits<T>::max();
    })();

    ////////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Construct a new color object representing (transparent) black.
     */
    color (): m_data{ color<T, Channels>::min } {}

    /**
     * \brief Create a copy of the given color object
     * 
     * \param other The color to copy
     */
    color (color const & other) = default;

    /**
     * \brief Move the the given color object
     * 
     * \param other The color to copy
     */
    color (color && other) = default;

    /**
     * \brief Construct a new color object initialised with the given value
     * 
     * \param value 
     */
    color (T const & value) {
        m_data.fill(value);
    }

    /**
     * \brief Construct a new color object with given values
     *
     * \param values The channel values
     */
    color (std::initializer_list<T> values) {
        std::copy(values.begin(), values.end(), m_data.begin());
    }

    /**
     * \brief Construct a new color object with given values
     * 
     * The values are assumed to be of length `Channels`
     *
     * \param values The channel values
     */
    color (T const * const values) {
        std::copy(values, values + Channels, m_data.begin());
    }

    ////////////////////////////////////////////////////////////////////////////
    // ASSIGNMENT OPERATORS
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Copy-assigns another color
     * 
     * \param other 
     * \return color& 
     */
    color& operator=(color const & other) noexcept = default;

    /**
     * \brief Move-assigns another color
     * 
     * \param other 
     * \return color& 
     */
    color& operator=(color && other) noexcept = default;

    /**
     * \brief Assign each channel the provided value
     * 
     * \param other 
     * \return color& 
     */
    color& operator=(T const & value) noexcept {
        m_data.fill(value);
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////
    // SIZE GETTERS
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Returns the channel count
     * 
     * \return size_t 
     */
    constexpr size_t size() const {
        return Channels;
    }

    /**
     * \brief Returns the channel count
     * 
     * \return size_t 
     */
    constexpr size_t channels() const {
        return Channels;
    }

    ////////////////////////////////////////////////////////////////////////////
    // COMPARISON OPERATORS
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Compares a color component-wise with a scalar
     * 
     * \tparam T_other A scalar that is comparable to the color's data type `T`
     * \param lhs 
     * \param rhs 
     * \return true if all elements of the color correspond to the scalar value,
     * false otherwise
     * 
     * \todo refactor this to work for any type that is convertible to `T`
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<std::is_scalar<T_other>::value, bool>
    operator==(T_other const & lhs,
        color const & rhs)
    {
        for (size_t i = 0; i < rhs.size(); ++i) {
            if (lhs != rhs[i])
                return false;
        }

        return true;
    }

    /**
     * \brief Compares a color component-wise with a scalar
     * 
     * \tparam T_other A scalar that is comparable to the color's data type `T`
     * \param lhs 
     * \param rhs 
     * \return true if all elements of the color correspond to the scalar value,
     * false otherwise
     * 
     * \todo refactor this to work for any type that is convertible to `T`
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<std::is_scalar<T_other>::value, bool>
    operator==(color const & lhs,
        T_other const & rhs)
    {
        return rhs == lhs;
    }

    /**
     * \brief Compares a color component-wise with a scalar for inequality
     * 
     * \tparam T_other A scalar that is comparable to the color's data type `T`
     * \param lhs 
     * \param rhs 
     * \return true if any of the elements of the color do not correspond to the
     * scalar value, false otherwise
     * 
     * \todo refactor this to work for any type that is convertible to `T`
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<std::is_scalar<T_other>::value, bool>
    operator!=(T_other const & lhs,
        color const & rhs)
    {
        return !(lhs == rhs);
    }

    /**
     * \brief Compares a color component-wise with a scalar for inequality
     * 
     * \tparam T_other A scalar that is comparable to the color's data type `T`
     * \param lhs 
     * \param rhs 
     * \return true if any of the elements of the color do not correspond to the
     * scalar value, false otherwise
     * 
     * \todo refactor this to work for any type that is convertible to `T`
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<std::is_scalar<T_other>::value, bool>
    operator!=(color const & lhs,
        T_other const & rhs)
    {
        return !(rhs == lhs);
    }

    /**
     * \brief Compares two colors for equality
     * 
     * \param lhs 
     * \param rhs 
     * \return true if the two have the same number of elements with
     * corresponding values, false otherwise
     */
    [[nodiscard]] friend bool operator==(color const & lhs, color const & rhs)
    {
        if (lhs.size() != rhs.size())
            return false;

        for (size_t i = 0; i < rhs.size(); ++i) {
            if (lhs[i] != rhs[i])
                return false;
        }

        return true;
    }

    /**
     * \brief Compares two colors for inequality
     * 
     * \param lhs 
     * \param rhs 
     * \return true if the two do not have the same number of elements or any of
     * the values do not correspond, false otherwise
     */
    [[nodiscard]] friend bool operator!=(color const & lhs, color const & rhs)
    {
        return !(lhs == rhs);
    }

    /**
     * \brief Compares two colors
     * 
     * \tparam T_other A type implementing the subscript operator as well as a
     * `size` member function
     * \param lhs 
     * \param rhs 
     * \return true if the two have the same number of elements with
     * corresponding values, false otherwise
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<
        !std::is_same<color, T_other>::value && !std::is_scalar<T_other>::value,
        bool
    >
    operator==(T_other const & lhs,
        color const & rhs) {
        if (lhs.size() != rhs.size())
            return false;

        for (size_t i = 0; i < rhs.size(); ++i) {
            if (lhs[i] != rhs[i])
                return false;
        }

        return true;
    }

    /**
     * \brief Compares two colors for inequality
     * 
     * \param lhs 
     * \param rhs 
     * \return true if the two do not have the same number of elements or any of
     * the values do not correspond, false otherwise
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<
        !std::is_same<color, T_other>::value && !std::is_scalar<T_other>::value,
        bool
    >
    operator!=(color const & lhs, T_other const & rhs)
    {
        return !(lhs == rhs);
    }

    /**
     * \brief Compares two colors
     * 
     * \tparam T_other A type implementing the subscript operator as well as a
     * `size` member function
     * \param lhs 
     * \param rhs 
     * \return true if the two have the same number of elements with
     * corresponding values, false otherwise
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<
        !std::is_same<color, T_other>::value && !std::is_scalar<T_other>::value,
        bool
    >
    operator==(color const & lhs,
        T_other const & rhs) {
        return rhs == lhs;
    }

    /**
     * \brief Compares two colors for inequality
     * 
     * \param lhs 
     * \param rhs 
     * \return true if the two do not have the same number of elements or any of
     * the values do not correspond, false otherwise
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<
        !std::is_same<color, T_other>::value && !std::is_scalar<T_other>::value,
        bool
    >
    operator!=(T_other const & lhs, color const & rhs)
    {
        return !(rhs == lhs);
    }

    ////////////////////////////////////////////////////////////////////////////
    // CHANNEL ACCESS OPERATORS
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Retrieve a reference to an element of the color
     * 
     * \param index 
     * \return T& 
     */
    [[nodiscard]] T& operator[] (size_t index) {
        return m_data[index];
    }

    /**
     * \brief Retrieve a constant reference to an element of the color
     * 
     * \param index 
     * \return T& 
     */
    [[nodiscard]] T const & operator[] (size_t index) const {
        return m_data[index];
    }

    ////////////////////////////////////////////////////////////////////////////
    // ARITHMETIC OPERATORS
    ////////////////////////////////////////////////////////////////////////////

    // ~~~ ADDITION ~~~ //

    /**
     * \brief Add `rhs` to this color channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
     color & operator+=(color const & rhs)
    {
        for (size_t c = 0; c < m_data.size(); ++c) {
            m_data[c] += rhs.m_data[c];
        }
        return *this;
    }
    
    /**
     * \brief Add `rhs` to `lhs` channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
    [[nodiscard]] friend color operator+(color lhs, color const & rhs)
    {
        lhs += rhs;
        return lhs;
    }

    /**
     * \brief Add `rhs` to this color channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
     color & operator+=(T const & rhs)
    {
        for (size_t c = 0; c < m_data.size(); ++c) {
            m_data[c] += rhs;
        }
        return *this;
    }

    /**
     * \brief Add `rhs` to `lhs` channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
    [[nodiscard]] friend color operator+(color lhs, T const & rhs)
    {
        lhs += rhs;
        return lhs;
    }

    // ~~~ SUBTRACTION ~~~ //

    /**
     * \brief Subtract `rhs` from this color channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
     color & operator-=(color const & rhs)
    {
        for (size_t c = 0; c < m_data.size(); ++c) {
            m_data[c] -= rhs.m_data[c];
        }
        return *this;
    }
    
    /**
     * \brief Subtract `rhs` from `lhs` channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
    [[nodiscard]] friend color operator-(color lhs, color const & rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    /**
     * \brief Subtract `rhs` from this color channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
     color & operator-=(T const & rhs)
    {
        for (size_t c = 0; c < m_data.size(); ++c) {
            m_data[c] -= rhs;
        }
        return *this;
    }

    /**
     * \brief Subtract `rhs` from `lhs` channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
    [[nodiscard]] friend color operator-(color lhs, T const & rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    // ~~~ MULTIPLICATION ~~~ //

    /**
     * \brief Multiply this color with `rhs` channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
     color & operator*=(color const & rhs)
    {
        for (size_t c = 0; c < m_data.size(); ++c) {
            m_data[c] *= rhs.m_data[c];
        }
        return *this;
    }
    
    /**
     * \brief Multiply `lhs` with `rhs` channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
    [[nodiscard]] friend color operator*(color lhs, color const & rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    /**
     * \brief Multiply this color with `rhs` channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
     color & operator*=(T const & rhs)
    {
        for (size_t c = 0; c < m_data.size(); ++c) {
            m_data[c] *= rhs;
        }
        return *this;
    }

    /**
     * \brief Multiply `lhs` with `rhs` channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
    [[nodiscard]] friend color operator*(color lhs, T const & rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    // ~~~ DIVISION ~~~ //

    /**
     * \brief Divide this color by `rhs` channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
     color & operator/=(color const & rhs)
    {
        for (size_t c = 0; c < m_data.size(); ++c) {
            m_data[c] /= rhs.m_data[c];
        }
        return *this;
    }
    
    /**
     * \brief Divide `lhs` by `rhs` channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
    [[nodiscard]] friend color operator/(color lhs, color const & rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    /**
     * \brief Divide this color by `rhs` channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
     color & operator/=(T const & rhs)
    {
        for (size_t c = 0; c < m_data.size(); ++c) {
            m_data[c] /= rhs;
        }
        return *this;
    }

    /**
     * \brief Divide `lhs` by `rhs` channel-wise
     * 
     * \param rhs 
     * \return color& 
     */
    [[nodiscard]] friend color operator/(color lhs, T const & rhs)
    {
        lhs /= rhs;
        return lhs;
    }
};

/**
 * Specialized formatter for `spice::color`.
 */
template<typename T, size_t Channels>
std::ostream& operator<<(std::ostream& os, color<T, Channels> const & pxl)
{
    os << "color(";
    for (size_t idx = 0; idx < pxl.size() - 1; ++idx)
        os << pxl[idx] << ", ";
    os << pxl[pxl.size() - 1];
    os << ")";
    return os;
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
    std::vector<color<T, Channels>> m_data;
public:
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
        m_data(width * height, color<T, Channels>()) {}

    /**
     * \brief Construct a new image object with the given width and height and
     * initialise it with the provided data
     * 
     * The data array is assumed to be of length `width * height * Channels`.
     * 
     * \param data 
     * \param width 
     * \param height 
     */
    image(T const * const data, size_t width, size_t height):
        m_width(width),
        m_height(height),
        m_data(width * height, T{})
    {
        for (size_t i = 0; i < m_data.size(); ++i) {
            m_data[i] = color<T, Channels>(data + (i * Channels));
        }
    }

    /**
     * \brief Get direct access to the underlying data
     * 
     * \return std::vector<color<T, Channels>>& 
     */
    std::vector<color<T, Channels>> & data() {
        return m_data;
    }

    /**
     * \brief Get direct access to the underlying data
     * 
     * \return std::vector<color<T, Channels>>& 
     */
    std::vector<color<T, Channels>> const & data() const {
        return m_data;
    }

    /**
     * \brief Get teh width of the image
     * 
     * \return size_t 
     */
    size_t width() const {
        return m_width;
    }

    /**
     * \brief Get the height of the image
     * 
     * \return size_t 
     */
    size_t height() const {
        return m_height;
    }

    /**
     * \brief Get the number of channels in the image
     * 
     * \return constexpr size_t 
     */
    constexpr size_t channels() const {
        return Channels;
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
     * \brief Get a reference to the pixel at the specified coordinates
     * 
     * \param index 
     * \return color<T, Channels>& 
     */
    color<T, Channels> & operator[] (size_t index) {
        return m_data[index];
    }

    /**
     * \brief Get a constant reference to the pixel at the specified coordinates
     * 
     * \param index 
     * \return color<T, Channels>& 
     */
    color<T, Channels> const & operator[] (size_t index) const {
        return m_data[index];
    }

    /**
     * \brief Get pixel value at specified coordinates
     * 
     * \param x The x-coordinate
     * \param y The y-coordinate
     * \return color<T, Channels>& A reference to the image's color at the given
     * coordinates
     */
    color<T, Channels> & operator() (int x, int y) {
        return m_data[y * m_width + x];
    }

    /**
     * \brief Get pixel value at specified coordinates
     * 
     * \param x The x-coordinate
     * \param y The y-coordinate
     * \return color<T, Channels>& A constant reference to the image's color at
     * the given coordinates
     */
    color<T, Channels> const & operator() (int x, int y) const {
        return m_data[y * m_width + x];
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

        for (size_t i = 0; i < rhs.data().size(); ++i) {
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

        for (size_t i = 0; i < rhs.data().size(); ++i) {
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

/**
 * Loads an image from disk and returns a representation of the indicated
 * data type. All conversions are handled internally by OIIO.
 * If the image could not be read, an empty image is returned (size 0x0).
 * 
 * \todo Transform from file format color model to internal color model
 *
 * \param filename The path on disk relative to the current working
 * directory
 * \param config file format specific parameters in the form of an ImageSpec
 * configuration that will be handed through to OIIO
 * \returns An image object representing the file contents
 */
template<typename T, size_t Channels>
[[nodiscard]] image<T, Channels> load_image(char const * filename,
    const OIIO::ImageSpec * config = nullptr)
{
    auto file = OIIO::ImageInput::open(filename, config);
    // TODO more expressive error handling
    if (!file)
        return image<T, Channels>();

    const OIIO::ImageSpec & spec = file->spec();
    auto channels = spec.channelnames;

    // bool needs_alpha = spec.alpha_channel == -1 && ensure_alpha;

    // if (needs_alpha)
    //     channels.push_back("A");

    std::vector<T> img_data(spec.width * spec.height * spec.nchannels);
    file->read_image(helpers::type_to_typedesc<T>(), &img_data[0]);

    image<T, Channels> result(
        img_data.data(),
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
// template<typename T>
// bool write_image(
//     char const * filename,
//     image<T> const & data,
//     OIIO::TypeDesc const & format = helpers::type_to_typedesc<T>())
// {
//     std::unique_ptr<OIIO::ImageOutput> out =
//         OIIO::ImageOutput::create(filename);
//     if (!out)
//         return false;

//     OIIO::ImageSpec spec(
//         data.width(),
//         data.height(),
//         data.channels(),
//         format);
//     out->open(filename, spec);
//     out->write_image(helpers::type_to_typedesc<T>(),
//         transpose(data).data().data()); // transposing image before writing
//     out->close();

//     return true;
// }

}

#endif // SPICE_IMAGE
