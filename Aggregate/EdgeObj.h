#ifndef EDGEOBJ_H_INCLUDED
#define EDGEOBJ_H_INCLUDED


class EdgeObj {//object on edge, landmark and moving obj

public:
//location information:
    int id;
    int snid;         //start node id
    int enid;         //end node id
    int eid;          //edge id
    double dist2sn;   //distance to start node
    double dist2en;   //distance to start node


    bool operator < (const EdgeObj& objItem) const{
        return id < objItem.id;
    }


};



#endif // EDGEOBJ_H_INCLUDED
