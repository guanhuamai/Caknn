#ifndef __MOVINGOBJECT
#define __MOVINGOBJECT

#include <algorithm>
#include <limits.h>
#include <float.h>
#include <xvilla.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using namespace std;
using namespace qdbm;


Villa* vl;

class MovingObject{  // moving object during the query, not the whole moving objects

    static unordered_map<int, unordered_set<int>> hsObj;

    static pair<int, double > read(int key) {
        char *v = NULL;
        pair<int, double> p = pair<int, double>(INT_MAX, DBL_MAX);

        try {
            v = vl->get((const char *) &key, sizeof(int), NULL);

            memcpy(&p.first, v, sizeof(int));
            memcpy(&p.second, v + sizeof(int), sizeof(double));

        }catch (Villa_error& e) {
            if (e.code() != Villa::ENOITEM) throw e;
//            else printf("item not found\n");
        }

        if (v!=NULL) free(v);
        return p;
    }

    static void write(int pid, int eid, double pos) {
        char* k = new char[100];
        char* v = new char[100];

        memcpy(k, &pid, sizeof(int));
        memcpy(v, &eid, sizeof(int));
        memcpy(v + sizeof(int), &pos, sizeof(double));

        try {
            vl->put(k, sizeof(int), v, sizeof(int) + sizeof(double), Villa::DOVER);
        }catch (Villa_error& e) { throw e; }

        free(k);
        free(v);
    }

    static void erase(int pid){
        char* k = new char[100];
        memcpy(k, &pid, sizeof(int));
        try{
            vl->out(k, sizeof(int));
        }catch (Villa_error& e){
            if (e.code() != Villa::ENOITEM) throw e;
            else printf("delete item with key %d failed, not found\n", pid);
        }
        free(k);
    }

    MovingObject(){}  // ban initialization

public:

    static void closeDB(){vl->close();}

    static void setDB(const char* DBNAME){vl = new Villa(DBNAME, Villa::OCREAT | Villa::OWRITER);}

    static void insertP(int pid, int eid, double pos){
//        printf("insert %d\n", pid);
        if(hsObj.find(eid) == hsObj.end())
            hsObj[eid] = unordered_set<int>();  // initialize the set
        hsObj[eid].insert(pid);
        write(pid, eid, pos);
    }

    static pair<int, double> getP(int pid) {return read(pid);}

    static vector<pair<int, pair<int, double>>> getPFromE(int eid){  // get moving objects from edge
        if (hsObj.find(eid) == hsObj.end())  return vector<pair<int, pair<int, double>>>(0);
        vector<pair<int, pair<int, double>>> res;
        for(auto pid : hsObj[eid]) { res.push_back(pair<int, pair<int, double>>(pid, read(pid)));}
        return res;
    }

    static void eraseP(int pid) {
//        printf("erase %d\n", pid);
        pair<int, double > p = read(pid);
        if (p.first != INT_MAX) hsObj[read(pid).first].erase(pid), erase(pid);
//        else printf("erase failed\n");
    }

    static void updateP(int pid, int eid, double pos){
        eraseP(pid);
        if (eid != -1) insertP(pid, eid, pos);
    }

};

unordered_map<int, unordered_set<int>> MovingObject::hsObj = unordered_map<int, unordered_set<int>>();

#endif // __MOVINGOBJECT
