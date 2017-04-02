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



class MovingObject{  //singleton
private:

    Villa* vl;

    unordered_map<int, unordered_set<int>> hsObj;

    pair<int, double > read(int key) {
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

    void write(int pid, int eid, double pos) {
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

    void erase(int pid){
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

    MovingObject(string dbname){
        vl = new Villa(dbname.c_str(), Villa::OCREAT | Villa::OWRITER);
        hsObj = unordered_map<int, unordered_set<int>>();
    }

    static MovingObject* movingObjectPtr;


public:

    static void buildMovingObject(string dbname){
        destructMovingObject();
        movingObjectPtr = new MovingObject(dbname);
    }

    static void destructMovingObject(){
        if (movingObjectPtr != NULL) {
            movingObjectPtr->vl->close();
            delete movingObjectPtr;
            movingObjectPtr = NULL;
        }
    }


    static void insertP(int pid, int eid, double pos){
        if(movingObjectPtr->hsObj.find(eid) == movingObjectPtr->hsObj.end())
            movingObjectPtr->hsObj[eid] = unordered_set<int>();  // initialize the set
        movingObjectPtr->hsObj[eid].insert(pid);
        movingObjectPtr->write(pid, eid, pos);
    }

    static pair<int, double> getP(int pid) {return movingObjectPtr->read(pid);}

    static unordered_set<int> getPidFromE(int eid){
        if (movingObjectPtr->hsObj.find(eid) == movingObjectPtr->hsObj.end())  return unordered_set<int>();
        return movingObjectPtr->hsObj[eid];
    }

    static vector<pair<int, pair<int, double>>> getPFromE(int eid){  // get moving objects from edge
        if (movingObjectPtr->hsObj.find(eid) == movingObjectPtr->hsObj.end())  return vector<pair<int, pair<int, double>>>(0);
        vector<pair<int, pair<int, double>>> res;
        for(auto pid : movingObjectPtr->hsObj[eid]) { res.push_back(pair<int, pair<int, double>>(pid, movingObjectPtr->read(pid)));}
        return res;
    }

    static void eraseP(int pid) {
        pair<int, double > p = movingObjectPtr->read(pid);
        if (p.first != INT_MAX)
            movingObjectPtr->hsObj[movingObjectPtr->read(pid).first].erase(pid), movingObjectPtr->erase(pid);
    }

    static void updateP(int pid, int eid, double pos){
        eraseP(pid);
        if (eid != -1) insertP(pid, eid, pos);
    }

};

MovingObject* MovingObject::movingObjectPtr = NULL;

#endif // __MOVINGOBJECT
