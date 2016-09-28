#include "AggFunc.h"




bool AggFunc::addRslts(std::vector<MovingObjItem> movingObjItems){
    this->rslts.insert(movingObjItems.begin(), movingObjItems.end());
    if(this->rslts.size() > k){
        return true;
    }
    return false;
}


void AggFunc::movObjsShift(std::vector< MovingObj > shiftObjs){
    std::vector<size_t> leafObjs;
    leafObjs.reserve(shiftObjs.size());
    for(size_t i = 0; i < shiftObjs.size(); i++){
        leafObjs.push_back(shiftObjs[i].id);
    }
    this->movObjsLeave(leafObjs, false);
    this->movObjsReach(shiftObjs);
}
