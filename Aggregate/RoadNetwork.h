#include <set>
#include "MovingObj.h"
#include "Landmark.h"


#ifndef ROADNETWORK_H_INCLUDED
#define ROADNETWORK_H_INCLUDED




class Edge{

public:
    int snid;
    int enid;
    double edgeLen;

    std::set<int> movObjsID;
    std::set<int> lmrksID;

    bool pasteMovObjs(std::vector<MovingObj> movObjs);
    bool eraseMovObjs(std::vector<MovingObj> movObjs);
};

class Node{
    std::vector<Edge> adjacentEdge;
};

class RoadNetwork{

    Edge* edges;
    int edgenum;

    Node* nodes;
    int nodenum;

    virtual ~RoadNetwork();
};

RoadNetwork::~RoadNetwork(){

    this->edgenum = 0;
    this->nodenum = 0;
    delete[] this->edges;
    delete[] this->nodes;


    int readNodesFromFile(char* nodePath);
    int readEdgesFromFile(char* edgePath);



}



#endif // ROADNETWORK_H_INCLUDED
