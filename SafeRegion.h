//
// Created by mgh on 3/24/17.
//

#ifndef CAKNNSR_SAFEREGION_H
#define CAKNNSR_SAFEREGION_H

#include "functional"
#include "MovingObject.h"
#include "PartialMatrix.h"
#include "Opf.h"
#include "Graph.h"
#include "Result.h"
#include "DistElement.h"
#include "BaseExpansion.h"
#include "LC.h"
#include "MAD.h"

using namespace std;

class SafeRegion{

private:
    SafeRegion(){r = DBL_MAX; dk = DBL_MAX;}

    double r;
    double dk;
    int nl; // number of landmarks

    int lcCnt;
    int madCnt;

    bool useLC; // determine whether to use LC to accelerate aggregate distance calculation
    bool useMAD; //determine whether to use MAD to prune moving objects from opf


    function<double (double, double)> agg;

    // no range at the beginning, generate the initial range
    static void forceUpdate(int mid, int eid, double pos);

    // set to 0. don't need to distinguish safe region and opf region
    static double velocity();
    static double getAggDist(int eid, double pos);

    static void updateLC(unordered_set<int>& expandedEdges);
    static void updateMAD(unordered_set<int>& expandedEdges);
    static void updateOpf(unordered_set<int>& expandedEdges);

    static double getAggDistByLC(int eid, double pos){  // return DBL_MAX if not work
        if (!sr->useLC || !LC::isEdgeClassified(eid)) return DBL_MAX;

        int snid = Graph::getEdgeStart(eid);
        int enid = Graph::getEdgeEnd(eid);
        double len = Graph::getEdgeLen(eid);

        set<int>* xLandmarks = LC::getLandmark(eid, X_LANDMARK);
        set<int>* yLandmarks = LC::getLandmark(eid, Y_LANDMARK);
        set<int>* zLandmarks = LC::getLandmark(eid, Z_LANDMARK);

        double res = sr->agg(1, 2) == 1 ? DBL_MAX : sr->agg(1, 2) == 2 ? DBL_MIN : 0;
        if (xLandmarks != NULL)
            for (const auto& e: *xLandmarks){
                res = sr->agg(res, PartialMatrix::getDist(snid, e) + pos);
            }

        if (yLandmarks != NULL)
            for (const auto& e: *yLandmarks){
                res = sr->agg(res, PartialMatrix::getDist(enid, e) + len - pos);
            }

        if (zLandmarks != NULL)
            for (const auto& e: *zLandmarks){
                double d = min(PartialMatrix::getDist(snid, e) + pos,
                               PartialMatrix::getDist(enid, e) + len - pos);
                if (eid == Graph::getLmrkById(e).first){
                    d = min(d, fabs(pos - len));
                }
                res = sr->agg(res, d);
            }
        sr->lcCnt ++;

        return res;
    }

public:


    static SafeRegion* sr;

    static void buildSafeRegion(int nl, function<double (double, double)> agg,
                                bool useLC, bool useMAD){
        destructSafeRegion();

        sr = new SafeRegion();

        sr->lcCnt = 0;
        sr->madCnt = 0;
        sr->agg = agg;
        sr->nl = nl;
        sr->useLC = useLC;
        sr->useMAD = useMAD;
    }


    static void destructSafeRegion(){
        if (sr != NULL){
            delete(sr);
            sr = NULL;
        }
    }

    static int getlcCnt(){return sr->lcCnt;}

    static int getmadCnt(){return sr->madCnt;}

    static void update(int mid, int eid, double pos);

    static int expand(BaseExpansion* expansion){  // return the number of expanded edges
        unordered_set<int> expandedEdges = expansion->expand(sr->r);
        if (sr->useLC) updateLC(expandedEdges);
        if (sr->useMAD) updateMAD(expandedEdges);

        updateOpf(expandedEdges);

//        if (expandedEdges.size() != 0)
//            expansion->display();
//        PartialMatrix::display();

        return (unsigned) expandedEdges.size();
    }
};

SafeRegion* SafeRegion::sr = NULL;

double SafeRegion::velocity() {return 0;}

double SafeRegion::getAggDist(int eid, double pos) {
    double lcRes = getAggDistByLC(eid, pos);
    if (lcRes != DBL_MAX) return lcRes;  // calculate distance successfully with lc

    double res = sr->agg(1, 2) == 1 ? DBL_MAX : sr->agg(1, 2) == 2 ? DBL_MIN : 0;

    int snid(Graph::getEdgeStart(eid)), enid(Graph::getEdgeEnd(eid));
    for (int i = 0; i < sr->nl; i++){
        double ds = PartialMatrix::getDist(snid, i);
        double de = PartialMatrix::getDist(enid, i);

        double d = DBL_MAX;

        if (ds != DBL_MAX) d = min(d, ds + pos);
        if (de != DBL_MAX) d = min(d, de + Graph::getEdgeLen(eid) - pos);

        if (eid == Graph::getLmrkById(i).first)
            d = min(d, fabs(pos - Graph::getEdgeLen(eid)));

        res = sr->agg(res, d);
    }

    return res;
}

void SafeRegion::update(int mid, int eid, double pos) {
    MovingObject::updateP(mid, eid, pos);  // update in moving object database unconditionally...

    if (eid != -1 && sr->dk == DBL_MAX) {  // normal object and range not expanded...
        forceUpdate(mid, eid, pos);
    }else {

        if (MAD::getLowbound(eid) > sr->r) {  // MAD pruning
//        if(0 > sr->r){
            sr->madCnt ++;
            Opf::erase(mid);
            if (Result::exist(mid)) {
                vector<pair<int, double>> kOpfs = Opf::getNSmallest(Result::getK());
                Result::rebuild(kOpfs);
            }
        }else {

            double di = getAggDist(eid, pos);

            if (di < sr->r) Opf::insert(mid, di);  // update opf according to new aggregate distance
            else Opf::erase(mid);

            if (Result::exist(mid) || di < sr->dk) { // rebuild if the moving object is in the result.
                vector<pair<int, double>> kOpfs = Opf::getNSmallest(Result::getK());
                Result::rebuild(kOpfs);
            }
        }
    }

    sr->dk = Result::getKthDist();
    if (sr->dk != DBL_MAX) {
        if (sr->r != DBL_MAX)
            sr->r = max(sr->r, sr->dk + velocity());
        else sr->r = sr->dk + velocity();
    }

}

void SafeRegion::forceUpdate(int mid, int eid, double pos) {

    printf("force update %d %d %lf\n", mid, eid, pos);

    if (eid == -1) return;

    DistElements h;

    h.push(Graph::getEdgeStart(eid), NODE, mid, MOVING_OBJECT, pos);
    h.push(Graph::getEdgeEnd(eid), NODE, mid, MOVING_OBJECT, Graph::getEdgeLen(eid) - pos);

    unordered_set<int> lmrks = Graph::getEdgeLmrks(eid);
    for (const auto& lm: lmrks)
        h.push(lm, LANDMARK, mid, MOVING_OBJECT, fabs(Graph::getLmrkById(lm).second - pos));

    ElementSet isVis;

    int cnt = 0;

    double aggV = sr->agg(1, 2) == 2 ? DBL_MIN : sr->agg(1, 2) == 1 ? DBL_MAX : 0;

    while (cnt != sr->nl && !h.empty()){

        DistElement p = h.top();
        h.pop();

        pair<int, ElemType > curElem = p.getStartNode();
        if (isVis.isExist(curElem)) continue;
        isVis.insert(curElem);

        if (curElem.second == NODE){
            vector<int> adjNodes = Graph::getAdjacentNode(curElem.first);
            vector<int> adjEdges = Graph::getAdjacentEdge(curElem.first);

            for (const auto& e: adjEdges){
                unordered_set<int> adjLmrks = Graph::getEdgeLmrks(e);
                for (const auto& l: adjLmrks){

                    pair<int, double> lmrk = Graph::getLmrkById(l);

                    if (isVis.isExist(pair<int, ElemType >(l, LANDMARK)))
                        continue;

                    double dist = 0;

                    if (curElem.first == Graph::getEdgeStart(e))
                        dist = p.getDist() + lmrk.second;
                    else
                        dist = p.getDist() + (Graph::getEdgeLen(e) - lmrk.second);

                    DistElement nextElement(l, LANDMARK, mid, MOVING_OBJECT, dist);
                    h.push(nextElement);
                }

            }

            for (size_t i = 0; i < adjNodes.size(); i++){
                if (isVis.isExist(pair<int, ElemType >(adjNodes[i], NODE)))
                    continue;

                double dist = Graph::getEdgeLen(adjEdges[i]) + p.getDist();

                DistElement nextElement(adjNodes[i], NODE, mid, MOVING_OBJECT, dist);
                h.push(nextElement);
            }


        }else if (curElem.second == LANDMARK){
            cnt++;
            aggV = sr->agg(p.getDist(), aggV);
        }
    }

    Opf::insert(mid, aggV);
    Result::push(pair<int, double>(mid, aggV));
}

void SafeRegion:: updateLC(unordered_set<int>& expandedEdges){
    for (const auto& e: expandedEdges) {
        int snid = Graph::getEdgeStart(e);
        int enid = Graph::getEdgeEnd(e);

        double len = Graph::getEdgeLen(e);

        for (int i = 0; i < sr->nl; i++) {
            if (!PartialMatrix::isExist(snid, i) || !PartialMatrix::isExist(enid, i))
                continue;
            double dx = PartialMatrix::getDist(snid, i);
            double dy = PartialMatrix::getDist(enid, i);
            if (fabs(dy - dx - len) < 0.0001){
                LC::insert(e, i, X_LANDMARK);
            }else if (fabs(dx - dy - len) < 0.0001){
                LC::insert(e, i, Y_LANDMARK);
            }else{
                LC::insert(e, i, Z_LANDMARK);
            }

        }
    }
}

void SafeRegion::updateMAD(unordered_set<int>& expandedEdges){
    for (const auto& e: expandedEdges) {
        int snid = Graph::getEdgeStart(e);
        int enid = Graph::getEdgeEnd(e);
        double len = Graph::getEdgeLen(e);
        unordered_set<int> lmrks = Graph::getEdgeLmrks(e);

        bool giveUp = false;
        for (int i = 0; i < sr->nl; i++){
            if (!PartialMatrix::isExist(snid, i)) {
                giveUp = true;
                break;
            }
            if (!PartialMatrix::isExist(enid, i)){
                giveUp = true;
                break;
            }
        }

        if (giveUp) continue;

        double lowBound = DBL_MAX;

        double aggS = getAggDist(e, 0);
        double aggE = getAggDist(e, len);

        lowBound = min(aggS, aggE);

        for (const auto& lmrkId: lmrks){
            auto lmrk = Graph::getLmrkById(lmrkId);
            lowBound = min(lowBound, getAggDist(lmrk.first, lmrk.second));
        }

        MAD::insert(e, lowBound);

    }
}

void SafeRegion::updateOpf(unordered_set<int> &expandedEdges) {
    for (const auto &e: expandedEdges){
        auto edgeMobjs = MovingObject::getPFromE(e);
        for (const auto &mobj: edgeMobjs){
            double aggDist = getAggDist(mobj.second.first, mobj.second.second);

            if (aggDist < sr->r) {
                Opf::insert(mobj.first, aggDist);
            }
        }
    }
}


#endif //CAKNNSR_SAFEREGION_H
