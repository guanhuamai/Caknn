#ifndef MOVINGOBJ_H_INCLUDED
#define MOVINGOBJ_H_INCLUDED

#include "EdgeObj.h"
#include <vector>



class MovingObj : public EdgeObj{



public:
//aggregate information:

    size_t numReachLmrk;              //log the number of landmark reached by moving object, -1: no need to search, num_lmark: aggValue has been generated
    double aggValue;                //aggregate value to all landmarks
    double speed;                   //a distance that the moving obj will move at next time stamp
    //unsigned long timestamp;        //time stamp of the moving object

    std::vector<MovingObj> readMobjFromFile(char* mobjPath);






};


#endif // MOVINGOBJ_H_INCLUDED
