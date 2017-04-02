//
// Created by mgh on 4/1/17.
//

#ifndef CAKNNSR_MAD_H
#define CAKNNSR_MAD_H

#include <unordered_map>
using namespace std;


class MAD{  //singleton
private:
    unordered_map<int, double> edgeLowbounds;

    MAD(){}

    static MAD* madPtr;

public:

    static void buildMAD(){
        destructMAD();
        madPtr = new MAD();
    }

    static void destructMAD(){
        if (madPtr != NULL)
            delete madPtr, madPtr = NULL;
    }

    static void insert(int eid, double aggLowbound){
        madPtr->edgeLowbounds[eid] = aggLowbound;
    }

    static double getLowbound(int eid){
        if (madPtr->edgeLowbounds.find(eid) != madPtr->edgeLowbounds.end())
            return madPtr->edgeLowbounds[eid];
        return 0;  // 0 will prune nothing which can elegantly stop pruning
    }


};

MAD* MAD::madPtr = NULL;

#endif //CAKNNSR_MAD_H
