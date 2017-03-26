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


using namespace std;

class SafeRegion{

private:
    SafeRegion(){}

    double r;
    double dk;
    int nl; // number of landmarks


    function<double (double, double)> agg;



    static SafeRegion* sr;

    static void forceUpdate(int mid, int eid, double pos);
    static double velocity();
    static double getAggDist(int eid, double pos);


public:

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
        unordered_set<pair<int, pair<int, double >>> mobjs = expansion->expand(sr->r);
        for (const auto & mobj: mobjs){
            double aggDist = getAggDist(mobj.second.first, mobj.second.second);
            if (aggDist < sr->r){
                Opf::insert(mobj.first, mobj.second.first,
                            mobj.second.second, aggDist);
            }
        }
    }
};

static double SafeRegion::velocity() {return 0;}

static double SafeRegion::getAggDist(int eid, double pos) {

    int snid = Graph::getEdgeStart(eid);
    int enid = Graph::getEdgeEnd(eid);
    double d = min(PartialMatrix::getDist(snid, 0), PartialMatrix::getDist(enid, 0));

    double res = d;

    for (int i = 1; i < sr->nl; i++){
        d = min(PartialMatrix::getDist(snid, i), PartialMatrix::getDist(enid, i));
        res = sr->agg(res, d);
    }
    return  res;
}

static void SafeRegion::update(int mid, int eid, double pos) {

    pair<int, double > oldPair = MovingObject::getP(mid);

//    if (oldPair.second != DBL_MAX)
//        double dold = getAggDist(oldPair.first, oldPair.second);

    MovingObject::updateP(mid, eid, pos);  // update in moving object database unconditionally...

    if (eid != -1 && sr->dk == DBL_MAX) {  // not a deleted moving object...
        forceUpdate(mid, eid, pos);
    }else {

        double di = getAggDist(eid, pos);

        if (di < sr->r) Opf::insert(mid, eid, pos, di);  // update opf according to new aggregate distance
        else Opf::erase(mid);

        if (Result::isExist(mid)) {
            Result::erase(mid);
        }

        sr->dk = Result::getKthDist();

        if (di < sr->dk) {
            pair<int, double> p(mid, di);
            Result::push(p);
            sr->dk = Result::getKthDist();
        }

        if (sr->dk == DBL_MAX) {
            pair<int, double> p = Opf::getMin();
            while (sr->dk == DBL_MAX && p != Opf::USE_LESS_PAIR) {
                if (!Result::isExist(p.first)) {
                    Result::push(p);
                    sr->dk = Result::getKthDist();
                }
            }
        }
    }

    sr->dk = Result::getKthDist();
    if (sr->dk != DBL_MAX) {
        sr->r = sr->dk + velocity();
    }

}

static void SafeRegion::forceUpdate(int mid, int eid, double pos) {

    printf("force update %d %d %lf\n", mid, eid, pos);

    if (eid == -1) return;

    DistElements h;

    h.push(Graph::getEdgeStart(eid), NODE, mid, MOVING_OBJECT, pos);
    h.push(Graph::getEdgeEnd(eid), NODE, mid, MOVING_OBJECT, pos);

    vector<int> lm = Graph::getEdgeLmrks(eid);
    for (int i = 0; i < lm.size(); i++)
        h.push(lm[i], LANDMARK, mid, MOVING_OBJECT, pos);

    unordered_set<pair<int, ElemType >> isVis;

    int cnt = 0;

    double aggV = sr->agg(1, 2) == 2 ? DBL_MIN : sr->agg(1, 2) == 1 ? DBL_MAX : 0;

    while (cnt != sr->nl && !h.empty()){

        DistElement p = h.top();
        h.pop();

        pair<int, ElemType > curElem = p.getStartNode();
        if (isVis.find(curElem) != isVis.end()) continue;
        isVis.insert(curElem);

        if (curElem.second == NODE){
            vector<int> adjNodes = Graph::getAdjacentNode(curElem.first);
            vector<int> adjEdges = Graph::getAdjacentEdge(curElem.first);

            for (const auto& e: adjEdges){
                vector<int> adjLmrks = Graph::getEdgeLmrks(e);
                for (const auto& l: adjLmrks){

                    pair<int, double> lmrk = Graph::getLmrkById(l);

                    if (isVis.find(pair<int, ElemType >(lmrk.first, LANDMARK)) != isVis.end())
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
                if (isVis.find(pair<int, ElemType >(adjNodes[i], NODE)) != isVis.end())
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

    Opf::insert(mid, eid, pos, aggV);
    Result::push(pair<int, double>(mid, aggV));
}

#endif //CAKNNSR_SAFEREGION_H
