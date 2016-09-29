#ifndef AGGFUNC_H_INCLUDED
#define AGGFUNC_H_INCLUDED

#include <vector>
#include <set>
#include <algorithm>
#include <unordered_map>


#include "DistMatrix.h"
#include "Utility.h"
#include "RoadNetwork.h"
#include "EdgeObj.h"


class AggFunc{

public:
    char* filePrefix;
    size_t k;                                          //k
    std::set< MovingObjItem > rslts;                //resul of moving object, sorted by aggregate value in decreasing order

    RoadNetwork roadnw;
    std::unordered_map<size_t, MovingObj> movingObjs;               //save in set, for delete and append efficiently
    //double radius;                                                //the update distance at each moving object update iteration


    std::vector<Landmark> lmrks;
    double radius;
    double pace;

//stop consition for single update for a landmark
//input: landmark id
    virtual bool isExpand() = 0;
    virtual double aggUpdate(double aggValue, double dist) = 0;
    virtual double aggBound();//return aggregate value bound at kth results
    virtual bool addRslts(std::vector< MovingObjItem > movingObjItems);
    virtual void readLmrks(char* filePrefix);

//moving objects's change of status
//input: affected moving objects
    //append these objects from the 'movingObjs', update the 'rslts' and construct further matrix index
    virtual void movObjsReach(std::vector< MovingObj > reachObjs);
    //update these objects from the 'movingObjs', update the 'rslts' and construct further matrix index
    virtual void movObjsShift(std::vector< MovingObj > shiftObjs);
    //remove these objects from the 'movingObjs', and update the 'rslts'
    virtual void movObjsLeave(std::vector< size_t > leaveObjs, bool reachEnd);

    virtual void expandLmrkByPace(Landmark &lmrk);
};


class AggMaxFunc: public AggFunc{

public:
    double aggUpdate(double aggValue, double dist) override {
        return aggValue > dist ? aggValue : dist;
    }
};


class AggMinFunc: public AggFunc{

public:
    double aggUpdate(double aggValue, double dist) override {
        return aggValue < dist ? aggValue : dist;
    }
};


class AggSumFunc: public AggFunc{

public:
    bool isExpand() override;
    double aggUpdate(double aggValue, double dist) override;

    double lowBound(NodeItem& nItm, Landmark& lmrk);
};

#endif // AGGFUNC_H_INCLUDED
