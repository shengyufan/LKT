#include <queue>
#include <math.h>
#include "Entry.h"
using namespace std;

Node::Node(float x1, float y1, float x2, float y2, int l)
{
    min_x = x1;
    min_y = y1;
    max_x = x2;
    max_y = y2;
    threshold = 2;
    isLeaf = true;
    // leaf_IF = unordered_map<uint64_t, vector<Object*>> ();
    // IF = unordered_map<uint64_t, vector<Node*>> ();
    invertedFile = unordered_map<uint64_t, float> ();
    objList = vector<Object*> ();
    level = l;
}

bool Node::containObject(Object *o)
{
    if(o->x >= min_x && o->x<= max_x && o->y >= min_y && o->y <= max_y)
    {
        return true;
    }

    return false;
}

void Node::insert(Object* o)
{
    for(auto &v : o->weightedKey)
    {
        if(invertedFile.find(v.first) != invertedFile.end())
        {
            float temp = invertedFile.at(v.first);
            if(temp < v.second)
            {
                temp = v.second;
                invertedFile[v.first] = temp;
            }
        }
        else
        {
            invertedFile[v.first] = v.second;
        }
    }
    if(isLeaf)
    {
        objList.emplace_back(o);
        if(objList.size() > threshold)
        {
            isLeaf = false;
            split();
            objList.clear();
        }
    }
    else
    {
        if(this->NW->containObject(o))
        {
            this->NW->insert(o);
        }
        else if(this->NE->containObject(o))
        {
            this->NE->insert(o);
        }
        else if(this->SW->containObject(o))
        {
            this->SW->insert(o);
        }
        else if(this->SE->containObject(o))
        {
            this->SE->insert(o);
        }
    }
}

void Node::split()
{
    this->NW = new Node(min_x, (min_y + max_y) / 2, (min_x + max_x) / 2, max_y, level + 1);
    this->NE = new Node((min_x + max_x) / 2, (min_y + max_y) / 2, max_x, max_y, level + 1);
    this->SW = new Node(min_x, min_y, (min_x + max_x) / 2, (min_y + max_y) / 2, level + 1);
    this->SE = new Node((min_x + max_x) / 2, min_y, max_x, (min_y + max_y) / 2, level + 1);

    for(int n = 0; n < objList.size(); n++)
    {
        Object *o = objList.at(n);
        if(this->NW->containObject(o))
        {
            this->NW->insert(o);
        }
        else if(this->NE->containObject(o))
        {
            this->NE->insert(o);
        }
        else if(this->SW->containObject(o))
        {
            this->SW->insert(o);
        }
        else if(this->SE->containObject(o))
        {
            this->SE->insert(o);
        }
    }
}

float computeScore(Entry &e, TopkQuery &q)
{
    float sscore = 0;
    float tscore = 0;
    float alpha = 0.4;
    if(e.isObject)
    {
        for(int i = 0; i < q.keys.size(); i++)
        {
            uint64_t key = q.keys.at(i);
            if(e.object->weightedKey.find(key) != e.object->weightedKey.end())
            {
                tscore += e.object->weightedKey.at(key);
            }
        }
        
        sscore = sqrt((e.object->x - q.x) * (e.object->x - q.x) + (e.object->y - q.y) * (e.object->y - q.y));
    }
    else
    {
        for(int i = 0; i < q.keys.size(); i++)
        {
            uint64_t key = q.keys.at(i);
            if(e.node->invertedFile.find(key) != e.node->invertedFile.end())
            {
                tscore += e.node->invertedFile.at(key);
            }
        }
        
        if(e.node->min_x > q.x)
        {
            sscore += (e.node->min_x - q.x) * (e.node->min_x - q.x);
        }
        else if(e.node->max_x < q.x)
        {
            sscore += (e.node->max_x - q.x) * (e.node->max_x - q.x);
        }

        if(e.node->min_y > q.y)
        {
            sscore += (e.node->min_y - q.y) * (e.node->min_y - q.y);
        }
        else if(e.node->max_y < q.y)
        {
            sscore += (e.node->max_y - q.y) * (e.node->max_y - q.y);
        }

        sscore = sqrt(sscore);
    }

    return alpha * sscore + (1 - alpha) * (1 - tscore);
}

vector<Object*> Node::topkQuery(int k, TopkQuery &query)
{
    vector<Object*> result;
    priority_queue<Entry, vector<Entry>, Entry::ascending> pq;
    pq.push(Entry(this, NULL, 0.0, false));
    int count = 0;
    float knearest = 0.0;
    while(!pq.empty())
    {
        Entry e = pq.top();
        pq.pop();
        
        if(e.isObject)
        {
            result.emplace_back(e.object);
            count++;
            if(count >= k)
            {
                break;
            }
        }
        else
        {
            Node* node = e.node;
            if(node->isLeaf)
            {
                for(int i = 0; i < node->objList.size(); i++)
                {
                    Object* o = node->objList.at(i);
                    Entry e(NULL, o, 0.0, true);
                    float score = computeScore(e, query);
                    e.updateDis(score);
                    pq.push(e);
                }
            }
            else
            {
                Entry nw_e(node->NW, NULL, 0.0, false);
                float nw_score = computeScore(nw_e, query);
                nw_e.updateDis(nw_score);
                pq.push(nw_e);

                Entry ne_e(node->NE, NULL, 0.0, false);
                float ne_score = computeScore(ne_e, query);
                ne_e.updateDis(ne_score);
                pq.push(ne_e);

                Entry sw_e(node->SW, NULL, 0.0, false);
                float sw_score = computeScore(sw_e, query);
                sw_e.updateDis(sw_score);
                pq.push(sw_e);

                Entry se_e(node->SE, NULL, 0.0, false);
                float se_score = computeScore(se_e, query);
                se_e.updateDis(se_score);
                pq.push(se_e);
            }
        }
    }
    return result;
}

void Node::printInfo()
{
    cout << "Level: " << level << endl;
    cout << min_x << ", " << min_y << " " << max_x << ", " << max_y << endl;
    for(auto &v : invertedFile)
    {
        cout << v.first << "," << v.second << " ";
    }
    cout << endl;

    if(isLeaf)
    {
        // cout << "Leaf Keywords: ";
        // for(auto it = leaf_IF.begin(); it != leaf_IF.end(); it++)
        // {
        //     cout << it->first << " ";
        // }
        // cout << endl;

        cout << "Objects: ";
        for_each(objList.begin(), objList.end(), [](const auto &o){cout << o->x << "," << o->y << " ";});
        cout << endl;
    }
    else
    {
        // cout << "Keywords: ";
        // for(auto it = IF.begin(); it != IF.end(); it++)
        // {
        //     cout << it->first << " ";
        // }
        // cout << endl;

        this->NW->printInfo();
        this->NE->printInfo();
        this->SW->printInfo();
        this->SE->printInfo();
    }
}
