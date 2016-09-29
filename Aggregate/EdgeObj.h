#ifndef EDGEOBJ_H_INCLUDED
#define EDGEOBJ_H_INCLUDED
#include <vector>
#include <stdlib.h>
#include <stddef.h>
#include "Utility.h"
#include "DistMatrix.h"


/**EdgeObj is not edge, but object adhering on edge*/

class EdgeObj {//object on edge, landmark and moving obj

public:
//location information:
    size_t id;
    size_t snid;         //start node id
    size_t enid;         //end node id
    size_t eid;          //edge id
    double dist2sn;   //distance to start node
    double dist2en;   //distance to start node

    EdgeObj(size_t id, size_t snid, size_t enid, size_t eid, double dist2sn, double dist2en)
        :id(id), snid(snid), enid(enid), eid(eid), dist2sn(dist2sn), dist2en(dist2en){}

    EdgeObj(){
        this->id = (size_t) -1;
    }

    bool operator < (const EdgeObj& objItem) const{
        return id < objItem.id;
    }

    bool operator == (const EdgeObj &mobj){
        if(mobj.id == this->id){
            return true;
        }
        return false;
    }
};


class MovingObj : public EdgeObj{
public:
//aggregate information:
    size_t numReachLmrk;              //log the number of landmark reached by moving object, -1: no need to search, num_lmark: aggValue has been generated
    double aggValue;                //aggregate value to all landmarks
    double speed;                   //a distance that the moving obj will move at next time stamp
    //unsigned long timestamp;        //time stamp of the moving object

    std::vector<MovingObj> readMobjFromFile(char* mobjPath);

};

class Landmark : public EdgeObj{

public:
//aggregate information:

    Landmark(): EdgeObj(){};
    Landmark(size_t id, size_t snid, size_t enid, size_t eid, double dist2sn, double dist2en): EdgeObj(id, snid, enid, eid, dist2sn, dist2en){};
    //Landmark(size_t snid, size_t enid, size_t eid, double dist2sn, double dist2en){};

    std::vector<NodeItem> ndHeap;
    DistMatrix* dm;
    bool *isEdgeVisit;

    double findDist(size_t nodeID);//lmrk id is in super class
    void  writeDist(size_t nodeID, double dist);
    double getMovObjDist(MovingObj &mobj);
};
#endif // EDGEOBJ_H_INCLUDED
