//
//  optional.h
//  cpp
//
//  Created by Alex Shirvinsky on 28.01.17.
//  Copyright © 2017 Alex Shirvinsky. All rights reserved.
//

#ifndef optional_h
#define optional_h

#include <memory>
#include <utility>

struct nullopt_t {};
constexpr nullopt_t nullopt_{};

struct inplace_t {};
constexpr inplace_t inplace_{};

template<typename T>
class optional {
    typename std::aligned_storage<sizeof(T), alignof(T)>::type data;
    bool init;
    
public:
    
    optional() : init(false) {}
    
    optional(T const& val) : init(true) {
        new (&data) T(val);
    }
    
    optional(nullopt_t) : init(false) {}
    
    template<typename ... Args>
    optional(inplace_t, Args&& ... args) : init(true) {
        new (&data) T(std::forward<Args>(args)...);
    }
    
    optional(optional const & other) : init(other.init) {
        if(other)
            new (&data) T(*other);
    }
    
    optional(optional&& other) : init(false) {
        emplace(*other);
        init = other.init;
    }
    
    ~optional() {
        reset();
    }
    
    optional& operator=(optional other) {
        if(init && other.init) {
            data = other.data;
        }
        else if(!init && other.init) {
            emplace(std::move(*other));
        }
        else if(init && !other.init) {
            reset();
        }
        return *this;
    }
    
    T operator*() {
        return *reinterpret_cast<T*>(&data);
    }
    
    T* operator->() {
        return reinterpret_cast<T*>(&data);
    }
    
    const T operator*() const {
        return *reinterpret_cast<const T*>(&data);
    }
    
    const T* operator->() const {
        return reinterpret_cast<const T*>(&data);
    }
    
    explicit operator bool() const {
        return init;
    }
    
    T value_or(T&& default_value) {
        if(init)
            return *(*this);
        return std::move(default_value);
    }
    
    void swap(optional& other) {
        if(init && other.init) {
            std::swap(data, other.data);
        }
        else if(!init && other.init) {
            new (&data) T (*other);
            init = true;
            (*reinterpret_cast<T*>(&other.data)).~T();
            other.init = false;
        }
        else if(init && !other.init) {
            new (&other.data) T(**this);
            init = false;
            (*reinterpret_cast<T*>(&data)).~T();
            other.init = true;
        }
    }
    
    inline void reset() {
        if(init)
            (*reinterpret_cast<T*>(&data)).~T();
        init = false;
    }
    
    template<typename ... Args>
    void emplace(Args&& ... args) {
        reset();
        new (&data) T(std::forward<Args>(args)...);
        init = true;
    }
    
    friend bool operator==(optional const& a, optional const& b) {
        if(a.init != b.init)
            return false;
        if(!a.init)
            return true;
        return *a == *b;
    }
    
    friend bool operator!=(optional const& a, optional const& b) {
        return !(a == b);
    }
    
    friend bool operator<(optional const& a, optional const& b) {
        if(a) {
            if(b)
                return *a < *b;
            else
                return false;
        } else if(b)
            return true;
        return false;
    }
    
    friend bool operator>(optional const& a, optional const& b) {
        return b < a;
    }
    
    friend bool operator<=(optional const& a, optional const& b) {
        return a < b || a == b;
    }
    
    friend bool operator>=(optional const& a, optional const& b) {
        return a > b || a == b;
    }
};

template <typename T, typename ... Args>
optional<T> make_optional(Args&& ...args) {
    return optional<T>(inplace_, std::forward<Args>(args)...);
}

#endif /* optional_h */
