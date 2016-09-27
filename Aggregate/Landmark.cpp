#include "Landmark.h"



double Landmark::findDist(size_t nodeID){return 0;}//lmrk id is in super class



double Landmark::getMovObjDist(MovingObj mobj){
    double dist = DBL_MAX;
    double dist1 = findDist(mobj.snid);
    double dist2 = findDist(mobj.enid);

    if(dist1 < 0 && dist2 < 0)  return dist;//prune if is located out side of range

    dist1 = dist1 > 0 ? dist1 + mobj.dist2sn : DBL_MAX;
    dist2 = dist2 > 0 ? dist2 + mobj.dist2en : DBL_MAX;
    dist = dist1 < dist2 ? dist1 : dist2;

    return dist;

}
