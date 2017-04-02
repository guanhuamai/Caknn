//
// Created by mgh on 3/24/17.
//

#ifndef CAKNNSR_OPF_H
#define CAKNNSR_OPF_H


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
#include <assert.h>

using namespace std;

namespace std{
    typedef pair<int, double > opfPair;


    bool operator == (const opfPair& a, const opfPair& b)  {
        return a.first == b.first;
    }

    bool operator != (const opfPair& a, const opfPair& b)  {
        return !(a == b);
    }
}

struct PairLess{bool operator () (pair<int, double >& a, pair<int, double>& b) const { return a.second > b.second;}};

class Opf{  // singleton: use to monitor safe region objects
private:

    // moving objects id inside of the safe region, or protential future objects
    priority_queue<pair<int, double >, vector<pair<int, double >>, PairLess> heapOpf;

    unordered_map<int,  double> opf; // use bits of double

    static Opf* opfPtr;

    Opf(){};  // ban initialization

    static void heapGC(){
        opfPtr->heapOpf = priority_queue<pair<int, double >, vector<pair<int, double >>, PairLess>();

        for(const auto& e: opfPtr->opf){
            opfPtr->heapOpf.push(e);
        }
    }

public:

    static pair<int, double > USE_LESS_PAIR;

    static void buildOpf(){
        destructOpf();
        opfPtr = new Opf();
    }

    static void destructOpf(){
        if (opfPtr != NULL){
            delete opfPtr;
            opfPtr = NULL;
        }
    }

    static void insert(int mid, double dist){
        if (opfPtr->opf.find(mid) == opfPtr->opf.end()
             || opfPtr->opf[mid] != dist) {
            opfPtr->opf[mid] = dist;
            opfPtr->heapOpf.push(pair<int, double>(mid, dist));
        }
    }

    static pair<int, double> getMin(){ // return the first element found in opf
        while (!opfPtr->heapOpf.empty()){
            if (!exist(opfPtr->heapOpf.top().first)) {
                opfPtr->heapOpf.pop();
            }
            else {
                pair<int, double> p = opfPtr->heapOpf.top();
                if (opfPtr->opf[p.first] == p.second){ // same with each bit
                    return p;
                }else opfPtr->heapOpf.pop();
            }
        }
        return USE_LESS_PAIR;
    }

    static vector<pair<int, double>> getNSmallest(int n){
//        vector<pair<int, double>> res;
//
//        opfPtr->heapOpf = priority_queue<pair<int, double >, vector<pair<int, double >>, PairLess> ();
//
//        for (const auto& e: opfPtr->opf){
//            opfPtr->heapOpf.push(e);
//            if (opfPtr->heapOpf.size() > n) opfPtr->heapOpf.pop();
//        }
//
//        while (!opfPtr->heapOpf.empty() && n != 0){
//            res.push_back(opfPtr->heapOpf.top());
//            opfPtr->heapOpf.pop();
//            n --;
//        }

        vector<pair<int, double>> res;

        while (n != 0 && !opfPtr->heapOpf.empty()){
            auto p = opfPtr->heapOpf.top();
            opfPtr->heapOpf.pop();
            if (opfPtr->opf.find(p.first) != opfPtr->opf.end()
                && opfPtr->opf[p.first] == p.second){
                res.push_back(p);
                n--;
            }
        }

        for (const auto& e: res){
            opfPtr->heapOpf.push(e);
        }

        if (opfPtr->heapOpf.size() > 300000){
            heapGC();
        }

        return res;
    };

    static void erase(int mid){
        if(opfPtr->opf.find(mid) == opfPtr->opf.end()) return;
        opfPtr->opf.erase(mid);
    }

    static bool exist(int mid){
        return opfPtr->opf.find(mid) != opfPtr->opf.end();
    }

    static void display(){
        cout << "opf heap size: " << Opf::opfPtr->heapOpf.size() << endl;
        cout << "opf map size: " << Opf::opfPtr->opf.size() << endl;
//        auto tmp = Opf::opfPtr->heapOpf;
//        cout << "display opf pq" << endl;
//        while (!tmp.empty()){
//            cout << "mid: " << tmp.top().first << " agg value: " << tmp.top().second << endl;
//            tmp.pop();
//        }
//        cout << "display opf map: " << endl;
//        for (const auto& e: Opf::opfPtr->opf){
//            cout << "key: " << e.first << " val: " << e.second << endl;
//        }
//        cout << "--------" << endl;
    }
};

Opf* Opf::opfPtr = NULL;

pair<int, double> Opf::USE_LESS_PAIR = pair<int, double>(INT_MAX, DBL_MAX);



#endif //CAKNNSR_OPF_H
