#ifndef AGGMAXFUNC_H_INCLUDED
#define AGGMAXFUNC_H_INCLUDED

#include "AggFunc.h"


class AggMaxFunc: public AggFunc{

public:

    bool movObjsReach(std::vector<MovingObj> reachObjs) override {

        for(int i = 0; i < lmrks.size(); i++){//shorten the range
            Landmark lmrk = lmrks[i];
            for(int j = 0; j < reachObjs.size(); j++){
                double dist = DBL_MAX;
                double dist1 = lmrk.findDist(reachObjs[j].snid);
                double dist2 = lmrk.findDist(reachObjs[j].enid);

                if(dist1 < 0 && dist2 < 0)  continue;//prune if is located out side of range

                dist1 = dist1 > 0 ? dist1 + reachObjs[j].dist2sn : DBL_MAX;
                dist2 = dist2 > 0 ? dist2 + reachObjs[j].dist2en : DBL_MAX;
                dist = dist1 < dist2 ? dist1 : dist2;

                reachObjs[j].numReachLmrk ++;
                reachObjs[j].aggValue = reachObjs[j].aggValue > dist ? reachObjs[j].aggValue : dist;

                if(reachObjs[j].numReachLmrk == lmrks.size()){


                }
            }
        }
        movingObjs.insert(reachObjs.begin(), reachObjs.end());
    }
};





#endif // AGGMAXFUNC_H_INCLUDED
