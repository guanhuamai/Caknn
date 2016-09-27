#include <stdlib.h>
#include <stddef.h>
#ifndef EDGEOBJ_H_INCLUDED
#define EDGEOBJ_H_INCLUDED


/*
EdgeObj is not edge, but object adhering on edge

*/

class EdgeObj {//object on edge, landmark and moving obj

public:
//location information:
    size_t id;
    size_t snid;         //start node id
    size_t enid;         //end node id
    size_t eid;          //edge id
    double dist2sn;   //distance to start node
    double dist2en;   //distance to start node


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



#endif // EDGEOBJ_H_INCLUDED
