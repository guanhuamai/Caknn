#ifndef LANDMARK_H_INCLUDED
#define LANDMARK_H_INCLUDED


#include <queue>
#include "EdgeObj.h"
#include "MovingObj.h"
#include "Utility.h"
#include "DistMatrix.h"


class Landmark : public EdgeObj{

public:
//aggregate information:
    std::priority_queue<NodeItem> pq;
    bool *isEdgeVisit;


    int readLmrksFromFile(char* lmrkPath);
    double findDist(size_t nodeID);//lmrk id is in super class
    bool writeDist(size_t nodeID);
    double getMovObjDist(MovingObj mobj);




};

#endif // LANDMARK_H_INCLUDED
