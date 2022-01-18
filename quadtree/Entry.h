#ifndef ENTRY_H
#define ENTRY_H

#include "Node.h"
#include "Object.h"
using namespace std;

class Entry
{
public:
    Node* node;
    Object* object;
    float minDis;
    bool isObject;

    Entry(Node* n, Object* o, float dis, bool f) : node(n), object(o), minDis(dis), isObject(f){};
    struct ascending : public binary_function<Entry , Entry, bool>
    {
        bool operator()(const Entry &_a, const Entry &_b) const
        {
            return _a.minDis > _b.minDis;
        }
    };

    void updateDis(float dis)
    {
        minDis = dis;
    }
};

#endif