#ifndef SPICE_COLOR_VIEW
#define SPICE_COLOR_VIEW

#include <type_traits>

/**
 * \brief Wrapper around color data of arbitrary length with arbitrary stride
 * between channel samples
 * 
 * \tparam T 
 */
template<typename T>
class color_view
{
private:
    T * m_data;
    size_t m_stride;
    size_t m_channels;
public:
    ////////////////////////////////////////////////////////////////////////////
    // MEMBER TYPES
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Alias for the type used to represent individual pixel values
     */
    using value_type = T;

    ////////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Construct a new, empty color_view object
     * 
     * Before the color_view can be used, it has to be assigned a reference
     * with `color_view<T>::reset`.
     */
    constexpr color_view ():
        m_data(nullptr),
        m_stride(0),
        m_channels(0)
    {}

    /**
     * \brief Create a copy of the given color_view object
     * 
     * \param other The color_view to copy
     */
    constexpr color_view (color_view const & other) = default;

    /**
     * \brief Move the the given color_view object
     * 
     * \param other The color_view to copy
     */
    constexpr color_view (color_view && other) = default;

    /**
     * \brief Construct a new color_view object referring to the given data
     * 
     * \param data 
     * \param stride The distance between the channels
     * \param channels The number of channels this color_view refers to
     */
    constexpr color_view (T * const data, size_t stride, size_t channels):
        m_data(data),
        m_stride(stride),
        m_channels(channels)
    {}

    ////////////////////////////////////////////////////////////////////////////
    // ASSIGNMENT OPERATORS
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Copy-assigns another color_view
     * 
     * \param other 
     * \return color_view& 
     */
    color_view& operator=(color_view const & other) noexcept = default;

    /**
     * \brief Move-assigns another color_view
     * 
     * \param other 
     * \return color_view& 
     */
    color_view& operator=(color_view && other) noexcept = default;

    /**
     * \brief Assign each channel the provided value
     * 
     * \param value 
     * \return color_view& 
     */
    color_view& operator=(T const & value) noexcept {
        for (size_t channel = 0; channel < m_channels; ++channel) {
            *(m_data + channel * m_stride) = value;
        }
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////
    // RESET OPERATIONS
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Replaces the reference with a nullptr reference and resets the
     * stride and size to 0
     */
    void reset() {
        m_data = nullptr;
        m_stride = 0;
        m_channels = 0;
    }

    /**
     * \brief Replaces the reference with the reference held by `other` and
     * adjusts the size accordingly
     * 
     * \param other 
     */
    void reset(color_view const & other) {
        m_data = other.data;
        m_stride = other.stride;
        m_channels = other.channels;
    }

    /**
     * \brief Replaces the reference to refer to `data` and updates the stride
     * and channel count
     * 
     * \param other 
     */
    void reset(T * const data, size_t stride, size_t channels) {
        m_data = data;
        m_stride = stride;
        m_channels = channels;
    }

    ////////////////////////////////////////////////////////////////////////////
    // GETTERS
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Returns the channel count
     * 
     * \return size_t 
     */
    constexpr size_t size() const {
        return m_channels;
    }

    /**
     * \brief Returns the channel count
     * 
     * \return size_t 
     */
    constexpr size_t channels() const {
        return m_channels;
    }

    /**
     * \brief Returns the stride
     * 
     * \return size_t 
     */
    constexpr size_t stride() const {
        return m_stride;
    }

    /**
     * \brief Returns the data pointer
     * 
     * \return size_t 
     */
    constexpr T * const data() {
        return m_data;
    }

    /**
     * \brief Returns the data pointer
     * 
     * \return size_t 
     */
    constexpr T const * const data() const {
        return m_data;
    }

    ////////////////////////////////////////////////////////////////////////////
    // COMPARISON OPERATORS
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Compares a color_view component-wise with a scalar
     * 
     * \tparam T_other A scalar that is comparable to the color_view's data type `T`
     * \param lhs 
     * \param rhs 
     * \return true if all elements of the color_view correspond to the scalar value,
     * false otherwise
     * 
     * \todo refactor this to work for any type that is convertible to `T`
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<std::is_scalar<T_other>::value, bool>
    operator==(T_other const & lhs,
        color_view const & rhs)
    {
        for (size_t i = 0; i < rhs.size(); ++i) {
            if (lhs != rhs[i])
                return false;
        }

        return true;
    }

    /**
     * \brief Compares a color_view component-wise with a scalar
     * 
     * \tparam T_other A scalar that is comparable to the color_view's data type `T`
     * \param lhs 
     * \param rhs 
     * \return true if all elements of the color_view correspond to the scalar value,
     * false otherwise
     * 
     * \todo refactor this to work for any type that is convertible to `T`
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<std::is_scalar<T_other>::value, bool>
    operator==(color_view const & lhs,
        T_other const & rhs)
    {
        return rhs == lhs;
    }

    /**
     * \brief Compares a color_view component-wise with a scalar for inequality
     * 
     * \tparam T_other A scalar that is comparable to the color_view's data type `T`
     * \param lhs 
     * \param rhs 
     * \return true if any of the elements of the color_view do not correspond to the
     * scalar value, false otherwise
     * 
     * \todo refactor this to work for any type that is convertible to `T`
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<std::is_scalar<T_other>::value, bool>
    operator!=(T_other const & lhs,
        color_view const & rhs)
    {
        return !(lhs == rhs);
    }

    /**
     * \brief Compares a color_view component-wise with a scalar for inequality
     * 
     * \tparam T_other A scalar that is comparable to the color_view's data type `T`
     * \param lhs 
     * \param rhs 
     * \return true if any of the elements of the color_view do not correspond to the
     * scalar value, false otherwise
     * 
     * \todo refactor this to work for any type that is convertible to `T`
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<std::is_scalar<T_other>::value, bool>
    operator!=(color_view const & lhs,
        T_other const & rhs)
    {
        return !(rhs == lhs);
    }

    /**
     * \brief Compares two color_views for equality
     * 
     * \param lhs 
     * \param rhs 
     * \return true if the two have the same number of elements with
     * corresponding values, false otherwise
     */
    [[nodiscard]] friend bool operator==(color_view const & lhs, color_view const & rhs)
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
     * \brief Compares two color_views for inequality
     * 
     * \param lhs 
     * \param rhs 
     * \return true if the two do not have the same number of elements or any of
     * the values do not correspond, false otherwise
     */
    [[nodiscard]] friend bool operator!=(color_view const & lhs, color_view const & rhs)
    {
        return !(lhs == rhs);
    }

    /**
     * \brief Compares two color_views
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
        !std::is_same<color_view, T_other>::value && !std::is_scalar<T_other>::value,
        bool
    >
    operator==(T_other const & lhs,
        color_view const & rhs) {
        if (lhs.size() != rhs.size())
            return false;

        for (size_t i = 0; i < rhs.size(); ++i) {
            if (lhs[i] != rhs[i])
                return false;
        }

        return true;
    }

    /**
     * \brief Compares two color_views for inequality
     * 
     * \param lhs 
     * \param rhs 
     * \return true if the two do not have the same number of elements or any of
     * the values do not correspond, false otherwise
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<
        !std::is_same<color_view, T_other>::value && !std::is_scalar<T_other>::value,
        bool
    >
    operator!=(color_view const & lhs, T_other const & rhs)
    {
        return !(lhs == rhs);
    }

    /**
     * \brief Compares two color_views
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
        !std::is_same<color_view, T_other>::value && !std::is_scalar<T_other>::value,
        bool
    >
    operator==(color_view const & lhs,
        T_other const & rhs) {
        return rhs == lhs;
    }

    /**
     * \brief Compares two color_views for inequality
     * 
     * \param lhs 
     * \param rhs 
     * \return true if the two do not have the same number of elements or any of
     * the values do not correspond, false otherwise
     */
    template<typename T_other>
    [[nodiscard]] friend
    std::enable_if_t<
        !std::is_same<color_view, T_other>::value && !std::is_scalar<T_other>::value,
        bool
    >
    operator!=(T_other const & lhs, color_view const & rhs)
    {
        return !(rhs == lhs);
    }

    ////////////////////////////////////////////////////////////////////////////
    // CHANNEL ACCESS OPERATORS
    ////////////////////////////////////////////////////////////////////////////

    /**
     * \brief Retrieve a reference to an element of the color_view
     * 
     * \param index 
     * \return T& 
     */
    [[nodiscard]] T& operator[] (size_t index) {
        return m_data[index * m_stride];
    }

    /**
     * \brief Retrieve a constant reference to an element of the color_view
     * 
     * \param index 
     * \return T& 
     */
    [[nodiscard]] T const & operator[] (size_t index) const {
        return m_data[index * m_stride];
    }

    ////////////////////////////////////////////////////////////////////////////
    // ARITHMETIC OPERATORS
    ////////////////////////////////////////////////////////////////////////////

    // ~~~ ADDITION ~~~ //

    /**
     * \brief Add `rhs` to this color_view channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
    color_view & operator+=(color_view const & rhs)
    {
        for (size_t c = 0; c < m_channels; ++c) {
            m_data[c * m_stride] += rhs.m_data[c * rhs.m_stride];
        }
        return *this;
    }
    
    /**
     * \brief Add `rhs` to `lhs` channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
    [[nodiscard]] friend color_view operator+(color_view lhs, color_view const & rhs)
    {
        lhs += rhs;
        return lhs;
    }

    /**
     * \brief Add `rhs` to this color_view channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
    color_view & operator+=(T const & rhs)
    {
        for (size_t c = 0; c < m_channels; ++c) {
            m_data[c * m_stride] += rhs;
        }
        return *this;
    }

    /**
     * \brief Add `rhs` to `lhs` channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
    [[nodiscard]] friend color_view operator+(color_view lhs, T const & rhs)
    {
        lhs += rhs;
        return lhs;
    }

    // ~~~ SUBTRACTION ~~~ //

    /**
     * \brief Subtract `rhs` from this color_view channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
     color_view & operator-=(color_view const & rhs)
    {
        for (size_t c = 0; c < m_channels; ++c) {
            m_data[c * m_stride] -= rhs.m_data[c * rhs.m_stride];
        }
        return *this;
    }
    
    /**
     * \brief Subtract `rhs` from `lhs` channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
    [[nodiscard]] friend color_view operator-(color_view lhs, color_view const & rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    /**
     * \brief Subtract `rhs` from this color_view channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
     color_view & operator-=(T const & rhs)
    {
        for (size_t c = 0; c < m_channels; ++c) {
            m_data[c * m_stride] -= rhs;
        }
        return *this;
    }

    /**
     * \brief Subtract `rhs` from `lhs` channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
    [[nodiscard]] friend color_view operator-(color_view lhs, T const & rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    // ~~~ MULTIPLICATION ~~~ //

    /**
     * \brief Multiply this color_view with `rhs` channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
     color_view & operator*=(color_view const & rhs)
    {
        for (size_t c = 0; c < m_channels; ++c) {
            m_data[c * m_stride] *= rhs.m_data[c * rhs.m_stride];
        }
        return *this;
    }
    
    /**
     * \brief Multiply `lhs` with `rhs` channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
    [[nodiscard]] friend color_view operator*(color_view lhs, color_view const & rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    /**
     * \brief Multiply this color_view with `rhs` channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
     color_view & operator*=(T const & rhs)
    {
        for (size_t c = 0; c < m_channels; ++c) {
            m_data[c * m_stride] *= rhs;
        }
        return *this;
    }

    /**
     * \brief Multiply `lhs` with `rhs` channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
    [[nodiscard]] friend color_view operator*(color_view lhs, T const & rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    // ~~~ DIVISION ~~~ //

    /**
     * \brief Divide this color_view by `rhs` channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
     color_view & operator/=(color_view const & rhs)
    {
        for (size_t c = 0; c < m_channels; ++c) {
            m_data[c * m_stride] /= rhs.m_data[c * rhs.m_stride];
        }
        return *this;
    }
    
    /**
     * \brief Divide `lhs` by `rhs` channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
    [[nodiscard]] friend color_view operator/(color_view lhs, color_view const & rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    /**
     * \brief Divide this color_view by `rhs` channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
     color_view & operator/=(T const & rhs)
    {
        for (size_t c = 0; c < m_channels; ++c) {
            m_data[c * m_stride] /= rhs;
        }
        return *this;
    }

    /**
     * \brief Divide `lhs` by `rhs` channel-wise
     * 
     * \param rhs 
     * \return color_view& 
     */
    [[nodiscard]] friend color_view operator/(color_view lhs, T const & rhs)
    {
        lhs /= rhs;
        return lhs;
    }
};

/**
 * Specialized formatter for `spice::color_view`.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, color_view<T> const & pxl)
{
    os << "color_view(";
    for (size_t idx = 0; idx < pxl.size() - 1; ++idx)
        os << pxl[idx] << ", ";
    os << pxl[pxl.size() - 1];
    os << ")";
    return os;
}

#endif // SPICE_COLOR_VIEW