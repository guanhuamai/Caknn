#ifndef LANDMARK_H_INCLUDED
#define LANDMARK_H_INCLUDED


#include <queue>
#include "EdgeObj.h"
#include "Utility.h"
#include "DistMatrix.h"

class Landmark : public EdgeObj{

public:
//aggregate information:
    std::priority_queue<NodeItem> pq;


    int readLmrksFromFile(char* lmrkPath);
    double findDist(int nodeID);//lmrk id is in super class
    bool writeDist(int nodeID);

};

#endif // LANDMARK_H_INCLUDED
