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


    function<double (double, double)> agg;

    static void forceUpdate(int mid, int eid, double pos);
    static double velocity();
    static double getAggDist(int eid, double pos);

    static void updateLC(unordered_set<int>& expandedEdges);
    static void updateMAD(unordered_set<int>& expandedEdges);
    static void updateOpf(unordered_set<int>& expandedEdges);


public:

    static SafeRegion* sr;

    static void buildSafeRegion(string nodeF, string edgeF,
                                vector<pair<int, double>>& lmrks,
                                int k,
                                function<double (double, double)> agg){
        Graph::graphFactory(nodeF, edgeF);
        Graph::initLmrks(lmrks);

        PartialMatrix::buildPartialMatrix();
        Opf::buildOpf();
        Result::buildResult(k);

        sr = new SafeRegion();

        sr->agg = agg;
        sr->nl = Graph::getNumLmrks();
    }


    static void update(int mid, int eid, double pos);

    static void expand(BaseExpansion* expansion){
//        cout << "start expanding &&&&&&&&&&&&&&&&&&&&&&& r = " << sr->r << endl;
        unordered_set<int> mobjs = expansion->expand(sr->r);
//        cout << "opf before " << endl;
//        Opf::display();
        for (const auto & mobjId: mobjs){
            auto mobjPos = MovingObject::getP(mobjId);

            double aggDist = getAggDist(mobjPos.first, mobjPos.second);

            if (mobjId == 4155){
                bool tmp = mobjId == 4155;
                cout << "catch mobjid" << endl;
                cout << mobjId << endl;
            }

            if (aggDist < sr->r){
                Opf::insert(mobjId, aggDist);
            }
        }
//        cout << "opf after " << endl;
//        Opf::display();
//        cout << "end expanding &&&&&&&&&&&&&&&&&&&&&&& r= " << sr->r << endl;
    }
};

SafeRegion* SafeRegion::sr = NULL;

double SafeRegion::velocity() {return 0;}

double SafeRegion::getAggDist(int eid, double pos) {

    double res = sr->agg(1, 2) == 1 ? DBL_MAX : sr->agg(1, 2) == 2 ? DBL_MIN : 0;

    int snid(Graph::getEdgeStart(eid)), enid(Graph::getEdgeEnd(eid));
    for (int i = 0; i < sr->nl; i++){
        double ds = PartialMatrix::getDist(snid, i);
        double de = PartialMatrix::getDist(enid, i);

        double d = DBL_MAX;

        if (ds != DBL_MAX) d = min(d, ds + pos);
        if (de != DBL_MAX) d = min(d, de + Graph::getEdgeLen(eid) - pos);

        if (eid == Graph::getLmrkById(i).first)
            d = min(d, abs(pos - Graph::getEdgeLen(eid)));

        res = sr->agg(res, d);
    }

    return res;
}

void SafeRegion::update(int mid, int eid, double pos) {
    MovingObject::updateP(mid, eid, pos);  // update in moving object database unconditionally...

    if (eid != -1 && sr->dk == DBL_MAX) {  // not a deleted moving object...
        forceUpdate(mid, eid, pos);
    }else {

        double di = getAggDist(eid, pos);

        if (di < sr->r) Opf::insert(mid, di);  // update opf according to new aggregate distance
        else  Opf::erase(mid);

        if (Result::exist(mid) || di < sr->dk) { // rebuild if the moving object is in the result.
            int k = Result::getK();
            vector<pair<int, double>> kOpfs = Opf::getNSmallest(k);
            Result::rebuild(kOpfs);
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
        h.push(lm, LANDMARK, mid, MOVING_OBJECT, abs(Graph::getLmrkById(lm).second - pos));

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
            if (abs(dy - dx - len) < 0.0001){
                LC::insert(e, i, X_LANDMARK);
            }else if (abs(dx - dy - len) < 0.0001){
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

#endif //CAKNNSR_SAFEREGION_H
