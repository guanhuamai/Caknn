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

using namespace std;



struct PairLess{bool operator () (pair<int, double >& a, pair<int, double>& b) const { return a.second < b.second;}};

class Opf{  // use to monitor safe region objects
private:

    // moving objects id inside of the safe region, or protential future objects
    priority_queue<pair<int, double >, vector<pair<int, double >>, PairLess> heapOpf;

    unordered_map<int, pair<int, double >> opf;

    static Opf* opfPtr;

    Opf(){};  // ban initialization

public:

    static const pair<int, double > USE_LESS_PAIR = pair<int, double>(INT8_MAX, DBL_MAX);

    static void buildOpf(){opfPtr = new Opf();}

    static void insert(int mid, int eid, double pos, double dist){
        opfPtr->opf[mid] = pair<int, double >(eid, pos);
        opfPtr->heapOpf.push(pair<int, double >(mid, dist));
    }

    static pair<int, double> getMin(){ // return the first element found in opf
        while (!opfPtr->heapOpf.empty()){
            if (opfPtr->opf.find(opfPtr->heapOpf.top().first) == opfPtr->opf.end())
                opfPtr->heapOpf.pop();
            return pair<int, double >(opfPtr->heapOpf.top().first, opfPtr->heapOpf.top().second);
        }

        return USE_LESS_PAIR;
    }

    static void erase(int mid){
        opfPtr->opf.erase(mid);
    }

    static bool exist(int mid){
        return opfPtr->opf.find(mid) == opfPtr->opf.end();
    }
};

#endif //CAKNNSR_OPF_H
