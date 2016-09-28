#ifndef AGGMAXFUNC_H_INCLUDED
#define AGGMAXFUNC_H_INCLUDED

#include "AggFunc.h"

class AggMaxFunc: public AggFunc{

public:

    bool isExpand() override{
        return rslts.size() >= k;
    }


    double aggUpdate(double aggValue, double dist){
        return aggValue > dist ? aggValue : dist;
    }


};





#endif // AGGMAXFUNC_H_INCLUDED
