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
    DistElements distElements;

    ElementSet visited;  // visited[i]: (0, N - 1): node;

    int expandCount;

    bool isVisit(int frm, pair<int, ElemType> to){
        return visited.isExist(to); }

    void visit(int frm, pair<int, ElemType> to){
        visited.insert(to);
    }

public:

    IleSumExpansion(unsigned int numLmrks){
        visited = ElementSet();
        expandedRange = DBL_MAX;
        expandCount = 0;
    }

    unordered_set<int> expand(double r);

    void display() const {
        cout << "***********" << endl;
        cout << "display sle expansion:" << endl;
        distElements.display();
        visited.display();
        cout << "expanded for " << expandCount << "times" << endl;
        cout << "***********" << endl;
    }
};

unordered_set<int> IleSumExpansion::expand(double r){
    if (r > expandedRange) expandedRange = r;

    if (r > expandedRange && r != DBL_MAX) expandCount++;

    if (r == DBL_MAX)
        return unordered_set<int>();

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

    unordered_set<int> expandedEdges;

    while(!distElements.empty()){
        if (distElements.top().getDist() > r) return expandedEdges;

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
        expandedEdges.insert(adjE.begin(), adjE.end());
    }

    return expandedEdges;
}

#endif //CAKNNSR_ILESUMEXPANSION_H
