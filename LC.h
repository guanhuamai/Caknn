//
// Created by mgh on 4/1/17.
//

#ifndef CAKNNSR_LC_H
#define CAKNNSR_LC_H

#include <vector>
#include <set>
using namespace std;

const enum LC_TYPE {X_LANDMARK, Y_LANDMARK, Z_LANDMARK};

class LC{
private:
    vector<set<int>> xLandmarks;
    vector<set<int>> yLandmarks;
    vector<set<int>> zLandmarks;

    LC(unsigned int numOfEdge){
        xLandmarks = std::vector<set<int>>(numOfEdge, std::set<int>());
        yLandmarks = std::vector<set<int>>(numOfEdge, std::set<int>());
        zLandmarks = std::vector<set<int>>(numOfEdge, std::set<int>());
    };


    static LC* lcPtr;

public:

    static void buildLC(unsigned int numOfEdges){
        lcPtr = new LC(numOfEdges);
    }

    static void insert(int eid, int lid, LC_TYPE lc_type) {
        if (lc_type == X_LANDMARK){
            lcPtr->xLandmarks[eid].insert(lid);
        } else if (lc_type == Y_LANDMARK){
            lcPtr->yLandmarks[eid].insert(lid);
        } else if (lc_type == Z_LANDMARK){
            lcPtr->zLandmarks[eid].insert(lid);
        }
    }

    static set<int> getLandmark(int eid, LC_TYPE lc_type){

        if (lc_type == X_LANDMARK){
            return lcPtr->xLandmarks[eid];
        } else if (lc_type == Y_LANDMARK){
            return lcPtr->yLandmarks[eid];
        } else if (lc_type == Z_LANDMARK){
            return lcPtr->zLandmarks[eid];
        } else{
            return set<int>();
        }
    }


};

#endif //CAKNNSR_LC_H
