#ifndef ROADNETWORK_H_INCLUDED
#define ROADNETWORK_H_INCLUDED

#include <vector>



class Edge{
    int snid;
    int enid;
    double edgeLen;
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
