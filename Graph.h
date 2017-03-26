#ifndef __GRAPH
#define __GRAPH

#include <algorithm>
#include <vector>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <climits>
#include <cfloat>
#include "Utility.h"

using namespace std;



class Graph{ // singleton

    // can't be modified without graphFactory
    struct Edge{
        int eid;
        double len;
        int snid;
        int enid;

        Edge(int i, double l, int s, int e): eid(i), len(l), snid(s), enid(e){}
        Edge(): eid(0), len(0), snid(0), enid(0){}
    };

    struct Node{
        int nid;
        double x;
        double y;
        Node(): nid(0){}
        Node(int i): nid(i){}
        Node(int i, double x, double y): nid(i), x(x), y(y){}

        vector<int> adjNodes;
        vector<int> adjEdges;

    };

    vector<Edge> edges;
    vector<Node> nodes;

    unordered_map<int, vector<int>> hsLmrk;  // landmark hash key: eid, val: lmid
    vector<pair<int, double>> lmrks;  // landmarks   pair: 1st

    Graph(){}  // singleton

    static Graph* graph;

    static bool isInit;

public:

    //static constructor
    static void graphFactory(string nodeF, string edgeF){

        if (isInit)  return;
        isInit = true;

        graph = new Graph();
        vector<string> nodeStr = Utility::readLines(nodeF);
        vector<string> edgeStr = Utility::readLines(edgeF);
        int n(Utility::my_stoi(nodeStr[0])), m(Utility::my_stoi(edgeStr[0]));
        for (int i = 1; i <= n; i++){
            vector<string> splitStr = Utility::split(nodeStr[i], '\t');
            graph->nodes.push_back(
                    Node(Utility::my_stoi(splitStr[0]),
                         Utility::my_stof(splitStr[1]),
                         Utility::my_stof(splitStr[2])));
        }

        for (int i = 1; i <= m; i++){
            vector<string> splitStr = Utility::split(edgeStr[i], '\t');
            int eid (Utility::my_stoi(splitStr[0])),
                snid(Utility::my_stoi(splitStr[1])),
                enid(Utility::my_stoi(splitStr[2]));
            double len(Utility::my_stof(splitStr[3]));

            if (snid > enid) swap(snid, enid);

            graph->edges.push_back(Edge(eid, len, snid, enid));
            graph->nodes[snid].adjNodes.push_back(enid);
            graph->nodes[snid].adjEdges.push_back(eid);

            graph->nodes[enid].adjNodes.push_back(snid);
            graph->nodes[enid].adjEdges.push_back(eid);
        }
    }

    //static visit
    static vector<int> getNeighbors(int nid) {
        if (nid < 0 || nid >= graph->nodes.size()) return vector<int>();
        vector<int> res = graph->nodes[nid].adjNodes;
        return res;
    };

    static vector<int> getAdjacentEdge(int nid){
        vector<int> res;
        if (nid >= 0 && nid < graph->nodes.size())
            res = graph->nodes[nid].adjEdges;
        return res;
    }

    static vector<int> getAdjacentNode(int nid){
        vector<int> res;
        if (nid >= 0 && nid < graph->nodes.size())
            res = graph->nodes[nid].adjNodes;
        return res;
    }

    static int getEdgeStart(int eid){
        if (eid < 0 || eid >= graph->edges.size()) return -1;
        return graph->edges[eid].snid;
    }

    static int getEdgeEnd(int eid){
        if (eid < 0 || eid >= graph->edges.size()) return -1;
        return graph->edges[eid].enid;
    }

    static double getEdgeLen(int eid){
        if (eid < 0 || eid >= graph->edges.size()) return -1;
        return graph->edges[eid].len;
    }

    static pair<double, double > getNodeCoordinates(int nid){
        if (nid < 0 || nid >= graph->nodes.size())
            return pair<double, double >(DBL_MAX, DBL_MAX);
        return pair<int, int>(graph->nodes[nid].x, graph->nodes[nid].y);
    };

    static void initLmrks(vector<pair<int, double >>& lmrks){
        graph->lmrks = lmrks;
        graph->hsLmrk.clear();
        for (int i = 0; i < lmrks.size(); i++){
            if (graph->hsLmrk.find(i) == graph->hsLmrk.end()){
                graph->hsLmrk[i] = vector<int>();
            }
            graph->hsLmrk[i].push_back(i);
        }
    }

    static vector<int> getEdgeLmrks(int eid){
        return graph->hsLmrk[eid];
    }

    static pair<int, double > getLmrkById(int lmrkId){
        return graph->lmrks[lmrkId];
    };

    static int getNumLmrks(){
        return graph->lmrks.size();
    }
};

bool Graph::isInit = false;
Graph* Graph::graph = NULL;

//Graph::graph = NULL;
//Graph::isInit = false;

#endif // __GRAPH

