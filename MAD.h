//
// Created by mgh on 4/1/17.
//

#ifndef CAKNNSR_MAD_H
#define CAKNNSR_MAD_H

#include <vector>
using namespace std;


class MAD{  //singleton
private:
    vector<double> edgeLowbounds;
    unsigned int numOfEdges;

    MAD(unsigned numOfEdges): numOfEdges(numOfEdges){
        edgeLowbounds = vector<double>(numOfEdges, 0);
    }

    static MAD* madPtr;

public:

    static void buildMAD(unsigned numOfEdges){
        destructMAD();
        madPtr = new MAD(numOfEdges);
    }

    static void destructMAD(){
        if (madPtr != NULL)
            delete madPtr, madPtr = NULL;
    }

    static void insert(int eid, double aggLowbound){
        madPtr->edgeLowbounds[eid] = aggLowbound;
    }

    static double getLowbound(int eid){
        if (eid < 0 || eid >= madPtr->numOfEdges) return 0;
        return madPtr->edgeLowbounds[eid];  // 0 will prune nothing which can elegantly stop pruning
    }

    static void display(){
        cout << "-----------------display MAD ---------------" << endl;
        cout << "-----------------number of edges------------" << endl;
        int i = 0;
        for (const auto& e: madPtr->edgeLowbounds){
            if (e != 0)
                cout << "edge: " << i++ << " min distance: "<< e << endl;
        }
        cout << "-----------------display MAD over---------------" << endl;

    }


};

MAD* MAD::madPtr = NULL;

#endif //CAKNNSR_MAD_H
