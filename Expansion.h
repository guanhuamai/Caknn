#ifndef __EXPANSION
#define __EXPANSION

#include <vector>
#include "SDB.h"
using namespace std;
using namespace std::placeholders;

class SLE: public SDB{
protected:
    void expand(function<bool (int, int)> isVisit, function<void (int, int)> visit);
public:

    vector<DistEle> h;  // heap for expanding landmarks
    vector<unordered_set<int>> visited;  // visited[i]: (0, N - 1): node;

    SLE(size_t k, Graph g, vector<pair<int, double>> lmrks,
        function<double(double, double)> agg) :SDB(k, g, lmrks, agg){
        visited = vector<unordered_set<int>>(nl, unordered_set<int>());
    }

    bool sleIsVisit(int lid, int oid){return visited[lid].find(oid) != visited[lid].end();}

    void sleVisit(int lid, int oid){visited[lid].insert(oid);}

    void expand() {
        function<bool(int, int)> isVisitFunction = bind(&SLE::sleIsVisit, this, _1, _2);
        function<void(int, int)> visitFunction = bind(&SLE::sleVisit, this, _1, _2);
        expand(isVisitFunction, visitFunction);
    }

};

class ILE_MIN: public SLE{
public:
    vector<DistEle> h;  // heap for expanding landmarks
    unordered_set<int> visited;

    ILE_MIN(size_t k, Graph g, vector<pair<int, double>> lmrks,
            function<double(double, double)> agg):SLE(k, g, lmrks, agg){}

    bool ileMinIsVisit(int lid, int oid){return visited.find(oid) != visited.end();}

    void ileMinVisit(int lid, int oid){visited.insert(oid);}

    void expand() {
        function<bool(int, int)> isVisitFunction = bind(&ILE_MIN::ileMinIsVisit, this, _1, _2);
        function<void(int, int)> visitFunction = bind(&ILE_MIN::ileMinVisit, this, _1, _2);
        SLE::expand(isVisitFunction, visitFunction);
    }

};

class ILE_SUM: public SDB{
public:
    vector<vector<DistEle>> h;

    vector<unordered_set<int>> visited;  // visited[i]: (0, N - 1): node; (N + P,): landmarks

    vector<double> sum;
    vector<int> n;

    ILE_SUM(size_t k, Graph g, vector<pair<int, double>> lmrks,
            function<double(double, double)> agg):SDB(k, g, lmrks, agg){
        h = vector<vector<DistEle>>(nl, vector<DistEle>());
        visited = vector<unordered_set<int>>(nl, unordered_set<int>());
        sum = vector<double> (nl, 0);
        n = vector<int> (nl, 1);
    }

    bool ileSumIsVisit(int lid, int oid){return visited[lid].find(oid) != visited[lid].end();}

    void ileSumVisit(int lid, int oid){visited[lid].insert(oid);}

    void expand();  // expand M according to r, improved in sum

    double calDlow(int i){
        if (!h[i].empty()) return sum[i] + n[i] * h[i][0].dist;
        else return DBL_MAX;
    }

    pair<int, double> findMinDlow(){
        int lid(-1);  double minD(DBL_MAX);
        for(int i = 0; i < nl; i++){
            double tmp = calDlow(i);
            if (tmp < minD){minD = tmp, lid = i;}
        }
        return pair<int, double>(lid, minD);
    }

    bool hEmpty(){for(int i = 0; i < nl; i++){if(h[i].size()!=0)return false;} return true;}
};

// expand till the top exceed radius r
void SLE::expand(function<bool (int, int)> isVisit, function<void (int, int)> visit){
    if (!isVisit(0, 0) && h.empty()){  // the search is not began
        for (int i = 0; i < nl; i++){
            Edge& e = G.edges[lmrks[i].first];
            insertP(DistEle(e.snid, i, lmrks[i].second), h);
            insertP(DistEle(e.enid, i, e.len - lmrks[i].second), h);
        }
    }
    while(!h.empty()){

        if (h[0].dist > r) return;

        DistEle p = popP(h); // deheap

        if (isVisit(p.lid, p.oid)) continue; // the object is not visited

        visit(p.lid, p.oid);  // mark as visited

        if (p.oid < (int)G.nodes.size()) insertM(p.oid, p.lid, p.dist);  // if it is a node, insert to M
        else continue;

        vector<int>& adjN = G.nodes[p.oid].adjNodes;
        vector<int>& adjE = G.nodes[p.oid].adjEdges;

        for (int i = 0; i < (int)adjN.size(); i++){  // next search
            if (isVisit(p.lid, adjN[i])) continue; // if found skip...
            insertP(DistEle(adjN[i], p.lid, p.dist + G.edges[adjE[i]].len), h);
        }

        // update opf from adjacent node, these objects are only stored in disk
        for (int i = 0; i < (int)adjE.size(); i++){
            vector<pair<int, pair<int, double >>> mobjs = MovingObject::getPFromE(adjE[i]);
            for (const auto& mobj : mobjs) updateOpf(mobj.first, mobj.second.first, mobj.second.second);
        }
    }
}

void ILE_SUM::expand(){
    if (!ileSumIsVisit(0, 0) && hEmpty()){
        for (int i = 0; i < nl; i++){
            Edge& e = G.edges[lmrks[i].first];
            insertP(DistEle(e.snid, i, lmrks[i].second), h[i]);
            insertP(DistEle(e.enid, i, e.len - lmrks[i].second), h[i]);
        }
    }

    while (!hEmpty()){
        pair<int, double> dlow = findMinDlow();
        if (dlow.second > r) return;

        vector<DistEle>& hi = h[dlow.first];

        DistEle p = popP(hi);

        if (ileSumIsVisit(p.lid, p.oid)) continue; // the object is not visited

        ileSumVisit(p.lid, p.oid);  // mark as visited

        if (p.oid < (int)G.nodes.size()) insertM(p.oid, p.lid, p.dist);  // if it is a node, insert to M
        else{n[p.oid - G.nodes.size()]++, sum[p.oid - G.nodes.size()] += p.dist; continue;}


        vector<int>& adjN = G.nodes[p.oid].adjNodes;
        vector<int>& adjE = G.nodes[p.oid].adjEdges;

        for (int i = 0; i < (int)adjN.size(); i++){
            if (ileSumIsVisit(p.lid, adjN[i])) continue; // if found skip...
            insertP(DistEle(adjN[i], p.lid, p.dist + G.edges[adjE[i]].len), hi);
        }

        for (int i = 0; i < (int)adjE.size(); i++){
            vector<pair<int, pair<int, double >>> mobjs = MovingObject::getPFromE(adjE[i]);
            for (const auto& mobj : mobjs) updateOpf(mobj.first, mobj.second.first, mobj.second.second);

            if (hsLmrk.find(adjE[i]) == hsLmrk.end()) continue;

            for (const auto & lmid : hsLmrk[adjE[i]]){
                if (ileSumIsVisit(p.lid, lmid + G.nodes.size())) continue;
                double d = getDist(p.lid, adjE[i], lmrks[lmid].second);

                insertP(DistEle(lmid + G.nodes.size(), p.lid, d), hi);
            }
        }

        for (int i = 0; i < res.size(); i++){ if (opf.find(res[i].first) != opf.end()) opf.erase(res[i].first); }
    }
}

#endif // __EXPANSION
