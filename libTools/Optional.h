#ifndef OPTIONAL_H
#define OPTIONAL_H
#include <mutex>

namespace tools
{
template <class T>
class optional
{
public:
    optional(T def) : m_dirty(false), m_value(def)
    {
    }

    T &get()
    {
        m_dirty = true;
        return m_value;
    }
    bool dirty()
    {
        return m_dirty;
    }

    optional<T> &operator=(const T &other) noexcept
    {
        get() = other;
        return *this;
    }

private:
    bool m_dirty = false;
    T m_value;
};

template <class T>
class optional_safe
{
public:
    optional_safe(T def) : m_dirty(false), m_value(def)
    {
    }

    T get()
    {
        return m_value;
    }

    void set(T val)
    {
        m_mutex.lock();
        m_dirty = true;
        m_value = val;
        m_mutex.unlock();
    }

    bool dirty()
    {
        return m_dirty;
    }
    void reset()
    {
        m_mutex.lock();
        m_dirty = false;
        m_mutex.unlock();
    }

    optional<T> &operator=(const T &other) noexcept
    {
        set(other);
        return *this;
    }

private:
    bool m_dirty = false;
    T m_value;
    std::mutex m_mutex;
};
}

#endif // OPTIONAL_H
