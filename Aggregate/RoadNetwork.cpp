#include "RoadNetwork.h"



size_t Edge::getOppositeNode(size_t nid){
    if(nid == snid){
        return enid;
    }
    return snid;
}

bool Edge::pasteMovObjs(std::vector<MovingObj> movOjbs){

    for(size_t i = 0; i < movOjbs.size(); i++){
        if(movObjsID.find(movOjbs[i].id) == movObjsID.end()){
            movObjsID.insert(movOjbs[i].id);
        }else{
            return false;
        }
    }
    return true;
}

bool Edge::eraseMovObjs(std::vector<MovingObj> movOjbs){
    for(size_t i = 0; i < movOjbs.size(); i++){
        std::set<size_t>::iterator itr = this->movObjsID.find(movOjbs[i].id);
        if(itr != this->movObjsID.end()){
            this->movObjsID.erase(itr);
        }else{
            return false;
        }
    }
    return true;
}


RoadNetwork::RoadNetwork(char* filePrefix){
    char* nodePath = new char[100];
    char* edgePath = new char[100];
    strcpy(nodePath, filePrefix);
    strcpy(edgePath, filePrefix);
    strcat(nodePath, ".cedge");
    strcat(edgePath, ".cnode");

    FILE* np = fopen(nodePath, "r");
    size_t nid;
    double lon;
    double lat;
    fscanf(np, "%zu", &nodenum);
    this->nodes = new Node[nodenum];
    while(fscanf(fp, "%zu %lf %lf", &nid, &lon, &lat) == 3){
        this->nodes[nid].lon = lon;
        this->nodes[nid].lat = lat;
    }
    fclose(np);

    FILE* ep = fopen(edgePath, "r");
    size_t eid;
    size_t snid;
    size_t enid;
    double elen;
    fscanf(ep, "%zu", &edgenum);
    this->edges = new Edge[edgenum];
    while(fscanf(fp, "%zu %zu %zu %lf", &eid, &snid, &enid, &elen) == 4){
        this->edges[eid].snid = snid;
        this->edges[eid].enid = enid;
        this->edges[eid].edgeLen = elen;
        this->nodes[snid].adjacentEdge.push_back(eid);
        this->nodes[enid].adjacentEdge.push_back(eid);
    }
    fclose(ep);

    for(size_t i = 0; i < nodenum; i++){//remove duplicate edges in node
        std::sort(this->nodes[i].adjacentEdge.begin(), this->nodes[i].adjacentEdge.end());
        std::vector<size_t>::iterator itr = this->nodes[i].adjacentEdge.begin();
        while(itr != this->nodes[i].adjacentEdge.end()){
            std::vector<size_t>::iterator itrNext = itr++;
            itr--;
            if(*itr == *itrNext){
                itr = this->nodes[i].adjacentEdge.erase(itr);
            }else{
                itr++;
            }
        }
    }

    delete[] nodePath;
    delete[] edgePath;
}

RoadNetwork::~RoadNetwork(){

    this->edgenum = 0;
    this->nodenum = 0;
    delete[] this->edges;
    delete[] this->nodes;


    int readNodesFromFile(char* nodePath);
    int readEdgesFromFile(char* edgePath);



}
