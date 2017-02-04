//
//  function.h
//  cpp
//
//  Created by Alex Shirvinsky on 04.02.17.
//  Copyright © 2017 Alex Shirvinsky. All rights reserved.
//

#ifndef function_h
#define function_h

#include <functional>

template <typename Sig>
class function;

template <typename R, typename ... Params>
class function<R (Params ...)> {
public:
    
    typedef R signature_type (Params ...);
    
    function() : cont() {}
    
    template <typename T>
    function(T f) : cont(new free_holder<T>(f)) {}
    
    template <typename T, typename ClassType>
    function(T ClassType::* f) : cont(new member_holder<T, Params ...>(f)) {}
    
    function(const function & other) : cont(other.cont->clone()) {}
    
    function(function&& f) {
        std::swap(f.cont, cont);
    }
    
    function & operator=(const function & other) {
        cont = other.cont->clone();
        return *this;
    }
    
    function & operator=(function&& other) {
        other.swap(*this);
        return *this;
    }
    
    R operator()(Params ... args) {
        return cont->caller(args ...);
    }
    
    void swap(function& other) {
        std::swap(other.cont, cont);
    }
    
    explicit operator bool() const {
        return cont;
    }
    
private:
    
    class holder_base {
    public:
        holder_base() {}
        virtual ~holder_base() {}
        virtual R caller(Params ... args) = 0;
        virtual std::shared_ptr<holder_base> clone() = 0;
    private:
        holder_base(const holder_base & );
        void operator=(const holder_base &);
    };
    
    typedef std::shared_ptr<holder_base> callerr_t;
    
    
    template <typename T>
    class free_holder : public holder_base {
    public:
        
        free_holder(T func)  {
            if(sizeof(func) < 1024) {
                new (&small_mFunc) T(func);
                is_small = true;
            }
            else {
                mFunc = std::make_shared<T>(func);
                is_small = false;
            }
        }
        
        virtual R caller(Params ... args) {
            if(!is_small)
                return (*mFunc)(args ...);
            return (get(&small_mFunc))(args ...);
        }
        
        inline T get(void *data) {
            return *static_cast<T *>(data);
        }
        
        virtual callerr_t clone() {
            if(!is_small)
                return callerr_t(new free_holder(*mFunc));
            return callerr_t(new free_holder(get(&small_mFunc)));
        }
        
    private:
        bool is_small;
        typename std::aligned_storage<1024, 8>::type small_mFunc;
        std::shared_ptr<T> mFunc;
    };
    
    template <typename T, typename ClassType, typename ... RestParams>
    class member_holder : public holder_base {
    public:
        typedef typename std::remove_reference<ClassType>::type non;
        typedef T non::* member_signature_t;

        member_holder(member_signature_t f) {
            if(sizeof(f) < 1024) {
                new (&small_mFunc) member_signature_t(f);
                is_small = true;
            }
            else {
                mFunc = std::make_shared<member_signature_t>(f);
                is_small = false;
            }
        }
        
        virtual R caller(ClassType obj, RestParams ... restArgs) {
            if(!is_small)
                return (obj.*(*mFunc))(restArgs ...);
            return (obj.*(get(&small_mFunc)))(restArgs ...);
        }
        
        inline member_signature_t get(void *data) {
            return *static_cast<member_signature_t *>(data);
        }
        
        virtual callerr_t clone() {
            if(!is_small)
                return callerr_t(new member_holder(*mFunc));
            return callerr_t(new member_holder(get(&small_mFunc)));
        }
        
    private:
        bool is_small;
        typename std::aligned_storage<1024, 8>::type small_mFunc;
        std::shared_ptr<member_signature_t> mFunc;
    };
    
    callerr_t cont;
};

#endif /* function_h */
