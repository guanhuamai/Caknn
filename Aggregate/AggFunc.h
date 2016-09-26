#ifndef AGGFUNC_H_INCLUDED
#define AGGFUNC_H_INCLUDED

#include <limits>
#include <vector>
#include <set>
#include <algorithm>
#include <unordered_map>

#include "Landmark.h"
#include "MovingObj.h"
#include "DistMatrix.h"
#include "Utility.h"
#include "RoadNetwork.h"


#define DBL_MAX std::numeric_limits<double>::max()

class AggFunc{



public:
    char* filePrefix;
    int k;                                   //k
    std::set< MovingObjItem > rslts;   //resul of moving object, sorted by aggregate value in decreasing order


    RoadNetwork roadnw;
    std::unordered_map<int, MovingObj> movingObjs;               //save in set, for delete and append efficiently
    //double radius;                       //the update distance at each moving object update iteration


    std::vector<Landmark> lmrks;


    virtual bool initRdAndLmrk(char* fileprefix);

//stop consition for single update for a landmark
//input: landmark id
    virtual bool isStopSearch(int lid) = 0;



//moving objects's change of status
//input: affected moving objects

    //append these objects from the 'movingObjs', update the 'rslts' and construct further matrix index
    virtual bool movObjsReach(std::vector< MovingObj > reachObjs);
    //update these objects from the 'movingObjs', update the 'rslts' and construct further matrix index
    virtual bool movObjsShift(std::vector< MovingObj > shiftObjs);
    //remove these objects from the 'movingObjs', and update the 'rslts'
    virtual bool movObjsLeave(std::vector< MovingObj > leaveObjs);


    virtual std::vector<MovingObjItem> expandLmrkByPace(Landmark lmrk);
    virtual bool maintainRslts(std::vector< MovingObjItem > movingObjItems, bool isDelete);


};





#endif // AGGFUNC_H_INCLUDED
