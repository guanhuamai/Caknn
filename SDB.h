#ifndef __SDB
#define __SDB

#include <algorithm>
#include <vector>
#include <bitset>
#include <unordered_set>
#include <unordered_map>

#include "Graph.h"
#include "functional"
#include "MovingObject.h"
using namespace std;

bool cmpResult(pair<int, double>& a, pair<int, double>& b){return a.second < b.second;}

class SDB{
public:
    Graph G;  // the graph

    int nl;  // number of landmarks
    unordered_map<int, vector<int>> hsLmrk;  // landmark hash key: eid, val: lmid
    vector<pair<int, double>> lmrks;  // landmarks   pair: 1st

    unordered_map<bitset<64>, double> m; // partial distance matrix M, key: 32int + 32int

    function<double(double, double)> agg;

    double r;  // safe distance
    unordered_set<int> opf;  // moving objects id inside of the safe region, or protential future objects

    double dk;  // always point to kth dist
    vector<pair<int, double>> res;  //result.

    size_t k;

    struct DistEle{
        int oid;  //(0, N - 1): node; [N, N + L): landmarks; [N + L,): moving objects
        int lid;  //landmark id
        double dist;

        DistEle(int o, int l, double d): oid(o), lid(l), dist(d){}

        bool operator < (const DistEle& b) const{
            return dist > b.dist;
        }
    };

    //--------------------- important method below------------------------
    SDB(){}

    SDB(size_t k, Graph g, vector<pair<int, double>> lmrks,
        function<double (double, double)> aggt):
            r(DBL_MAX), dk(DBL_MAX), k(k), G(g), lmrks(lmrks), agg(aggt){
        nl = lmrks.size();
        for (int i = 0; i < nl; i++) hsLmrk[lmrks[i].first] = vector<int>();
        for (int i = 0; i < nl; i++) hsLmrk[lmrks[i].first].push_back(i);
    }

    virtual double velocity();
    // distance from lid to point
    virtual double getDist(int lid, int eid, double pos);

    virtual double getAggDist(int eid, double pos);

    virtual void update(int mid, int eid, double pos); // mid: moving object id

    virtual void forceUpdate(int mid, int eid, double pos);

    virtual void expand(){} // base expand

    virtual void updateOpf(int mid, int eid, double pos);  // update potential objects

    //--------------------trivial method below-----------------
    void findLoc (int id, size_t& s, vector<pair<int, double>>& v){
        while (s < v.size() && v[s].first != id) s++;
    }

    void resMkHeap(){
        make_heap(res.begin(), res.end(), cmpResult);
        if (res.size() == k) dk = res[0].second, r = dk + velocity(), printf("update radius: %lf\n", r);
        else if (res.size() < k) dk = DBL_MAX, r = DBL_MAX;
        else printf("res size should not larger than k, get %zu\n", res.size());
    }

    pair<int, double> findMinCand(){  // use brute force
        pair<int, double> r(-1, DBL_MAX);
        for (const auto &i : opf){
            pair<int, double> pi = MovingObject::getP(i);
            double di = getAggDist(pi.first, pi.second);
            if (di < r.second) r.first = i, r.second = di;
        }
        return r;
    }

    void replaceV(vector<pair<int, double>>& v, int i, pair<int, double> p){
        swap(v[i], v[v.size() - 1]), v.pop_back();
        v.push_back(p);
    }

    void eraseV(vector<pair<int, double>>& v, int i){swap(v[i], v[v.size() - 1]), v.pop_back();}

    bitset<64> getKey(int nid, int lid){bitset<64> k(nid); k = k<<32, k|=lid;  return k;}

    void insertM(int nid, int lid, double d){ bitset<64> k = getKey(nid, lid); m[k] = d;}

    void insertP(DistEle de, vector<DistEle>& h){h.push_back(de), make_heap(h.begin(),h.end());}

    DistEle popP(vector<DistEle>& h) {pop_heap(h.begin(),h.end());DistEle res = h.back();h.pop_back();return res;}
};

void SDB::updateOpf(int mid, int eid, double pos){  // update potential objects
    if (eid == -1) return;  // return if it has a delete mark
    for (int i = 0; i < nl; i++)
        if( r >= getDist(i, eid, pos)){
            opf.insert(mid);
            return;
        }

    if(opf.find(mid) != opf.end()) opf.erase(mid);  // if the moving object can no longer stay
}

void SDB::forceUpdate(int mid, int eid, double pos) { // using dijkstra to update

    printf("force update %d %d %lf\n", mid, eid, pos);

    if (eid == -1) return;

    vector<DistEle> tmpH;
    insertP(DistEle(G.edges[eid].snid, mid, pos), tmpH);
    insertP(DistEle(G.edges[eid].enid, mid, G.edges[eid].len - pos), tmpH);
    if (hsLmrk.find(eid) != hsLmrk.end()) {
        vector<int> &lm = hsLmrk[eid];
        for (int i = 0; i < lm.size(); i++)
            insertP(DistEle(G.nodes.size() + lm[i],
                            mid,
                            abs(pos - lmrks[lm[i]].second)),tmpH);
    }

    unordered_set<int> isVis;

    int cnt = 0;

    double aggV = agg(1, 2) == 2 ? DBL_MIN : agg(1, 2) == 1 ? DBL_MAX : 0;

    while (cnt != nl && !tmpH.empty()){
        DistEle p = popP(tmpH);
        if (isVis.find(p.oid) != isVis.end()) continue;
        isVis.insert(p.oid);
        if (p.oid < G.nodes.size()) {  // pop a node, push adjacent node and landmarks
            for (int i = 0; i < (int) G.nodes[p.oid].adjEdges.size(); i++){
                Edge& e = G.edges[G.nodes[p.oid].adjEdges[i]];

                if (hsLmrk.find(e.eid) == hsLmrk.end()) continue; // no landmark on edge

                vector<int>& tmplmrks = hsLmrk[e.eid];
                for (int j = 0; j < (int) tmplmrks.size(); j++){
                    int lmid = G.nodes.size() + tmplmrks[j];
                    if (isVis.find(lmid) != isVis.end()) continue;
                    if (p.oid == e.snid) insertP(DistEle(lmid, mid, p.dist + lmrks[tmplmrks[j]].second), tmpH);
                    else insertP(DistEle(lmid, mid, p.dist + e.len - lmrks[tmplmrks[j]].second), tmpH);
                }
            }

            for (int i = 0; i < (int) G.nodes[p.oid].adjNodes.size(); i++){
                if (isVis.find(G.nodes[p.oid].adjNodes[i]) != isVis.end()) continue;
                insertP(DistEle(G.nodes[p.oid].adjNodes[i], mid, G.edges[G.nodes[p.oid].adjEdges[i]].len + p.dist), tmpH);
            }
        }else{  // pop a landmark
            cnt++;
            aggV = agg(p.dist, aggV);
        }
    }
    res.push_back(pair<int, double>(mid, aggV));
    resMkHeap();

    MovingObject::insertP(mid, eid, pos);
}

void SDB::update(int mid, int eid, double pos){  // input the new information of the point
    if (eid != -1 && dk == DBL_MAX){ forceUpdate(mid, eid, pos); return;}  // not a deleted moving object...

    double di = getAggDist(eid, pos);

    size_t loc(0);
    findLoc(mid, loc, res);

    if (di <= r && eid != -1){
        if (di < dk){ // case i

            if (loc != res.size()) res[loc].second = di;
            else replaceV(res, 0, pair<int, double>(mid, di));

            resMkHeap();

        }else{ // case ii

            if (loc == res.size()){  //can't find mid
                if (opf.find(mid) == opf.end())
                    opf.insert(mid), MovingObject::insertP(mid, eid, pos);

            }else {
                pair<int, double> cand = findMinCand();
                if (cand.second < di) replaceV(res, loc, cand);

                resMkHeap();
            }
        }
    }else{  // case iii

        if (loc != res.size()){
            pair<int, double> cand = findMinCand();  // find the object in opf with minimum aggregate value.

            if (cand.second != DBL_MAX) {

                replaceV(res, loc, cand);

                opf.erase(cand.first);  // erase the newly selected moving object from opf

            }else eraseV(res, loc);

            resMkHeap();
        }

        updateOpf(mid, eid, pos);

        if(eid == -1) MovingObject::eraseP(mid);  // the moving object is marked to be deleted

        else MovingObject::insertP(mid, eid, pos);  // replace the new position or insert the moving object
    }
}

double SDB::velocity(){return 10.0;}
// return the distance to the landmark by matrix
double SDB::getDist(int lid, int eid, double pos){
    if (eid == -1) return DBL_MAX;
    int snid(G.edges[eid].snid), enid(G.edges[eid].enid);
    double elen(G.edges[eid].len);

    bitset<64> k1(getKey(snid, lid)), k2(getKey(enid, lid));

    double res = DBL_MAX;
    if (m.find(k1) != m.end()) res = min(res, m[k1] + pos);
    if (m.find(k2) != m.end()) res = min(res, m[k2] + elen - pos);
    if (lmrks[lid].first == eid) res = min(res, abs(lmrks[lid].second - pos));
    if (res != DBL_MAX) return res;
}

double SDB::getAggDist(int eid, double pos){
    if (eid == -1) return DBL_MAX;
    double aggV = agg(1, 2) == 2 ? DBL_MIN : agg(1, 2) == 1 ? DBL_MAX : 0;
    for (int i = 0; i < nl; i++){
        double d = getDist(i, eid, pos);
        aggV = agg(aggV, d);
    }
    return aggV;
}
#endif
