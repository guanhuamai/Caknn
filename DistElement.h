//
// Created by mgh on 3/24/17.
//

#ifndef CAKNNSR_DISTELEM_H
#define CAKNNSR_DISTELEM_H

#include <vector>
#include <algorithm>
#include <vector>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <climits>
#include <cfloat>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

const enum ElemType {NODE, MOVING_OBJECT, LANDMARK};

class DistElement{
private:

    int strtId;  //(0, N - 1): node; [N, N + L): landmarks; [N + L,): moving objects
    int endId;  //landmark id
    double dist;

    ElemType startElemType;
    ElemType endElemType;



public:

    DistElement(int strtId, ElemType sType, int endId, ElemType eType, double d):
            strtId(strtId), startElemType(sType), endId(endId), endElemType(eType),
            dist(d){}

    bool operator < (const DistElement& b) const{
        return dist > b.dist;
    }

    bool operator () (DistElement& a, DistElement& b) const {
        return a.dist < b.dist;
    }

    pair<int, ElemType > getStartNode(){return pair<int, ElemType >(strtId, startElemType);}

    pair<int, ElemType > getEndNode(){return pair<int, ElemType >(strtId, startElemType);}

    double getDist(){ return dist;}

};


class DistElements{

private:
    priority_queue<DistElement, vector<DistElement>, DistElement> h;

public:

    void push(DistElement distElement){h.push(distElement);}

    void push(int strtId, ElemType sType,
              int endId, ElemType eType,
              double dist){
        h.push(DistElement(strtId, sType, endId, eType, dist));
    }

    DistElement top(){
        if (h.empty())  return DistElement(-1, DistElement::NODE, -1, DistElement::NODE, DBL_MAX);
        else return h.top();
    }

    void pop(){
        if (!h.empty()){
            h.pop();
        }
    }

    bool empty(){return h.empty();}
};

#endif //CAKNNSR_DISTELEM_H
