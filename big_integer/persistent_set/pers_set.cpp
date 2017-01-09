#include "pers_set.h"
#include <limits>

persistent_set::persistent_set() : root(make_shared<node>(numeric_limits<value_type>::max())) {}

persistent_set::persistent_set(persistent_set const& other) : root(other.root) {}

persistent_set& persistent_set::operator=(persistent_set const& other) {
	root = make_shared<node>(*other.root);
	return *this;
}

persistent_set::iterator::iterator(shared_ptr<node>other, shared_ptr<node>parent) : cur(other), root(parent) {}

persistent_set::iterator::iterator(iterator const &other) : cur(other.cur), root(other.root){}

persistent_set::iterator& persistent_set::iterator::operator=(iterator const& other) {
	cur = other.cur;
	root = other.root;
	return *this;
}

persistent_set::node::node(value_type other) : data(other), l(nullptr), r(nullptr) {}

persistent_set::node::node(node const &other) : data(other.data), l(other.l), r(other.r) {}

persistent_set::node& persistent_set::node::operator=(const node& other) {
	data = other.data;
	l = other.l;
	r = other.r;
	return *this;
}

shared_ptr<persistent_set::node> persistent_set::node::find_node(value_type other) {
	if (other > data) {
		if (r == nullptr) 
			return nullptr;
		else 
			return r->find_node(other);
	}
	if (other < data) {
		if (l == nullptr) 
			return nullptr;
		else 
			return l->find_node(other);
	}
	return shared_from_this();
}

shared_ptr<persistent_set::node> persistent_set::node::insert_node(value_type other) {
	if (other > data) {
		if (r == nullptr) {
			r = make_shared<node>(other);
			return r;
		}
		else {
			r = make_shared<node>(*r);
			return r->insert_node(other);
		}
	}
	else if (other < data) {
		if (l == nullptr) {
			l = make_shared<node>(other);
			return l;
		}
		else {
			l = make_shared<node>(*l);
			return l->insert_node(other);
		}
	}
	return shared_from_this();
}

shared_ptr<persistent_set::node> persistent_set::node::next_node(shared_ptr<node> other) {
	if (data < other->data) {
		if (r == nullptr) 
			return shared_from_this();
		return r->next_node(other);
	}
	else {
		shared_ptr<node> ans;
		if (l == nullptr) 
			ans = shared_from_this();
		else 
			ans = l->next_node(other);
		if (ans->data <= other->data)
			ans = shared_from_this();
		return ans;
	}
}

shared_ptr<persistent_set::node> persistent_set::node::prev_node(shared_ptr<node> other) {
	if (data > other->data) {
		if (l == nullptr) 
			return shared_from_this();
		return l->prev_node(other);
	}
	else {
		shared_ptr<node> ans;
		if (r == nullptr) 
			ans = shared_from_this();
		else 
			ans = r->prev_node(other);
		if (ans->data >= other->data)
			ans = shared_from_this();
		return ans;
	}
}

void persistent_set::node::erase_node(shared_ptr<node> other, shared_ptr<node> parent) {
	if (data == other->data) {
		if (l != nullptr) {
			l = make_shared<node>(*l);
			shared_ptr<node> it = l;
			while (it->r != nullptr) {
				it->r = make_shared<node>(*it->r);
				it = it->r;
			}
			it->r = r;
			r = nullptr;
		}
		if (parent->r != shared_from_this()) {
			if (l == nullptr) 
				parent->l = r;
			else 
				parent->l = l;
		}
		else {
			if (r == nullptr) 
				parent->r = l;
			else 
				parent->r = r;
		}
	}
	else if (other->data < data) {
		l = make_shared<node>(*l);
		l->erase_node(other, shared_from_this());
	}
	else {
		r = make_shared<node>(*r);
		r->erase_node(other, shared_from_this());
	}
}

persistent_set::value_type const& persistent_set::iterator::operator*() const {
	return cur->data;
}

persistent_set::iterator& persistent_set::iterator::operator++() {
	if(cur->r == nullptr)
		cur = root->next_node(cur);
	else {
		cur = cur->r;
		while (cur->l != nullptr) {
			shared_ptr<node> it = cur->l;
			cur = it;
		}
	}		
	return *this;
}

persistent_set::iterator& persistent_set::iterator::operator--() {
	if(cur->l == nullptr)
		cur = root->prev_node(cur);
	else {
		cur = cur->l;
		while (cur->r != nullptr) {
			shared_ptr<node> it = cur->r;
			cur = it;
		}
	}	
	return *this;
}

persistent_set::iterator persistent_set::iterator::operator++(int) {
	iterator cur = *this;
	++*this;
	return cur;
}

persistent_set::iterator persistent_set::iterator::operator--(int) {
	iterator cur = *this;
	--*this;
	return cur;
}

bool operator==(persistent_set::iterator a, persistent_set::iterator b) {
	return a.cur->data == b.cur->data;
}

bool operator!=(persistent_set::iterator a, persistent_set::iterator b) {
	return a.cur->data != b.cur->data;
}

persistent_set::iterator persistent_set::begin() const {
	shared_ptr<node> it = root;
	while (it->l != nullptr) 
		it = it->l;
	return iterator(it, root);
}

persistent_set::iterator persistent_set::end() const {
	shared_ptr<node> it = root;
	while (it->r != nullptr) 
		it = it->r;
	return iterator(it, root);
}

persistent_set::iterator persistent_set::find(value_type other) {
	shared_ptr<node> it = root->find_node(other);                
	if (it == nullptr)
		return end();                                    
	return iterator(it, root);                                        
}

pair<persistent_set::iterator, bool> persistent_set::insert(value_type other) {
	persistent_set::iterator it = find(other);                                     
	if (it != end()) 
		return make_pair(it, false);                  
	it = iterator(root->insert_node(other), root);                    
	return make_pair(it, true);
}

void persistent_set::erase(iterator it) {
	root->erase_node(it.cur, root);
}


