#include "AggFunc.h"


void AggFunc::readLmrks(char* filePrefix){
    FILE* fp = fopen(filePrefix, "r");
    size_t snid;
    size_t enid;
    size_t  eid;
    double len1;
    double len2;
    while(fscanf(fp, "%zu %zu %zu %lf %lf", &snid, &enid, &eid, &len1, &len2) == 4){
        this->lmrks.push_back(Landmark(snid, enid, eid, len1, len2));
    }
}


bool AggFunc::addRslts(std::vector<MovingObjItem> movingObjItems){
    this->rslts.insert(movingObjItems.begin(), movingObjItems.end());
    if(this->rslts.size() > k){
        return true;
    }
    return false;
}

void AggFunc::movObjsShift(std::vector< MovingObj > shiftObjs){
    std::vector<size_t> leafObjs;
    leafObjs.reserve(shiftObjs.size());
    for(size_t i = 0; i < shiftObjs.size(); i++){
        leafObjs.push_back(shiftObjs[i].id);
    }
    this->movObjsLeave(leafObjs, false);
    this->movObjsReach(shiftObjs);
}

void AggFunc::expandLmrkByPace(Landmark &lmrk) {

    NodeItem ndItem = lmrk.ndHeap[0];
    double oldRadius = radius;
    double newRadius = radius + pace;
    while(std::abs(ndItem.dist - newRadius) > DBL_ERROR && ndItem.dist < newRadius){
        std::pop_heap(lmrk.ndHeap.begin(), lmrk.ndHeap.end());

        lmrk.ndHeap.pop_back();
        Node &nd = roadnw.nodes[ndItem.nid];

        for(size_t i = 0; i < nd.adjacentEdge.size(); i++){
            size_t adjEdgeID = nd.adjacentEdge[i];
            Edge &edge = roadnw.edges[adjEdgeID];
            //calculate distance for moving obj in (oldRadius, newRadius]
            for(std::set<size_t>::iterator itr = edge.movObjsID.begin(); itr != edge.movObjsID.end(); itr++){
                MovingObj& mObj = movingObjs[*itr];
                double dist = lmrk.getMovObjDist(mObj);
                if(std::abs(dist - oldRadius) > DBL_ERROR  && dist > oldRadius &&
                    (std::abs(dist - newRadius) < DBL_ERROR && dist < newRadius)){
                    mObj.numReachLmrk ++;
                    double &aggValue = mObj.aggValue;
                    aggValue = aggUpdate(aggValue, dist);
                    if( mObj.numReachLmrk == lmrks.size()){
                        rslts.insert(MovingObjItem(mObj.id, mObj.aggValue));
                    }
                }
            }

            if(!lmrk.isEdgeVisit[adjEdgeID]){//dijks expand unvisit edges
                lmrk.isEdgeVisit[adjEdgeID] = true;
                size_t nextNodeID = roadnw.edges[adjEdgeID].getOppositeNode(ndItem.nid);
                double nextDist = roadnw.edges[adjEdgeID].edgeLen + ndItem.dist;
                for(size_t i = 0; i < lmrk.ndHeap.size(); i++){
                    if(lmrk.ndHeap[i].nid == nextNodeID){
                        if(std::abs(nextDist - lmrk.ndHeap[i].dist) > DBL_ERROR && nextDist < lmrk.ndHeap[i].dist){
                            lmrk.ndHeap[i].dist = nextDist;
                            std::make_heap(lmrk.ndHeap.begin(), lmrk.ndHeap.end());
                            break;
                        }
                    }
                    else if(i == lmrk.ndHeap.size() - 1){
                        lmrk.ndHeap.push_back(NodeItem(nextNodeID, nextDist));
                        std::push_heap(lmrk.ndHeap.begin(), lmrk.ndHeap.end());

                    }
                }
            }
        }
    }
}

void AggFunc::movObjsReach(std::vector<MovingObj> reachObjs) {

    std::vector<MovingObjItem> mObjItems;
    mObjItems.reserve(reachObjs.size());
    for(size_t i = 0; i < reachObjs.size(); i++){
        for(size_t j = 0; j < lmrks.size(); j++){
            Landmark lmrk = lmrks[j];
            double dist = lmrk.getMovObjDist(reachObjs[i]);
            if(dist == DBL_MAX) continue;
            reachObjs[i].numReachLmrk ++;
            reachObjs[i].aggValue = aggUpdate(reachObjs[i].aggValue, dist);
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
        while(isExpand()){
            for(size_t i = 0; i < lmrks.size(); i++){
                expandLmrkByPace(lmrks[i]);
            }
            radius += pace;
        }

    }
}

void AggFunc::movObjsLeave(std::vector< size_t > leaveObjs, bool reachEnd){
    for(size_t i = 0; i < leaveObjs.size(); i++){
        size_t id = leaveObjs[i];
        double aggValue = movingObjs[id].aggValue;
        rslts.erase(MovingObjItem(id, aggValue));
        movingObjs.erase(leaveObjs[i]);
    }
    if(!reachEnd){
        while(isExpand()){
            for(size_t i = 0; i < lmrks.size(); i++){
                expandLmrkByPace(lmrks[i]);
            }
            radius += pace;
        }
    }
}

double AggFunc::aggBound(){
    if(rslts.size() >= k){
        size_t i = 0;
        std::set<MovingObjItem>::iterator itr = rslts.begin();
        while(i < k)  itr++;
        return (*itr).aggValue;
    }
    return 0;
}

double AggSumFunc::lowBound(NodeItem& nItm, Landmark& lmrk){
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

bool   AggSumFunc::isExpand(){
    std::set<size_t> ndSet;
    if(rslts.size() >= k){
        double kAgg = aggBound();
        for(size_t i = 0; i < lmrks.size(); i++){
            Landmark& lmrk = lmrks[i];
            for(size_t j = 0; j < lmrk.ndHeap.size(); j ++){
                double lb = this->lowBound(lmrk.ndHeap[j], lmrk);
                if( std::abs(lb - kAgg) <= DBL_ERROR || lb < kAgg){//found one expansion may create the knn result
                    return false;
                }
            }
        }
    }else{
        return false;
    }
    return true;
}

double AggSumFunc::aggUpdate(double aggValue, double dist){
    return aggValue += dist;
}
