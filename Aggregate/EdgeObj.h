#ifndef EDGEOBJ_H_INCLUDED
#define EDGEOBJ_H_INCLUDED


/*
EdgeObj is not edge, but object adhering on edge

*/

class EdgeObj {//object on edge, landmark and moving obj

public:
//location information:
    int id;
    int snid;         //start node id
    int enid;         //end node id
    int eid;          //edge id
    double dist2sn;   //distance to start node
    double dist2en;   //distance to start node


    EdgeObj(){
        this->id = -1;
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
