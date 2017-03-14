#ifndef __GRAPH
#define __GRAPH
#include <vector>
#include <float.h>


using namespace std;


class Edge{

public:
    int eid;
    double len;
    int snid;
    int enid;

    Edge(int i, double l, int s, int e): eid(i), len(l), snid(s), enid(e){}
    Edge(): eid(0), len(0), snid(0), enid(0){}
};

class Node{

public:
    int nid;
    double x;
    double y;
    vector<int> adjNodes;
    vector<int> adjEdges;
    Node(): nid(0){}
    Node(int i): nid(i){}
    Node(int i, double x, double y): nid(i), x(x), y(y){}
};


class Graph{

public:
    vector<Edge> edges;
    vector<Node> nodes;
    vector<vector<double> > matrix; // node distance matrix

    Graph(){}
};


#endif // __GRAPH

