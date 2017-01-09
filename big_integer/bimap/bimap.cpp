#include "bimap.h"
#include <vector>
#include <iostream>
#include <functional>

using namespace std;

bimap::bimap() {
	root = new node();
}

bimap::~bimap() {
	if (root->right_right != nullptr) {
		std::function<void(node*)> clear_all = [&](node* cur) {
			if (cur->right_right != nullptr)
				clear_all(cur->right_right);
			if (cur->right_left != nullptr)
				clear_all(cur->right_left);
			delete(cur);
		};
		clear_all(root->right_right);
	}
	delete(root);
}

bimap::left_iterator bimap::insert(bimap::left_t const& left, right_t const& right) {

	if (root->left_left == nullptr) {
		node* temp = new node();
		temp->left_data = left;
		temp->right_data = right;
		temp->left_parent = root;
		root->left_left = temp;
		temp->right_data = right;
		temp->right_parent = root;
		root->right_left = temp;
		left_iterator trying_it;
		trying_it.cur = temp;
		return trying_it;
	}

	node *temp_left = root->left_left, *temp_right = root->right_left;
	int l_insert = 0, r_insert = 0;

	while (true) {
		if (temp_left->left_data == left) break;
		if (temp_left->left_data < left) {
			if (temp_left->left_right != nullptr) 
				temp_left = temp_left->left_right;
			else {
				l_insert = 2;
				break;
			}
		}
		if (temp_left->left_data > left) {
			if (temp_left->left_left != nullptr) {
				temp_left = temp_left->left_left;
			}
			else {
				l_insert = 1;
				break;
			}
		}
	}
	while (true) {
		if (temp_right->right_data == right) break;
		if (temp_right->right_data < right) {
			if (temp_right->right_right != nullptr) 
				temp_right = temp_right->right_right;
			else {
				r_insert = 2;
				break;
			}
		}
		if (temp_right->right_data > right) {
			if (temp_right->right_left != nullptr) 
				temp_right = temp_right->right_left;
			else {
				r_insert = 1;
				break;
			}
		}
	}
	if (l_insert != 0 && r_insert != 0) {
		node* temp = new node();
		temp->left_data = left;
		temp->right_data = right;
		temp->left_parent = temp_left;
		if (l_insert == 1)
			temp_left->left_left = temp;
		else
			temp_left->left_right = temp;

		temp->right_data = right;
		temp->right_parent = temp_right;
		if (r_insert == 1)
			temp_right->right_left = temp;
		else
			temp_right->right_right = temp;
		left_iterator trying_it;
		trying_it.cur = temp;
		return trying_it;
	}
	else
		return end_left();
}

void bimap::erase(bimap::right_iterator it) {
	if (it.cur->right_parent != nullptr) {
		bool l_parent, r_parent;
		if (it.cur->right_parent->right_right == it.cur) 
			l_parent = 0;
		else 
			l_parent = 1;
		if (it.cur->left_parent->left_right == it.cur)
			r_parent = 0;
		else
			r_parent = 1;
		if (it.cur->right_right == nullptr && it.cur->right_left == nullptr) {
			if (!l_parent)
				it.cur->right_parent->right_right = nullptr;
			else
				it.cur->right_parent->right_left = nullptr;
		}
		else if (it.cur->right_right != nullptr && it.cur->right_left != nullptr) {
			if (!l_parent)
				it.cur->right_parent->right_right = it.cur->right_right;
			else
				it.cur->right_parent->right_left = it.cur->right_right;
			it.cur->right_right->right_parent = it.cur->right_parent;
			node* temp = it.cur->right_right;
			while (temp->right_left != nullptr) 
				temp = temp->right_left;
			temp->right_left = it.cur->right_left;
			it.cur->right_left->right_parent = temp;
		}
		else {
			if (!l_parent) {
				if (it.cur->right_right != nullptr)
					it.cur->right_parent->right_right = it.cur->right_right;
				else
					it.cur->right_parent->right_right = it.cur->right_left;
			}
			else {
				if (it.cur->right_right != nullptr)
					it.cur->right_parent->right_left = it.cur->right_right;
				else
					it.cur->right_parent->right_left = it.cur->right_left;
			}
			if (it.cur->right_right != nullptr)
				it.cur->right_right->right_parent = it.cur->right_parent;
			else
				it.cur->right_left->right_parent = it.cur->right_parent;
		}

		if (it.cur->left_right == nullptr && it.cur->left_left == nullptr) {
			if (r_parent == false)
				it.cur->left_parent->left_right = nullptr;
			else
				it.cur->left_parent->left_left = nullptr;
		}
		else if (it.cur->left_right != nullptr && it.cur->left_left != nullptr) {
			if (!r_parent)
				it.cur->left_parent->left_right = it.cur->left_right;
			else
				it.cur->left_parent->left_left = it.cur->left_right;
			it.cur->left_right->left_parent = it.cur->left_parent;
			node* trying = it.cur->left_right;
			while (trying->left_left != nullptr) 
				trying = trying->left_left;
			trying->left_left = it.cur->left_left;
			it.cur->left_left->left_parent = trying;
		}
		else {
			if (!r_parent) {
				if (it.cur->left_right != nullptr)
					it.cur->left_parent->left_right = it.cur->left_right;
				else
					it.cur->left_parent->left_right = it.cur->left_left;
			}
			else {
				if (it.cur->left_right != nullptr)
					it.cur->left_parent->left_left = it.cur->left_right;
				else
					it.cur->left_parent->left_left = it.cur->left_left;
			}
			if(it.cur->left_right != nullptr) 
				it.cur->left_right->left_parent = it.cur->left_parent;
			else
				it.cur->left_left->left_parent = it.cur->left_parent;
		}

		delete it.cur;
	}
}

void bimap::erase(bimap::left_iterator it) {
	erase(it.flip());
}

bimap::right_iterator  bimap::find_right(bimap::right_t const& right)  const {
	if (root->right_left == nullptr) 
		return end_right();
	node *temp = root->right_left;
	while (true) {
		if (temp->right_data == right) 
			break;
		else if (temp->right_data < right && temp->right_right != nullptr) 
			temp = temp->right_right;
		else if(temp->right_data > right && temp->right_left != nullptr) 
			temp = temp->right_left;
		else if (temp->right_right == nullptr || temp->right_left == nullptr)
			return end_right();
	}
	right_iterator temp_it;
	temp_it.cur = temp;
	return temp_it;
}

bimap::left_iterator bimap::find_left(bimap::left_t const& left) const {
	if (root->left_left == nullptr) 
		return end_left();
	node *temp = root->left_left;
	while (true) {
		if (temp->left_data == left) 
			break;
		else if (temp->left_data < left && temp->left_right != nullptr) 
			temp = temp->left_right;
		else if (temp->left_data > left && temp->left_left != nullptr)
			temp = temp->left_left;
		else if(temp->left_right == nullptr || temp->left_left == nullptr)
			return end_left();
	}
	left_iterator temp_it;
	temp_it.cur = temp;
	return temp_it;
}

bimap::right_iterator bimap::begin_right() const {
	if (root->right_left == nullptr)
		return end_right();
	node* temp = root->right_left;
	while (temp->right_left != nullptr)
		temp = temp->right_left;
	right_iterator temp_it;
	temp_it.cur = temp;
	return temp_it;
}

bimap::left_iterator bimap::begin_left() const {
	if (root->left_left == nullptr) 
		return end_left();
	node* temp = root->left_left;
	while (temp->left_left != nullptr)
		temp = temp->left_left;
	left_iterator temp_it;
	temp_it.cur = temp;
	return temp_it;
}
bimap::left_iterator bimap::end_left() const {
	left_iterator left;
	left.cur = root;
	return left;
}

bimap::right_iterator bimap::end_right() const {
	right_iterator right;
	right.cur = root;
	return right;
}

bimap::right_iterator bimap::left_iterator::flip() const {
	right_iterator right;
	right.cur = cur;
	return right;
}

bimap::left_iterator bimap::right_iterator::flip() const {
	left_iterator left;
	left.cur = cur;
	return left;
}

bimap::right_t const& bimap::right_iterator::operator*() const {
	return cur->right_data;
}

bimap::right_iterator& bimap::right_iterator::operator++() {
	if (cur->right_left != nullptr) {
		cur = cur->right_left;
		while (cur->right_right != nullptr) cur = cur->right_right;
	}
	else {
		while (cur->right_parent != nullptr && cur->right_parent->right_left == cur)
			cur = cur->right_parent;
		if (cur->right_parent != nullptr)
			cur = cur->right_parent;
	}
	return *this;
}

bimap::right_iterator& bimap::right_iterator::operator--() {
	if (cur->right_right != nullptr) {
		cur = cur->right_right;
		while (cur->right_left != nullptr) 
			cur = cur->right_left;
	}
	else {
		while (cur->right_parent != nullptr && cur->right_parent->right_right == cur)
			cur = cur->right_parent;
		if (cur->right_parent != nullptr)
			cur = cur->right_parent;
	}
	return *this;
}

bimap::right_iterator bimap::right_iterator::operator--(int) {
	right_iterator res(*this);
	(*this)--;
	return res;
}

bimap::right_iterator bimap::right_iterator::operator++(int) {
	right_iterator res(*this);
	(*this)++;
	return res;
}

bool bimap::right_iterator::operator==(const right_iterator left) const {
	return cur == left.cur;
}

bool bimap::right_iterator::operator!=(const right_iterator left) const {
	return cur != left.cur;
}

bimap::left_t const& bimap::left_iterator::operator*() const {
	return cur->left_data;
}

bimap::left_iterator& bimap::left_iterator::operator--() {
	if (cur->left_right != nullptr) {
		cur = cur->left_right;
		while (cur->left_left != nullptr)
			cur = cur->left_left;
	}
	else {
		while (cur->left_parent != nullptr && cur->left_parent->left_right == cur)
			cur = cur->left_parent;
		if (cur->left_parent != nullptr)
			cur = cur->left_parent;
	}
	return *this;
}

bimap::left_iterator& bimap::left_iterator::operator++() {
	if (cur->left_left != nullptr) {
		cur = cur->left_left;
		while (cur->left_right != nullptr)
			cur = cur->left_right;
	}
	else {
		while (cur->left_parent != nullptr && cur->left_parent->left_left == cur) 
			cur = cur->left_parent;
		if (cur->left_parent != nullptr)
			cur = cur->left_parent;
	}
	return *this;
}

bimap::left_iterator bimap::left_iterator::operator++(int) {
	left_iterator res(*this);
	(*this)++;
	return res;
}

bimap::left_iterator bimap::left_iterator::operator--(int) {
	left_iterator res(*this);
	(*this)--;
	return res;
}

bool bimap::left_iterator::operator==(const left_iterator left) const {
	return cur == left.cur;
}
bool bimap::left_iterator::operator!=(const left_iterator left) const {
	return cur != left.cur;
}