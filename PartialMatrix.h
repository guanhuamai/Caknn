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


class  PartialMatrix{  // singleton: may be modified to disk based service
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

    static pair<int, int> reverseKey(bitset<64> k){
        pair<int, int> res;
        res.first = (int)(k >> 32).to_ulong();
        res.second = (int)k.to_ulong();
        return res;
    };

    static PartialMatrix* pm;

public:

    static void buildPartialMatrix(){
        destructPartialMatrix();
        pm = new PartialMatrix();
    }

    static void destructPartialMatrix(){
        if (pm != NULL){
            delete(pm);
            pm = NULL;
        }
    }

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

    static void display(){
        cout << "display partial matrix" << endl;
        for (const auto &k : pm->m){
            pair<int, int> p = reverseKey(k.first);
            cout << "nid: " << p.first << " lid: " << p.second << " dist: " << k.second << endl;
        }
        cout << endl;
    }

};

PartialMatrix* PartialMatrix::pm = NULL;

#endif //CAKNNSR_PARTIALMATRIX_H
