#ifndef NODE_H
#define NODE_h

#include <vector>
#include <set>
#include <unordered_map>
#include <stdint.h>
#include <algorithm>
#include <iostream>
#include "Object.h"
#include "TopkQuery.h"
using namespace std;

class Node
{
public:
    float min_x;
    float min_y;
    float max_x;
    float max_y;
    // unordered_map<uint64_t, vector<Object*>> leaf_IF;
    // unordered_map<uint64_t, vector<Node*>> IF;
    unordered_map<uint64_t, float> invertedFile;
    bool isLeaf;
    int threshold;
    int level;
    vector<Object*> objList;

    Node* NW;
    Node* NE;
    Node* SW;
    Node* SE;

    Node(float x1, float y1, float x2, float y2, int level);
    bool containObject(Object *o);
    void insert(Object *o);
    void split();
    void printInfo();
    vector<Object*> topkQuery(int topk, TopkQuery &q);

    bool operator == (const Node & n) const
    {
        return n.level == level && n.min_x == min_x && n.max_x == max_x && n.min_y == min_y && n.max_y == max_y;
    }
};

#endif