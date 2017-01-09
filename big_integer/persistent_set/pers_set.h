#ifndef pers_set_h
#define pers_set_h

#include <iostream>
#include <vector>
#include <memory>

using namespace std;

struct persistent_set {
	typedef int value_type;

	struct node : enable_shared_from_this<node> {
		node(value_type);
		node(node const&);
		node& operator=(const node&);
		shared_ptr<node> find_node(value_type);
		shared_ptr<node> insert_node(value_type);
		shared_ptr<node> next_node(shared_ptr<node>);
		shared_ptr<node> prev_node(shared_ptr<node>);
		void erase_node(shared_ptr<node>, shared_ptr<node>);

	public:
		value_type data;
		shared_ptr<node> l;
		shared_ptr<node> r;
	};

	struct iterator {
		friend struct persistent_set;
		iterator(shared_ptr<node>, shared_ptr<node>);
		iterator(iterator const&);
		iterator& operator=(iterator const&);

		value_type const& operator*() const;
		iterator& operator++();
		iterator operator++(int);
		iterator& operator--();
		iterator operator--(int);

		friend bool operator==(iterator, iterator);
		friend bool operator!=(iterator, iterator);
	private:
		shared_ptr<node> cur, root;
	};

	persistent_set();                                    
	persistent_set(persistent_set const&);                                                 
	persistent_set& operator=(persistent_set const&);    

	iterator find(value_type);                         
	pair<iterator, bool> insert(value_type);       
	void erase(iterator);                                  

	iterator begin() const;                               
	iterator end() const;                                
private:
	shared_ptr<node> root;
};

#endif 
