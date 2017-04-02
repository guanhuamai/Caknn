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
#include <hash_fun.h>

using namespace std;

enum ElemType{NODE=0, MOVING_OBJECT, LANDMARK};

const char* ELEM_TYPE_STRS [] = {"NODE", "MOVING_OBJECT", "LANDMARK"};

class ElementSet{

private:
    bitset<64> getKey(int id, ElemType elemType){
        bitset<64> k;
        k |= id;
        k <<= 32;
        k |= elemType;
        return k;
    }

    pair<int, ElemType > getElement(bitset<64> k) const {
        int id = (int)(k >> 32).to_ulong();
        ElemType elemType = (ElemType) k.to_ulong();
        return pair<int, ElemType >(id, elemType);
    };

    unordered_set<bitset<64>> elementSet;

public:

    void insert(pair<int, ElemType> elem){
        bitset<64> k = getKey(elem.first, elem.second);
        elementSet.insert(k);
    }

    bool isExist(pair<int, ElemType> elem){
        bitset<64> k = getKey(elem.first, elem.second);
        return elementSet.find(k) != elementSet.end();
    }

    void erase(pair<int, ElemType> elem){
        bitset<64> k = getKey(elem.first, elem.second);
        elementSet.erase(k);
    }

    void display() const {
        cout << "display ElementSet" << endl;
        for (const auto& e: elementSet){
            auto p = getElement(e);
            cout << "id: " << p.first
                 << " type: " << ELEM_TYPE_STRS[p.second]
                 << endl;
        }
        cout << "--------------" << endl;
    }
};

class DistElement{
private:

    int strtId;  //(0, N - 1): node; [N, N + L): landmarks; [N + L,): moving objects
    int endId;  //landmark id
    double dist;

    ElemType startElemType;
    ElemType endElemType;

public:

    DistElement() : strtId(0), endId(0), dist(DBL_MAX){}

    DistElement(int strtId, ElemType sType, int endId, ElemType eType, double d):
            strtId(strtId), startElemType(sType), endId(endId), endElemType(eType),
            dist(d){}

    bool operator < (const DistElement& b) const{
        return dist > b.dist;
    }

    bool operator () (DistElement& a, DistElement& b) const {
        return a.dist > b.dist;
    }

    pair<int, ElemType > getStartNode(){return pair<int, ElemType >(strtId, startElemType);}

    pair<int, ElemType > getEndNode(){return pair<int, ElemType >(endId, endElemType);}

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
        if (h.empty())  return DistElement(-1, ElemType ::NODE, -1, ElemType ::NODE, DBL_MAX);
        else return h.top();
    }

    void pop(){
        if (!h.empty()){
            h.pop();
        }
    }

    bool empty(){return h.empty();}

    void display() const {
        priority_queue<DistElement, vector<DistElement>, DistElement> tmp = h;
        cout << "display DistElements:" << endl;
        while (!tmp.empty()){
            auto p = tmp.top();
            cout << "start node: id: " << p.getStartNode().first
                 << " type: " << ELEM_TYPE_STRS[p.getStartNode().second]
                 << " end node: id: " << p.getEndNode().first
                 << " type: " << ELEM_TYPE_STRS[p.getEndNode().second]
                 << " dist: " << p.getDist()
                 << endl;
            tmp.pop();
        }
        cout << "-----------" << endl;
    }
};



//int main(){
//    ElementSet s;
//
//    for (int i = 0; i < 10; i++){
//        s.insert(pair<int, ElemType >(i % 4, (ElemType)(i % 4)));
//    }
//
//    for (int i = 0; i < 10; i++){
//        for (int j = 0; j < 4; j++){
//            pair<int, ElemType > tmp(i, (ElemType) j);
//            if (s.isExist(tmp)){
//                cout << tmp.first << " " << tmp.second << "exists" << endl;
//            }else{
//                cout << tmp.first << " " << tmp.second << "not exists" << endl;
//            }
//        }
//    }
//
//    for (int i = 0; i < 2; i++){
//        s.erase(pair<int, ElemType >(i % 4, (ElemType)(i % 4)));
//    }
//
//    for (int i = 0; i < 10; i++){
//        for (int j = 0; j < 4; j++){
//            pair<int, ElemType > tmp(i, (ElemType) j);
//            if (s.isExist(tmp)){
//                cout << tmp.first << " " << tmp.second << "exists" << endl;
//            }else{
//                cout << tmp.first << " " << tmp.second << "not exists" << endl;
//            }
//        }
//    }
//    return 0;
//}

#endif //CAKNNSR_DISTELEM_H
