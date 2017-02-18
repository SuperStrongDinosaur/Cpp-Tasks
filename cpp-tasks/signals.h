//
//  signals.h
//  cpp
//
//  Created by Alex Shirvinsky on 18.02.17.
//  Copyright © 2017 Alex Shirvinsky. All rights reserved.
//

#ifndef signals_h
#define signals_h

#include <functional>
#include <list>
#include <memory>

template <typename R>
struct Signal;

template <typename...Params>
class Signal<void(Params...)> {
public:
    typedef std::function<void(Params...)> slot_t;
    
    Signal() : sz(0) {}
    
    class connection_item {
    public:
        connection_item() = default;
        
        connection_item(Signal* par, slot_t sl, bool cnct) : parent(par), slot(sl), connected(cnct) {}
        
        template<typename F, typename... Args>
        connection_item(F&& f, Args&&... args) : slot(std::bind(f, args...)) {}
        
        void disconnect() {
            connected = false;
        }
        
        bool is_connected() const {
            return connected;
        }
        
        void operator()(Params ...p) const {
            slot(p...);
        }
        
    private:
        Signal* parent;
        slot_t slot;
        bool connected;
    };
    
    typedef std::shared_ptr<connection_item> ptr_t;
    struct connection {
        connection(ptr_t ptr) : pointer(ptr) {}
        
        template<typename F, typename... Args>
        connection(F&& f, Args&&... args) : pointer(std::make_shared<connection_item>(std::bind(f, args...))) {}

        
        void disconnect() {
            pointer->disconnect();
        }
        
    private:
        ptr_t pointer;
    };
    
    connection connect(slot_t slot) {
        ptr_t ptr = std::make_shared<connection_item>(this, slot, true);
        if (sz == 1) {
            slots = std::make_shared<std::list<ptr_t>>();
            (*slots).emplace_back(small_slot);
            (*slots).push_back(ptr);
        } else if(sz == 0) {
            small_slot = ptr;
        } else {
            (*slots).push_back(ptr);
        }
        sz++;
        return connection(ptr);
    }
    
    void disconnect_all() {
        if(sz > 1) {
            for (auto it = (*slots).begin(); it != (*slots).end(); it++) {
                (*it)->disconnect();
            }
        }
        else
            small_slot->disconnect();
    }
    
    void operator()(Params...p) {
        if (sz < 2) {
            if (sz == 1) {
                if ((*small_slot).is_connected()) {
                    (*small_slot)(p...);
                } else {
                    sz--;
                }
            }
        } else {
            for (auto it = (*slots).begin(); it != (*slots).end(); it++) {
                if ((*it) -> is_connected()) {
                    (*(*it))(p...);
                }
                else {
                    it = (*slots).erase(it);
                    if(it != (*slots).begin()) it--;
                    sz--;
                }
            }
            if (sz == 1) {
                small_slot = (*slots).front();
                slots.reset();
            } else if(sz == 0) {
                slots.reset();
            }
        }
    }
 
private:
    size_t sz;
    ptr_t small_slot;
    std::shared_ptr<std::list<ptr_t>> slots;
    
};

#endif /* signals_soo_hpp */
