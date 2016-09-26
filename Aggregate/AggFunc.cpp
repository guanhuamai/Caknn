#include "AggFunc.h"




bool AggFunc::maintainKNN(std::vector<MovingObjItem> movingObjItems, bool isDelete){

    if(isDelete){


    }else{
        std::set<int> deleteS;
        for(int i = 0; i < movingObjItems.size(); i++){
            if(movItemRank[movingObjItems[i].oid] != NULL){

            }
        }

        this->rslts.insert(rslts.back(), movingObjItems.begin(), movingObjItems.end());
    }

    std::make_heap(knn.begin(), knn.end());
    while(knn.size() > k){
        std::pop_heap(knn.begin(), knn.end());
        knn.pop_back();
    }
    if(knn.size() == k){
        return true;
    }
    return false;
}
