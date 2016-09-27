#include "AggFunc.h"




bool AggFunc::addRslts(std::vector<MovingObjItem> movingObjItems){
    this->rslts.insert(movingObjItems.begin(), movingObjItems.end());
    if(this->rslts.size() > k){
        return true;
    }
    return false;
}



