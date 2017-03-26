//
// Created by mgh on 3/26/17.
//

#ifndef CAKNNSR_SLEEXPANSION_H
#define CAKNNSR_SLEEXPANSION_H

#include <algorithm>
#include <vector>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <queue>

#include "BaseExpansion.h"
#include "MovingObject.h"
#include "PartialMatrix.h"
#include "DistElement.h"
#include "Graph.h"


class SLEExpansion : public BaseExpansion{
private:
    DistElements distElements;

    vector<ElementSet> visited;  // visited[i]: (0, N - 1): node;

    bool isVisit(int frm, pair<int, ElemType> to){
        if (frm >= visited.size())  cout << "illegal landmark id triggered" << endl;
        return visited[frm].isExist(to); }

    void visit(int frm, pair<int, ElemType> to){
        visited[frm].insert(to);
    }

public:

    SLEExpansion(unsigned int numLmrks){ visited = vector<ElementSet>(numLmrks, ElementSet());}

    vector<pair<int, pair<int, double >>> expand(double r);
};

vector<pair<int, pair<int, double >>> SLEExpansion::expand(double r){
    if (r == DBL_MAX)
        return vector<pair<int, pair<int, double >>>();

    if (!isVisit(0, pair<int, ElemType >(0, NODE)) && distElements.empty()){  // the search is not began
        for (int i = 0; i < Graph::getNumLmrks(); i++){
            pair<int, double> lmrk = Graph::getLmrkById(i);
            int &edgeLmrk = lmrk.first;
            double &pos = lmrk.second;
            distElements.push(i, LANDMARK,
                              Graph::getEdgeStart(edgeLmrk), NODE,
                              pos);
            distElements.push(i, LANDMARK,
                              Graph::getEdgeEnd(edgeLmrk), NODE,
                              Graph::getEdgeLen(edgeLmrk) - pos);
        }
    }

    vector<pair<int, pair<int, double >>> expandedOpf;

    while(!distElements.empty()){

        if (distElements.top().getDist() > r) return expandedOpf;

        DistElement p = distElements.top();
        distElements.pop();


        if (isVisit(p.getStartNode().first, p.getEndNode())) continue; // the object is not visited

        pair<int, ElemType > frm = p.getStartNode();
        pair<int, ElemType > to = p.getEndNode();
        visit(frm.first, to);


        // if it is a node, insert to M
        if (to.second == NODE) PartialMatrix::setDist(to.first, frm.first, p.getDist() );
        else continue;

        vector<int> adjN = Graph::getAdjacentNode(to.first);
        vector<int> adjE = Graph::getAdjacentEdge(to.first);

        for (int i = 0; i < (int)adjN.size(); i++){  // next search
            if (isVisit(frm.first, pair<int, ElemType >(adjN[i], NODE))) continue; // if found skip...
            distElements.push(frm.first, LANDMARK, adjN[i], NODE,
                              p.getDist() + Graph::getEdgeLen(adjE[i]));
        }

        // update opf from adjacent node, these objects are only stored in disk
        for (int i = 0; i < (int)adjE.size(); i++){
            vector<pair<int, pair<int, double >>> mobjs = MovingObject::getPFromE(adjE[i]);
            for (const auto& mobj : mobjs)
                expandedOpf.push_back(mobj);
        }
    }
    return expandedOpf;
}
#endif //CAKNNSR_SLEEXPANSION_H
