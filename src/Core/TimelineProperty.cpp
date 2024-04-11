#include "TimelineProperty.h"
#include "Vector2.h"

template<typename T>
TimelineProperty<T>::TimelineProperty(std::vector<std::pair<uint32_t, T>> &&values_) :
    m_isEmpty(false)
{
    m_values = std::move(values_);

    std::ranges::sort(m_values,
    [](const std::pair<uint32_t, T> &lhs, const std::pair<uint32_t, T> &rhs) {
        return lhs.first < rhs.first;
    });

    if (m_values.size() == 0 || m_values.front().first != 0)
    {
        m_values.insert(m_values.begin(), std::make_pair<uint32_t, T>(0, std::move(T())));
    }
}

template<typename T>
TimelineProperty<T>::TimelineProperty(const TimelineProperty<T> &rhs)
{
    m_values = rhs.m_values;
    m_isEmpty = rhs.m_isEmpty;
}

template <typename T>
TimelineProperty<T> &TimelineProperty<T>::operator=(const TimelineProperty<T> &rhs)
{
    m_values = rhs.m_values;
    m_isEmpty = rhs.m_isEmpty;

    return *this;
}

template<typename T>
TimelineProperty<T>::TimelineProperty(T &&value_)
{
    m_values.push_back(std::make_pair<uint32_t, T>(0, std::move(value_)));
    m_isEmpty = false;
}

template<typename T>
TimelineProperty<T>::TimelineProperty(const T &value_)
{
    m_values.push_back(std::make_pair(0, value_));
    m_isEmpty = false;
}

template<typename T>
TimelineProperty<T>::TimelineProperty(TimelineProperty<T> &&rhs)
{
    m_values = std::move(rhs.m_values);
    m_isEmpty = rhs.m_isEmpty;
    rhs.m_isEmpty = true;
}

template<typename T>
TimelineProperty<T>::TimelineProperty()
{
    m_values.insert(m_values.begin(), std::make_pair<uint32_t, T>(0, std::move(T())));
    m_isEmpty = true;
}

template<typename T>
TimelineProperty<T>& TimelineProperty<T>::operator=(TimelineProperty<T> &&rhs)
{
    m_values = std::move(rhs.m_values);
    m_isEmpty = rhs.m_isEmpty;
    rhs.m_isEmpty = true;
    return *this;
}

template<typename T>
void TimelineProperty<T>::addPropertyValue(uint32_t timeMark_, T &&value_)
{
    auto it = std::upper_bound(m_values.begin(), m_values.end(), timeMark_,
        [](uint32_t timeMark, const auto& pair) {
            return timeMark <= pair.first;
        });

    if (it != m_values.end())
    {
        if (it->first == timeMark_)
            it->second = std::move(value_);
        else
            m_values.insert(it, std::make_pair(timeMark_, std::move(value_)));
    }
    else
    {
        m_values.push_back(std::make_pair(timeMark_, std::move(value_)));
    }

    m_isEmpty = false;
}

template<typename T>
const T &TimelineProperty<T>::operator[](uint32_t timeMark_) const
{
    auto it = std::lower_bound(m_values.rbegin(), m_values.rend(), timeMark_,
        [](const auto& pair, uint32_t timeMark) {
            return timeMark < pair.first;
        });
    if (it != m_values.rend())
        return it->second;
    return m_values.front().second;
}

template <typename T>
int TimelineProperty<T>::getValuesCount() const
{
    return m_values.size();
}

template <typename T>
std::pair<uint32_t, T> &TimelineProperty<T>::getValuePair(int id_)
{
    return m_values[id_];
}

template <typename T>
void TimelineProperty<T>::setPairValue(int id_, T &&value_)
{
    m_values[id_].second = std::move(value_);
}

template <typename T>
bool TimelineProperty<T>::deletePair(int id_)
{
    if (m_values[id_].first == 0)
        return false;

    m_values.erase(m_values.begin() + id_);
    return true;
}

template<typename T>
bool TimelineProperty<T>::isEmpty() const
{
    return m_isEmpty;
}

template TimelineProperty<bool>;
template TimelineProperty<float>;
template TimelineProperty<int>;
template TimelineProperty<Vector2<float>>;
template TimelineProperty<Vector2<int>>;