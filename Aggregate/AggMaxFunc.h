#ifndef AGGMAXFUNC_H_INCLUDED
#define AGGMAXFUNC_H_INCLUDED

#include "AggFunc.h"


class AggMaxFunc: public AggFunc{

public:



    void expandLmrkByPace(Landmark &lmrk) override{

        NodeItem ndItem = lmrk.pq.top();
        double oldRadius = radius;
        radius += pace;
        while(std::abs(ndItem.dist - radius) > DBL_ERROR && ndItem.dist < radius){
            lmrk.pq.pop();
            Node &nd = roadnw.nodes[ndItem.nid];
            for(size_t i = 0; i < nd.adjacentEdge.size(); i++){
                size_t adjEdgeID = nd.adjacentEdge[i];
                Edge &edge = roadnw.edges[adjEdgeID];
                for(std::set<size_t>::iterator itr = edge.movObjsID.begin(); itr != edge.movObjsID.end(); itr++){//calculate distance for moving obj in (oldRadius, radius]
                    MovingObj& mObj = movingObjs[*itr];
                    double dist = lmrk.getMovObjDist(mObj);
                    if(std::abs(dist - oldRadius) > DBL_ERROR  && dist > oldRadius &&
                        (std::abs(dist - radius) < DBL_ERROR && dist < radius)){
                        mObj.numReachLmrk ++;
                        double &aggValue = mObj.aggValue;
                        aggValue = aggValue > dist ? aggValue : dist;
                        if( mObj.numReachLmrk == lmrks.size()){
                            rslts.insert(MovingObjItem(mObj.id, mObj.aggValue));
                        }
                    }
                }

                if(!lmrk.isEdgeVisit[adjEdgeID]){
                    lmrk.isEdgeVisit[adjEdgeID] = true;
                    size_t nextNode = roadnw.edges[adjEdgeID].getOppositeNode(ndItem.nid);
                    double nextDist = roadnw.edges[adjEdgeID].edgeLen + ndItem.dist;
                    lmrk.pq.push(NodeItem(nextNode, nextDist));
                }
            }
        }
    }

    void movObjsReach(std::vector<MovingObj> reachObjs) override {
        std::vector<MovingObjItem> mObjItems;
        mObjItems.reserve(reachObjs.size());
        for(size_t i = 0; i < reachObjs.size(); i++){
            for(size_t j = 0; j < lmrks.size(); j++){
                Landmark lmrk = lmrks[j];
                double dist = lmrk.getMovObjDist(reachObjs[i]);
                if(dist == DBL_MAX) continue;
                reachObjs[i].numReachLmrk ++;
                reachObjs[i].aggValue = reachObjs[i].aggValue > dist ? reachObjs[i].aggValue : dist;
            }

            roadnw.edges[reachObjs[i].eid].movObjsID.insert(reachObjs[i].id);   //add moving object id to edges
            movingObjs[reachObjs[i].id] = reachObjs[i];                         //save moving object
            if(reachObjs[i].numReachLmrk == lmrks.size()){                      //if the aggregate value is calculated successfully, add it to rslts
                MovingObjItem mObjItem;
                mObjItem.oid = reachObjs[i].id;
                mObjItem.aggValue = reachObjs[i].aggValue;
                mObjItems.push_back(mObjItem);
            }
        }
        if(!addRslts(mObjItems)){//after adding the moving objects to graph, rslts size
            for(size_t i = 0; i < lmrks.size(); i++){
                expandLmrkByPace(lmrks[i]);
            }
        }
    }
};





#endif // AGGMAXFUNC_H_INCLUDED
