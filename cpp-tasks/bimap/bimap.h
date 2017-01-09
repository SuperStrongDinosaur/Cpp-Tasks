#ifndef bimap_h
#define bimap_h

#include <cstdio>
#include <iostream>


struct bimap
{

	// �� ������ ���������� ��� �������� �� ������ ����������.
	typedef int left_t;
	typedef int right_t;

	struct left_iterator;
	struct right_iterator;
	struct node;

	//    friend left_iterator;
	//    friend right_iterator;

	// ������� bimap �� ���������� �� ����� ����.
	bimap();

	// ����������. ���������� ��� �������� �������� bimap.
	// ������������ ��� ��������� ����������� �� �������� ����� bimap
	// (������� ��������� ����������� �� �������� ��������� �� ����������).
	~bimap();

	// ������� ���� (left, right), ���������� �������� �� left.
	// ���� ����� left ��� ����� right ��� ������������ � bimap, ������� ��
	// ������������ � ������������ end_left().
	left_iterator insert(left_t const& left, right_t const& right);

	// ������� ������� � ��������������� ��� ������.
	// erase ����������� ��������� �����������.
	// erase(end_left()) � erase(end_right()) ������������.
	// ����� it ��������� �� ��������� ������� e.
	// erase ������������ ��� ��������� ����������� �� e � �� ������� ������ � e.
	void erase(left_iterator it);
	void erase(right_iterator it);

	// ���������� �������� �� ��������. � ������ ���� ������� �� ������, ����������
	// end_left()/end_right() ��������������.
	left_iterator  find_left(left_t  const& left)  const;
	right_iterator find_right(right_t const& right) const;

	// ��������� �������� �� ����������� �� �������� left.
	left_iterator begin_left() const;
	// ��������� �������� �� ��������� �� ��������� �� �������� left.
	left_iterator end_left() const;

	// ��������� �������� �� ����������� �� �������� right.
	right_iterator begin_right() const;
	// ��������� �������� �� ��������� �� ��������� �� �������� right.
	right_iterator end_right() const;

private:
	node* root;
};

struct bimap::left_iterator
{
	friend struct bimap;
	// ������� �� ������� ������ ��������� ��������.
	// ������������� ��������� end_left() ������������.
	// ������������� ����������� ��������� ������������.
	left_t const& operator*() const;

	// ������� � ���������� �� �������� left'�.
	// ��������� ��������� end_left() �����������.
	// ��������� ����������� ��������� �����������.
	left_iterator& operator++();
	left_iterator operator++(int);

	// ������� � ����������� �� �������� left'�.
	// ��������� ��������� begin_left() �����������.
	// ��������� ����������� ��������� �����������.
	left_iterator& operator--();
	left_iterator operator--(int);

	// left_iterator ��������� �� ����� ������� ��������� ����.
	// ��� ������� ���������� �������� �� ������ ������� ��� �� ����.
	// end_left().flip() ��������� end_right().
	// end_right().flip() ���������� end_left().
	// flip() ����������� ��������� �����������.
	right_iterator flip() const;

	bool operator==(const left_iterator) const;
	bool operator!=(const left_iterator) const;

private:
	node* cur;
};

struct bimap::right_iterator
{
	friend struct bimap;
	// ����� �� ���������� left_iterator.
	right_t const& operator*() const;

	right_iterator& operator++();
	right_iterator operator++(int);

	right_iterator& operator--();
	right_iterator operator--(int);

	left_iterator flip() const;

	bool operator==(const right_iterator) const;
	bool operator!=(const right_iterator) const;

private:
	node* cur;
};

struct bimap::node
{
	bimap::left_t  left_data;
	node*   left_left;
	node*   left_right;
	node*   left_parent;

	bimap::right_t right_data;
	node*   right_left;
	node*   right_right;
	node*   right_parent;
};


#endif