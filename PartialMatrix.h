//
// Created by mgh on 3/24/17.
//

#ifndef CAKNNSR_PARTIALMATRIX_H
#define CAKNNSR_PARTIALMATRIX_H

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


class  PartialMatrix{  // may be modified to disk based service
private:
    PartialMatrix(){}
    unordered_map<bitset<64>, double> m; // partial distance matrix M, key: 32int + 32int

    static bitset<64> genKey(int nid, int lid){
        bitset<64> k;
        k |= nid;
        k <<= 32;
        k |= lid;
        return k;
    }

    static PartialMatrix* pm;

public:

    static void buildPartialMatrix(){pm = new PartialMatrix();}

    static bool isExist(int nid, int lid){
        bitset<64> k = genKey(nid, lid);
        return pm->m.find(k) != pm->m.end();
    }

    static void setDist(int nid, int lid, double dist){
        bitset<64> k = genKey(nid, lid);
        pm->m[k] = dist;
    }

    static double getDist(int nid, int lid){
        bitset<64> k = genKey(nid, lid);
        if (pm->m.find(k) != pm->m.end()) return pm->m[k];
        else return DBL_MAX;
    }
};

PartialMatrix* PartialMatrix::pm = NULL;

#endif //CAKNNSR_PARTIALMATRIX_H
