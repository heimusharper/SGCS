#ifndef OPTIONAL_H
#define OPTIONAL_H
#include <list>
#include <mutex>

namespace tools
{
template <typename T>
class optional
{
public:
    class handler
    {
    public:
        virtual bool handleChanged() = 0;
    };

    optional(T def) : m_dirty(false), m_value(def)
    {
    }

    void set(T &&val)
    {
        m_dirty = true;
        m_value = val;
        for (auto h : m_handle)
            h->handleChanged();
    }
    T get() const
    {
        return m_value;
    }

    bool dirty() const
    {
        return m_dirty;
    }

    optional<T> &operator=(T &&other) noexcept
    {
        set(std::move(other));
        return *this;
    }

    bool operator==(const T &t) const
    {
        return get() == t;
    }
    bool operator==(const optional<T> &t) const
    {
        return get() == t.get();
    }

    void addHandler(optional::handler *handler, bool emitOnSet)
    {
        m_handle.push_back(handler);
        if (emitOnSet && dirty())
            handler->handleChanged();
    }
    void removeHandler(optional::handler *handler)
    {
        m_handle.remove(handler);
    }

private:
    bool m_dirty = false;
    T m_value;
    std::list<optional::handler *> m_handle;
};

template <typename T>
class optional_safe
{
public:
    class handler
    {
    public:
        virtual bool handleChanged() = 0;
    };

    optional_safe(T def) : m_dirty(false), m_value(def)
    {
    }

    T get() const
    {
        return m_value;
    }

    void set(T &&val)
    {
        m_mutex.lock();
        m_dirty = true;
        m_value = val;
        m_mutex.unlock();
        for (auto h : m_handle)
            h->handleChanged();
    }

    bool dirty() const
    {
        return m_dirty;
    }

    void reset()
    {
        m_mutex.lock();
        m_dirty = false;
        m_mutex.unlock();
    }

    optional_safe<T> &operator=(T &&other) noexcept
    {
        set(other);
        for (auto h : m_handle)
            h->handleChanged();
        return *this;
    }

    bool operator==(const T &t) const
    {
        return get() == t;
    }
    bool operator==(const optional_safe<T> &t) const
    {
        return get() == t.get();
    }

    void addHandler(optional_safe::handler *handler, bool emitOnSet)
    {
        m_mutex.lock();
        m_handle.push_back(handler);
        m_mutex.unlock();
        if (emitOnSet && dirty())
            handler->handleChanged();
    }

    void removeHandler(optional_safe::handler *handler)
    {
        m_mutex.lock();
        m_handle.remove(handler);
        m_mutex.unlock();
    }

private:
    bool m_dirty = false;
    T m_value;
    std::mutex m_mutex;
    std::list<optional_safe::handler *> m_handle;
};
}

#endif // OPTIONAL_H
