//
// Created by mgh on 3/26/17.
//

#ifndef CAKNNSR_BASEEXPANSION_H
#define CAKNNSR_BASEEXPANSION_H

#include <algorithm>
#include <vector>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <set>

#include "MovingObject.h"
#include "PartialMatrix.h"
#include "DistElement.h"
#include "Graph.h"

using namespace std;

class BaseExpansion{
protected:
    double expandedRange;

public:
    virtual bool isVisit(int frm, pair<int, ElemType> to){
        cout << "not implemented isVisit function called";
        return false;
    }

    virtual void visit(int frm, pair<int, ElemType> to){
        cout << "not implemented visit function called";
    }

    virtual unordered_set<int> expand(double r){
        cout << "not implemented expand function called";
        return  unordered_set<int>();
    }

    virtual void display() const {}

    double getExpandedRange(){return expandedRange;}
};



#endif //CAKNNSR_BASEEXPANSION_H