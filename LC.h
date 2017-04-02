//
// Created by mgh on 4/1/17.
//

#ifndef CAKNNSR_LC_H
#define CAKNNSR_LC_H

#include <vector>
#include <set>
using namespace std;

enum LC_TYPE{X_LANDMARK = 0, Y_LANDMARK, Z_LANDMARK};

class LC{  // singleton

private:
    vector<set<int>> xLandmarks;
    vector<set<int>> yLandmarks;
    vector<set<int>> zLandmarks;

    LC(unsigned int numOfEdges, unsigned int numOfLandmarks)
            : numOfEdges(numOfEdges), numOfLandmarks(numOfLandmarks){
        xLandmarks = std::vector<set<int>>(numOfEdges, std::set<int>());
        yLandmarks = std::vector<set<int>>(numOfEdges, std::set<int>());
        zLandmarks = std::vector<set<int>>(numOfEdges, std::set<int>());
    };

    unsigned int numOfEdges;
    unsigned int numOfLandmarks;

    static LC* lcPtr;

public:

    static void buildLC(unsigned int numOfEdges, unsigned int numOfLandmarks){
        destructLC();
        lcPtr = new LC(numOfEdges, numOfLandmarks);
    }

    static void destructLC(){
        if (lcPtr != NULL)
            delete lcPtr, lcPtr = NULL;
    };

    static void insert(int eid, int lid, LC_TYPE lc_type) {
        if (lc_type == X_LANDMARK){
            lcPtr->xLandmarks[eid].insert(lid);
        } else if (lc_type == Y_LANDMARK){
            lcPtr->yLandmarks[eid].insert(lid);
        } else if (lc_type == Z_LANDMARK){
            lcPtr->zLandmarks[eid].insert(lid);
        }
    }

    static set<int>* getLandmark(int eid, LC_TYPE lc_type){

        if (lc_type == X_LANDMARK){
            return &(lcPtr->xLandmarks[eid]);
        } else if (lc_type == Y_LANDMARK){
            return &(lcPtr->yLandmarks[eid]);
        } else if (lc_type == Z_LANDMARK){
            return &(lcPtr->zLandmarks[eid]);
        } else{
            return NULL;
        }
    }

    static bool isEdgeClassified(int eid){
        if (eid < 0 || eid >= lcPtr->numOfEdges) return false;
        return lcPtr->xLandmarks[eid].size()
               + lcPtr->yLandmarks[eid].size()
               + lcPtr->zLandmarks[eid].size()
               == lcPtr->numOfLandmarks;
    }

    static void display(){
        for(int i = 0; i < lcPtr->numOfEdges; i++){
            for (const auto& e: lcPtr->xLandmarks[i])
                cout << "edge: " << i << "  xLandmark: " << e << endl;

            for (const auto& e: lcPtr->yLandmarks[i])
                cout << "edge: " << i << "  yLandmark: " << e << endl;

            for (const auto& e: lcPtr->zLandmarks[i])
                cout << "edge: " << i << "  zLandmark: " << e << endl;

        }
    }

};

LC* LC::lcPtr = NULL;

#endif //CAKNNSR_LC_H
