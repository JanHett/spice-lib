#ifndef SPICE_COLOR
#define SPICE_COLOR

#include <array>
#include <type_traits>
#include <limits>

#include "color_view.hpp"

namespace spice {

/**
 * Represents a color.
 */
template<typename T = float, size_t Channels = 4>
class color {
private:
    std::array<T, Channels> m_data;
public:
    ////////////////////////////////////////////////////////////////////////////
    // MEMBER TYPES
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Alias for the type used to represent individual pixel values
     */
    using value_type = T;

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

    /**
     * \brief Create a copy of the given color_view object
     * 
     * \param other The color_view to copy values from
     */
    template<typename T_other>
    color (color_view<T_other> const & other) {
        for (size_t i = 0; i < std::min(other.channels(), Channels); ++i) {
            m_data[i] = other[i];
        }
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
    // DATA GETTERS
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Returns the raw data pointer
     * 
     * \return size_t 
     */
    constexpr T * const data() {
        return m_data.data();
    }

    /**
     * \brief Returns the raw data pointer
     * 
     * \return size_t 
     */
    constexpr T const * const data() const {
        return m_data.data();
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
    template<typename T_other,
        std::enable_if_t<!std::is_same<T_other, color_view<T>>::value, int> = 0>
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
    template<typename T_other,
        std::enable_if_t<!std::is_same<T_other, color_view<T>>::value, int> = 0>
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
    template<typename T_other,
        std::enable_if_t<!std::is_same<T_other, color_view<T>>::value, int> = 0>
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
    template<typename T_other,
        std::enable_if_t<!std::is_same<T_other, color_view<T>>::value, int> = 0>
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
    template<typename T_other,
        std::enable_if_t<!std::is_same<T_other, color_view<T>>::value, int> = 0>
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
    template<typename T_other,
        std::enable_if_t<!std::is_same<T_other, color_view<T>>::value, int> = 0>
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
    template<typename T_other,
        std::enable_if_t<!std::is_same<T_other, color_view<T>>::value, int> = 0>
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
    template<typename T_other,
        std::enable_if_t<!std::is_same<T_other, color_view<T>>::value, int> = 0>
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

}

#endif // SPICE_COLOR