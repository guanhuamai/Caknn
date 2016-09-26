#ifndef AGGMAXFUNC_H_INCLUDED
#define AGGMAXFUNC_H_INCLUDED

#include "AggFunc.h"


class AggMaxFunc: public AggFunc{

public:



    std::vector<MovingObjItem> expandLmrkByPace(Landmark lmrk) override{



    }

    bool movObjsReach(std::vector<MovingObj> reachObjs) override {
        std::vector<MovingObjItem> mObjItems;
        mObjItems.reserve(reachObjs.size());
        for(int i = 0; i < reachObjs.size(); i++){
            for(int j = 0; j < lmrks.size(); j++){
                Landmark lmrk = lmrks[j];
                double dist = DBL_MAX;
                double dist1 = lmrk.findDist(reachObjs[i].snid);
                double dist2 = lmrk.findDist(reachObjs[i].enid);

                if(dist1 < 0 && dist2 < 0)  continue;//prune if is located out side of range

                dist1 = dist1 > 0 ? dist1 + reachObjs[i].dist2sn : DBL_MAX;
                dist2 = dist2 > 0 ? dist2 + reachObjs[i].dist2en : DBL_MAX;
                dist = dist1 < dist2 ? dist1 : dist2;

                reachObjs[i].numReachLmrk ++;
                reachObjs[i].aggValue = reachObjs[i].aggValue > dist ? reachObjs[i].aggValue : dist;
            }

            movingObjs[reachObjs[i].id] = reachObjs[i];
            if(reachObjs[i].numReachLmrk == lmrks.size()){
                MovingObjItem mObjItem;
                mObjItem.oid = reachObjs[i].id;
                mObjItem.aggValue = reachObjs[i].aggValue;
                mObjItems.push_back(mObjItem);
            }
        }

        while(!this->maintainKNN(mObjItems) && this->isStopSearch()){
            for(int i = 0; i < lmrks.size(); i++){
                mObjItems = this->expandLmrkByPace(lmrks[i]);//report moving objects that are found finishing calculating
                this->maintainKNN(mObjItems);
            }
        }
    }
};





#endif // AGGMAXFUNC_H_INCLUDED
