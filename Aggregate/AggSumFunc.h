#ifndef AGGSUMFUNC_H_INCLUDED
#define AGGSUMFUNC_H_INCLUDED


#include "AggFunc.h"



class AggSumFunc: public AggFunc{

public:

    bool isExpand(){
        std::set<size_t> ndSet;
        if(rslts.size() >= k){
            double kAgg = aggBound();
            for(size_t i = 0; i < lmrks.size(); i++){
                Landmark& lmrk = lmrks[i];
                for(size_t j = 0; j < lmrk.ndHeap.size(); j ++){
                    double lb = lowBound(lmrk.ndHeap[j].nid, lmrk);
                    //found one expansion may create the knn result
                    if( std::abs(lb - kAgg) <= DBL_ERROR || lb < kAgg){
                        return false;
                    }
                }
            }
        }else{
            return false;
        }
        return true;
    }

    double lowBound(NodeItem& nItm, Landmark& lmrk){
        double lb = 0;

        //
        size_t nid = nItm.nid;
        double dist = nItm.dist;
        Node& nd = roadnw.nodes[nid];
        for(size_t i = 0; i < nd.adjacentEdge.size(); i++){
            size_t eid = nd.adjacentEdge[i];
            if(lmrk.isEdgeVisit[eid]){
                Edge& edge = roadnw.edges[eid];
                dist += edge.edgeLen;
            }
        }
        lb += dist;
        for(size_t i = 0; i < lmrks.size(); i++){
            Landmark& othLmrk = lmrks[i];
            if(othLmrk.id == lmrk.id)  continue;
            double distLB = othLmrk.findDist(nid);
            distLB = distLB < 0 ? radius : distLB;
            lb += distLB;
        }
        return lb;
    }

    double aggUpdate(double aggValue, double dist){
        return aggValue += dist;
    }


};


#endif // AGGSUMFUNC_H_INCLUDED
