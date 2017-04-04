//
// Created by mgh on 4/2/17.
//

#ifndef CAKNNSR_ILESUMEXPANSION_H
#define CAKNNSR_ILESUMEXPANSION_H

#include <vector>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <set>

#include "BaseExpansion.h"
#include "MovingObject.h"
#include "PartialMatrix.h"
#include "DistElement.h"
#include "Graph.h"


class IleSumExpansion : public BaseExpansion{
private:

    // distElements[i]: heap of ith landmark
    vector<DistElements> distElementsVec;

    // visited[i]: visited elements of the ith landmark
    vector<ElementSet> visited;

    // numOfVistedLandmarks[i]: the number of visited landmarks by ith landmark
    vector<unsigned > numOfVistedLandmarks;

    // sumOfVistedLandmarks[i]: the sum of visited landmarks by ith landmark
    vector<double> sumOfVistedLandmarks;

    // latestExpandDist[i]: the latest expanded dist poped from distElementsVec[i]
    vector<double> latestExpandDist;

    int expandCount;  // number of valid expansion

    bool isVisit(int frm, pair<int, ElemType> to){
        if (frm >= visited.size())  cout << "illegal landmark id triggered" << endl;
        return visited[frm].isExist(to); }

    void visit(int frm, pair<int, ElemType> to){
        visited[frm].insert(to);
    }

    bool distElementsVecEmpty(){
        for (auto& e: distElementsVec){
            if (!e.empty()) return false;
        }
        return true;
    }

    pair<int, double> getLandmarkByMinDlow(){  // return: first: heap/landmark id, second: dlow
        double minDlow = DBL_MAX;
        int res = -1;
        for (int i = 0; i < distElementsVec.size(); i++){
            if (distElementsVec[i].empty()) continue;  // must prune when the heap become empty

            double dlow = latestExpandDist[i] * numOfVistedLandmarks[i] - sumOfVistedLandmarks[i];
            if (minDlow > dlow){
                minDlow = dlow;
                res = i;
            }
        }
        return pair<int, double>(res, minDlow);
    }

public:

    IleSumExpansion(unsigned int numLmrks){
        distElementsVec = vector<DistElements>(numLmrks, DistElements());
        visited = vector<ElementSet>(numLmrks, ElementSet());
        numOfVistedLandmarks = vector<unsigned > (numLmrks, 0);
        sumOfVistedLandmarks = vector<double> (numLmrks, 0);
        latestExpandDist = vector<double> (numLmrks, 0);

        expandedRange = DBL_MAX;
        expandCount = 0;
    }

    unordered_set<int> expand(double r);

    void display() const {
        cout << "***********" << endl;
        cout << "display ile-sum expansion:" << endl;
        for (const auto& e: distElementsVec) {
            e.display();
        }
        for (const auto& e: visited){
            e.display();
        }

        for (const auto& e: numOfVistedLandmarks){
            cout << "number of visited landmarks: " << e << endl;
        }
        for (const auto& e: sumOfVistedLandmarks){
            cout << "sum of visited landmarks: " << e << endl;
        }
        for (const auto& e: latestExpandDist){
            cout << "latest expand distance: " << e << endl;
        }

        cout << "expanded for " << expandCount << " times" << endl;
        cout << "***********" << endl;
    }
};

unordered_set<int> IleSumExpansion::expand(double r){
    if ((expandedRange == DBL_MAX || r > expandedRange) && r != DBL_MAX) expandCount++;

    if (expandedRange != DBL_MAX) expandedRange = max(r, expandedRange);
    else expandedRange = r;

    if (r == DBL_MAX)
        return unordered_set<int>();

    if (!isVisit(0, pair<int, ElemType >(0, NODE)) && distElementsVecEmpty()){  // the search is not began
        for (int i = 0; i < Graph::getNumLmrks(); i++){
            pair<int, double> lmrk = Graph::getLmrkById(i);
            int &edgeLmrk = lmrk.first;
            double &pos = lmrk.second;
            distElementsVec[i].push(i, LANDMARK,
                              Graph::getEdgeStart(edgeLmrk), NODE,
                              pos);
            distElementsVec[i].push(i, LANDMARK,
                              Graph::getEdgeEnd(edgeLmrk), NODE,
                              Graph::getEdgeLen(edgeLmrk) - pos);
        }
    }

    unordered_set<int> expandedEdges;

    while(!distElementsVecEmpty()){
        pair<int, double> mindlowLandmark = getLandmarkByMinDlow();
        if (mindlowLandmark.second > r) return expandedEdges;

        auto& distElements = distElementsVec[mindlowLandmark.first];
        DistElement p = distElements.top();
        distElements.pop();


        if (isVisit(p.getStartNode().first, p.getEndNode())) continue; // the object is not visited

        pair<int, ElemType > frm = p.getStartNode();
        pair<int, ElemType > to = p.getEndNode();
        visit(frm.first, to);

        latestExpandDist[frm.first] = p.getDist();  // update d(n, lj)

        // if it is a node, insert to M
        if (to.second == NODE) {
            PartialMatrix::setDist(to.first, frm.first, p.getDist());

            vector<int> adjN = Graph::getAdjacentNode(to.first);
            vector<int> adjE = Graph::getAdjacentEdge(to.first);

            for (int i = 0; i < (int) adjN.size(); i++) {  // next search
                if (isVisit(frm.first, pair<int, ElemType>(adjN[i], NODE))) continue; // if found skip...
                distElements.push(frm.first, LANDMARK, adjN[i], NODE,
                                  p.getDist() + Graph::getEdgeLen(adjE[i]));
            }

            for (const auto& e: adjE){  // main difference between ile-sum and sle
                auto adjLmrks = Graph::getEdgeLmrks(e);
                for (const auto& adjLmrk: adjLmrks){
                    if (isVisit(frm.first, pair<int, ElemType >(adjLmrk, LANDMARK))) continue;

                    double d = DBL_MAX;

                    auto sourceLmrk = Graph::getLmrkById(frm.first);
                    auto curLmrk = Graph::getLmrkById(adjLmrk);

                    if (e == sourceLmrk.first){
                        d = fabs(sourceLmrk.second - curLmrk.second);
                    }else if (Graph::getEdgeStart(e) == to.first){
                        d = p.getDist() + curLmrk.second;
                    }else if (Graph::getEdgeEnd((e) == to.second)){
                        d = p.getDist() + Graph::getEdgeLen(e) - curLmrk.second;
                    }else{
                        cout << "unexpected adjacent landmark encountered" << endl;
                    }

                    distElements.push(frm.first, LANDMARK, adjLmrk, LANDMARK, d);
                }
            }

            // update opf from adjacent node, these objects are only stored in disk
            expandedEdges.insert(adjE.begin(), adjE.end());
        }
        else if (to.second == LANDMARK){  // update Ni, sumi
            numOfVistedLandmarks[frm.first] ++;
            sumOfVistedLandmarks[frm.first] += p.getDist();
        }

    }
    return expandedEdges;
}


#endif  //CAKNNSR_ILE_SUM_EXPANSION_H
