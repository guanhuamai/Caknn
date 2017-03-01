#ifndef __EXPANSION
#define __EXPANSION

#include <vector>
#include "SDB.h"
using namespace std;

class SLE: public SDB{
public:
    vector<DistEle> h;  // heap for expanding landmarks
    vector<unordered_set<int>> visited;  // visited[i]: (0, N - 1): node;

    SLE(size_t k, Graph g, vector<pair<int, double>> lmrks,
        function<double(double, double)> agg) :SDB(k, g, lmrks, agg){
        visited = vector<unordered_set<int>>(nl, unordered_set<int>());
    }

    bool isVisit(int lid, int oid){return visited[lid].find(oid) != visited[lid].end();}
    void visit(int lid, int oid){visited[lid].insert(oid);}

    void expand();
};

class ILE_MIN: public SDB{
    vector<DistEle> h;  // heap for expanding landmarks
    unordered_set<int> visited;

    ILE_MIN(size_t k, Graph g, vector<pair<int, double>> lmrks,
            function<double(double, double)> agg):SDB(k, g, lmrks, agg){}

    bool isVisit(int lid, int oid){return visited.find(oid) != visited.end();}
    void visit(int lid, int oid){visited.insert(oid);}

    void expand();
};

class ILE_SUM: public SDB{
    vector<vector<DistEle>> h;

    vector<unordered_set<int>> visited;  // visited[i]: (0, N - 1): node; (N + P,): landmarks

    vector<double> sum;
    vector<double> dn;
    vector<int> n;

    ILE_SUM(size_t k, Graph g, vector<pair<int, double>> lmrks,
            function<double(double, double)> agg):SDB(k, g, lmrks, agg){
        visited = vector<unordered_set<int>>(nl, unordered_set<int>());
        sum = vector<double> (nl, 0);
        dn = vector<double> (nl, 0);
        n = vector<int>(nl, 0);
    }

    void expand();  // expand M according to r, improved in sum

    double calDlow(int i){return sum[i] + dn[i] * n[i];}

    pair<int, double> findMinDlow(){
        int mid(-1);  double minD(DBL_MAX);
        for(int i = 0; i < nl; i++){
            double tmp = calDlow(i);
            if (tmp < minD){minD = tmp, mid = i;}
        }
        return pair<int, double>(mid, minD);
    }

    bool hEmpty(){for(int i = 0; i < nl; i++){if(h[i].size()!=0)return true;} return false;}
};

void SLE::expand(){ // expand till the top exceed radius r
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
        for (int i = 0; i < (int)adjN.size(); i++){
            if (isVisit(p.lid, adjN[i])) continue; // if found skip...
            insertP(DistEle(adjN[i], p.lid, p.dist + G.edges[adjE[i]].len), h);
        }

        for (int i = 0; i < (int)adjE.size(); i++){
            vector<pair<int, pair<int, double >>> mobjs = MovingObject::getPFromE(adjE[i]);
            for (const auto& mobj : mobjs) updateOpf(mobj.first, mobj.second.first, mobj.second.second);
        }

        for (int i = 0; i < res.size(); i++){ if (opf.find(res[i].first) != opf.end()) opf.erase(res[i].first); }
    }
}

void ILE_MIN::expand(){ // expand till the top exceed radius r
    while(!h.empty()){
        DistEle p = h[0];

        if (p.dist > r) return;

        pop_heap(h.begin(), h.end()), h.pop_back(); // deheap

        if (isVisit(p.lid, p.oid)) continue; // the object is not visited

        visit(p.lid, p.oid);  // mark as visited

        if (p.oid < (int)G.nodes.size()) insertM(p.oid, p.lid, p.dist);  // if it is a node, insert to M
        else continue;

        vector<int>& adjN = G.nodes[p.oid].adjNodes;
        vector<int>& adjE = G.nodes[p.oid].adjEdges;
        for (int i = 0; i < (int)adjN.size(); i++){
            if (isVisit(p.lid, adjN[i])) continue; // if found skip...
            insertP(DistEle(adjN[i], p.lid, p.dist + G.edges[adjE[i]].len), h);
        }
    }
}

void ILE_SUM::expand(){
    while (!hEmpty()){
        pair<int, double> dlow = findMinDlow();
        if (dlow.second > r) return;

        int i(dlow.first);

        vector<DistEle>& hi = h[i];

        DistEle p = hi[0];
        pop_heap(hi.begin(), hi.end()), hi.pop_back(); // deheap

        if (visited[p.lid].find(p.oid) != visited[p.lid].end()) continue; // the object is not visited

        visited[p.lid].insert(p.oid);  // mark as visited

        if (p.oid < (int)G.nodes.size()) insertM(p.oid, p.lid, p.dist);  // if it is a node, insert to M
        else{n[i]++, sum[i]+=p.dist; continue;}


        vector<int>& adjN = G.nodes[p.oid].adjNodes;
        vector<int>& adjE = G.nodes[p.oid].adjEdges;
        for (int i = 0; i < (int)adjN.size(); i++){

            if (visited[p.lid].find(adjN[i]) != visited[p.lid].end()) continue; // if found skip...
            insertP(DistEle(adjN[i], p.lid, p.dist + G.edges[adjE[i]].len), hi);

            if (hsLmrk.find(adjE[i]) == hsLmrk.end()) continue;

            vector<int>& lm = hsLmrk[adjE[i]];

            for (int j = 0; j < (int)lm.size(); j++){
                int lmid = lmrks[lm[j]].first + G.nodes.size(); // landmark id in visited vector
                if (visited[p.lid].find(lmid) == visited[p.lid].end()) continue;
                double d = getDist(p.lid, adjE[i], lmrks[lm[j]].second);

                insertP(DistEle(lmid, p.lid, d), hi);
            }
        }
    }
}

#endif // __EXPANSION
