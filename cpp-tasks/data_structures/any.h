//
//  any.h
//  cpp
//
//  Created by Alex Shirvinsky on 29.01.17.
//  Copyright © 2017 Alex Shirvinsky. All rights reserved.
//

#ifndef any_h
#define any_h

#include <memory>
#include <typeinfo>

class any {
public:
    any(): data() {}
    
    template<typename T>
    any(const T& val) :
    data(new holder<typename std::remove_cv<typename std::decay<const T>::type>::type>(val)){}
    
    any(const any& other) : data(other.data ? other.data->clone() : 0) {}
    
    any(any&& other) : data(std::move(other.data)) {}
    
    ~any() {}
    
    any& swap(any& other) {
        std::swap(data, other.data);
        return *this;
    }
    
    any& operator=(any&& other) {
        other.swap(*this);
        return *this;
    }
   
    template <class T>
    any& operator=(T&& other) {
        any(std::move(other)).swap(*this);
        return *this;
    }
    
    bool empty() const {
        return !data;
    }
    
    void clear() {
        any().swap(*this);
    }
    
    const std::type_info& type() const {
        return data ? data->type() : typeid(void);
    }
    
    template<typename T> friend T* any_cast(any*);
    template<typename T> friend const T* any_cast(const any*);
    template<typename T> friend T any_cast(any& oper);
    template<typename T> friend T any_cast(const any& oper);
    template<typename T> friend T any_cast(any&& oper);
    
private:
    struct placeholder {
        virtual ~placeholder() {}
        virtual const std::type_info& type() const = 0;
        virtual placeholder* clone() const = 0;
    };
    
    template<typename T>
    class holder : public placeholder {
    public:
        holder(const T & val) : data(val) {}
        
        holder(T&& val) : data(std::move(val)) {}
        
        virtual const std::type_info& type() const {
            return typeid(T);
        }
        
        virtual placeholder* clone() const {
            return new holder(data);
        }

        T data;
    };
    
    std::unique_ptr<placeholder> data;
};

void swap(any& a, any& b) {
    a.swap(b);
}

template<typename T>
T* any_cast(any* oper) {
    return oper && oper->type() == typeid(T) ?
    &static_cast<any::holder<typename std::remove_cv<T>::type>*>(oper->data.get())->data : 0;
}

template<typename T>
const T* any_cast(const any* oper) {
    return any_cast<T>(const_cast<any*>(oper));
}

template<typename T>
T any_cast(any& oper) {
    typedef typename std::remove_reference<T>::type non;
    non* res = any_cast<non>(&oper);
    return *res;
}

template<typename T>
T any_cast(const any& oper) {
    typedef typename std::remove_reference<T>::type non;
    return any_cast<const non&>(const_cast<any&>(oper));
}

template<typename T>
T any_cast(any&& oper) {
    return any_cast<T>(oper);
}

#endif /* any_h */
