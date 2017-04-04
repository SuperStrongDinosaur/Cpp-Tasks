//
//  function.h
//  cpp
//
//  Created by Alex Shirvinsky on 04.02.17.
//  Copyright © 2017 Alex Shirvinsky. All rights reserved.
//

#ifndef function_h
#define function_h

#include <type_traits>
#include <functional>

template <typename Sig>
class function;

template <typename R, typename ... Params>
class function<R (Params ...)> {
public:
    
    typedef R signature_type (Params ...);
    
    function() : is_small(true) {}
    
    template <typename T>
    function(T f) {
        if(sizeof(f) < 1024 && std::is_nothrow_move_constructible<T>::value) {
            new (&small_cont) free_holder<T>(std::move(f));
            is_small = true;
        }
        else {
            cont = std::make_unique<free_holder<T>>(std::move(f));
            is_small = false;
        }
    }
    
    template <typename T, typename ClassType>
    function(T ClassType::* f) {
        if(sizeof(f) < 1024) {
            new (&small_cont) member_holder<T, Params ...>(f);
            is_small = true;
        }
        else {
            cont = std::make_unique<member_holder<T, Params ...>>(f);
            is_small = false;
        }
    }
    
    function(const function & other) : is_small(other.is_small) {
        if(is_small)
            small_cont = other.small_cont;
        else
            cont = other.cont->clone();
    }
    
    function(function&& f) {
        if(!is_small)
            std::swap(f.cont, cont);
        else {
           // std::swap(small_cont, f.small_cont);
            auto a = get(&small_cont)->get();
            auto b = get(&f.small_cont)->get();
            auto c = std::move(a);
            a = std::move(b);
            b = std::move(c);
            small_cont = *static_cast<typename std::aligned_storage<1024, 8>::type*>(std::move(b));
            f.small_cont = *static_cast<typename std::aligned_storage<1024, 8>::type*>(std::move(c));
        }
        std::swap(f.is_small, is_small);
    }
    
    function & operator=(const function & other) {
        if(!is_small)
            cont = other.cont->clone();
        else
            small_cont = other.cmall_cont;
        is_small = other.small_cont;
        return *this;
    }
    
    function & operator=(function&& other) {
        other.swap(*this);
        return *this;
    }
    
    R operator()(Params ... args) {
        if(!is_small)
            return cont->caller(args ...);
        return get(&small_cont)->caller(args ...);
    }
    
    void swap(function& f) {
        if(!is_small)
            std::swap(f.cont, cont);
        else {
            auto a = get(&small_cont)->get();
            auto b = get(&f.small_cont)->get();
            auto c = std::move(a);
            small_cont = *static_cast<typename std::aligned_storage<1024, 8>::type*>(std::move(b));
            f.small_cont = *static_cast<typename std::aligned_storage<1024, 8>::type*>(std::move(c));
        }
        std::swap(f.is_small, is_small);
    }
    
    explicit operator bool() const {
        return cont;
    }
    
private:

    class holder_base {
    public:
        holder_base() {}
        virtual ~holder_base() {}
        virtual R caller(Params& ... args) = 0;
        virtual std::unique_ptr<holder_base> clone() = 0;
        holder_base(const holder_base & );
        void operator=(const holder_base &);
        virtual void* get() = 0;
    };
    
    inline holder_base* get(void *data) {
        return static_cast<holder_base *>(data);
    }
    
    typedef std::unique_ptr<holder_base> callerr_t;
 
    template <typename T>
    class free_holder : public holder_base {
    public:
        free_holder(T func) : holder_base(), mFunction(std::move(func)) {}
        
        virtual R caller(Params&... args) {
            return mFunction(args ...);
        }
    
        virtual callerr_t clone() {
            return callerr_t(new free_holder(mFunction));
        }
        
        /*virtual void swap(holder_base& other) {
            std::swap(mFunction, other.mFunction);
        }*/
        
        virtual void* get() {
            return static_cast<void*>(&mFunction);
        }
        
    private:
        T mFunction;
    };
    
    
    template <typename T, typename ClassType, typename ... RestParams>
    class member_holder : public holder_base {
    public:
        
        typedef typename std::remove_reference<ClassType>::type non;
        typedef T non::* member_signature_t;
        
        member_holder(member_signature_t f) : mFunction(f) {}
        
        virtual R caller(ClassType& obj, RestParams& ... restArgs) {
            return (obj.*mFunction)(restArgs ...);
        }
        
        virtual callerr_t clone() {
            return callerr_t(new member_holder(mFunction));
        }
        
        /*virtual void swap(holder_base& other) {
            std::swap(mFunction, other.mFunction);
        }*/
        
        virtual void* get() {
            return static_cast<void*>(&mFunction);
        }
        
    private:
        member_signature_t mFunction;
    };
    
    bool is_small;
    typename std::aligned_storage<1024, 8>::type small_cont;
    callerr_t cont;
};

#endif /* function_h */
