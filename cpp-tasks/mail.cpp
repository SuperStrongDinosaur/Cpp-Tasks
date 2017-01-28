#include "optional.h"
#include <cassert>
#include <iostream>

using namespace std;

struct lul {
    lul() : value() { }
    lul(int value) : value(value) { }
    lul(lul const& other) : value(other.value) { }
    ~lul() { }
    friend bool operator==(lul a, lul b) { return a.value == b.value; }
    friend bool operator!=(lul a, lul b) { return a.value != b.value; }
    int value;
};

int main()
{
    optional<lul> bb;
    optional<lul> cc;
    cout << bb.operator bool() << endl;
    bb = lul{5};
    cout << bb.operator bool() << " " << (*bb).value << endl;
    bb = lul{7};
    cout << bb.operator bool() << " " << (*bb).value << endl;
    bb = nullopt_;
    cout << bb.operator bool() << endl;
    bb = bb;
    cout << bb.operator bool() << endl;
    bb.emplace(10);
    cout << bb.operator bool() << " " << (*bb).value << endl;
    
    cc = lul{7};
    cc.swap(bb);
    cout << bb.operator bool() << " " << (*bb).value << endl;
    cout << cc.operator bool() << " " << (*cc).value << endl;
    return 0;
}



