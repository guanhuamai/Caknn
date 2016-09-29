#include <set>
#include "MovingObj.h"
#include "Landmark.h"


#ifndef ROADNETWORK_H_INCLUDED
#define ROADNETWORK_H_INCLUDED




class Edge{

public:
    size_t snid;
    size_t enid;
    double edgeLen;

    std::set<size_t> movObjsID;
    std::set<size_t> lmrksID;

    bool pasteMovObjs(std::vector<MovingObj> movObjs);
    bool eraseMovObjs(std::vector<MovingObj> movObjs);
    size_t  getOppositeNode(size_t nid);
};

class Node{

public:
    std::vector<size_t> adjacentEdge;
};

class RoadNetwork{


public:
    Edge* edges;
    size_t edgenum;

    Node* nodes;
    size_t nodenum;

    virtual ~RoadNetwork();
};



#endif // ROADNETWORK_H_INCLUDED
