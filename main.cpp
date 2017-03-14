#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include "Expansion.h"
using namespace std;

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

vector<string> readLines(string fname){
    string line;
    ifstream myfile (fname.c_str());
    vector<string> res;
    if (myfile.is_open())
        {
            while ( getline (myfile,line)) res.push_back(line);
            myfile.close();
        }
    else cout << "Unable to open file" << endl;
    return res;
}


int my_stoi(string s){istringstream iss(s); int res; iss >> res; return res;}

double my_stof(string s){istringstream iss(s); double res; iss >> res; return res;}

double euclidDist(const double& x1, const double& y1, const double& x2, const double& y2){
    double dx = x1 - x2;
    double dy = y1 - y2;
    return sqrt(dx * dx + dy * dy);
}

void readMovObj(ifstream& mfile, vector<int>& old,
                vector<int>& newI, vector<int>& newE, vector<double>& newP,
                string& line,
                Graph& g){

    newI.clear(), old.clear(), newE.clear(), newP.clear();

    int round = -1;
    if (mfile.is_open()){
        while (line != "" || getline(mfile, line)){
            vector<string> splitStr = split(line, '\t');

            int tmpround = my_stoi(splitStr[4]);

            if (tmpround != round && round != -1) break;

            round = tmpround;

            int  oid(my_stoi(splitStr[1])), snid(my_stoi(splitStr[7])), enid(my_stoi(splitStr[8]));
            double x(my_stof(splitStr[5])), y(my_stof(splitStr[6]));

            if (snid > enid) swap(snid, enid);

            if (splitStr[0] == "point" || splitStr[0] == "disappearpoint")  old.push_back(oid);

            newI.push_back(oid);

            vector<int>& adjN = g.nodes[snid].adjNodes;

            for (int i = 0; i < adjN.size(); i++){
                if (adjN[i] == enid) {
                    newE.push_back(g.nodes[snid].adjEdges[i]);
                    break;
                }
            }

            newP.push_back(euclidDist(g.nodes[snid].x, g.nodes[snid].y, x, y));
        }
    }
}

Graph construct(string nodeF, string edgeF){
    Graph res;
    vector<string> nodeStr = readLines(nodeF);
    vector<string> edgeStr = readLines(edgeF);
    int n(my_stoi(nodeStr[0])), m(my_stoi(edgeStr[0]));
    for (int i = 1; i <= n; i++){
        vector<string> splitStr = split(nodeStr[i], '\t');
        res.nodes.push_back(
                Node(my_stoi(splitStr[0]), my_stof(splitStr[1]), my_stof(splitStr[2])));
    }

    for (int i = 1; i <= m; i++){
        vector<string> splitStr = split(edgeStr[i], '\t');
        int eid (my_stoi(splitStr[0])), snid(my_stoi(splitStr[1])), enid(my_stoi(splitStr[2]));
        double len(my_stof(splitStr[3]));

        if (snid > enid) swap(snid, enid);

        res.edges.push_back(Edge(eid, len, snid, enid));
        res.nodes[snid].adjNodes.push_back(enid);
        res.nodes[snid].adjEdges.push_back(eid);

        res.nodes[enid].adjNodes.push_back(snid);
        res.nodes[enid].adjEdges.push_back(eid);
    }
    return res;
}

vector<pair<int, double>> construct(string lmrkF){
    vector<pair<int, double>> res;

    vector<string> lmrkStr = readLines(lmrkF);
    int n(my_stoi(lmrkStr[0]));

    for (int i = 1; i <= n; i++){
        vector<string> splitStr = split(lmrkStr[i], '\t');
        int snid(my_stoi(splitStr[0])), enid(my_stoi(splitStr[1])),
                eid(my_stoi(splitStr[2]));

        double len1(my_stof(splitStr[3])), len2(my_stof(splitStr[4]));

        double len = snid < enid ? len1 : len2;
        res.push_back(pair<int, double> (eid, len));
    }
    return res;
}

double aggmin(double a, double b){ return min(a, b);}

double aggmax(double a, double b){ return max(a, b);}

double aggsum(double a, double b){
    if (a != DBL_MAX && b != DBL_MAX) return a + b;
    else return DBL_MAX;
}


void sdb(SDB* sdb, vector<int> old, vector<int> newI, vector<int> newE, vector<double> newP){
    size_t no(old.size()), nn(newI.size());
    for (size_t i = 0; i < no; i++) {
        double rold = sdb->r;
        sdb->update(old[i], -1, DBL_MAX);
        if ((rold == DBL_MAX && sdb->r != DBL_MAX) || sdb->r > rold) sdb->expand();
        else printf("no expanding old radius: %lf, new radius %lf\n", rold, sdb->r);
    }
    for (size_t i = 0; i < nn; i++){
        double rold = sdb->r;
        sdb->update(newI[i], newE[i], newP[i]);
        if ((rold == DBL_MAX && sdb->r != DBL_MAX) || sdb->r > rold) sdb->expand();
        else printf("no expanding old radius: %lf, new radius %lf\n", rold, sdb->r);
    }
    printf("---------------the result ------------\n");
    for (size_t i = 0; i < sdb->res.size(); i++){
        pair<int, double >& res = sdb->res[i];
        printf("%d %lf\n", res.first, res.second);
    }
}

void doExperiment(const char* path, const char* dataName,
                  int numLmrks, int numObj, int k, int objSpeed, int numQueries){
    stringstream nodePath(path), edgePath(path), lmrkPath(path), movObjPath(path);
    nodePath << dataName << ".cnode";
    edgePath << dataName << ".cedge";
    lmrkPath << dataName << ".lmrk";
    movObjPath << dataName << "." << to_string(numObj).c_str()
               << "." <<  to_string(objSpeed).c_str() << ".mobj";

    Graph g = construct(nodePath.str(), edgePath.str());
    vector<pair<int, double>> lmrks = construct(lmrkPath.str());
    lmrks.reserve((unsigned)numLmrks);

    ifstream mfile(movObjPath.str().c_str());

    vector<int> old, mobjI, mobjE;
    vector<double > mobjPos;

    string line = "";

    function<double (double, double)> aggfunc = aggsum;
    SLE* sle = new SLE((unsigned) k, g, lmrks, aggfunc);

    while (mobjI.size() != 0) {
        readMovObj(mfile, old, mobjI, mobjE, mobjPos, line, g);
        sdb(sle, old, mobjI, mobjE, mobjPos);
    }

    mfile.close();
}

int main()
{
    doExperiment("/home/mgh/codes/C++/CaknnSR/data/", "alaska", 1, 500000, 3, 50, 50000);

//    Graph g = construct("/home/mgh/codes/C++/CaknnSR/data/alaska.cnode",
//                        "/home/mgh/codes/C++/CaknnSR/data/alaska.cedge");
//
//    vector<pair<int, double>> lmrks = construct("/home/mgh/codes/C++/CaknnSR/data/alaska.lmrk");
//
//    function<double (double, double)> aggfunc = aggsum;
//    vector<int> mobjI = {0, 1, 2, 3, 4};
//    vector<int> mobjE = {3, 6, 5, 7, 1};
//    vector<double > mobjPos = {3.0, 2.0, 3.0, 3.0, 3.0};
//
//    const char* DBNAME = "bt.mobj3";
//
//    MovingObject::setDB(DBNAME);
//
//
//
//    SLE* sle = new SLE(4, g, vector<pair<int, double>>{lmrks[0], lmrks[1], lmrks[2], lmrks[3]}, aggfunc);
//    sdb(sle, vector<int>(), mobjI, mobjE, mobjPos);
//    sdb(sle, vector<int>{0}, vector<int>{}, vector<int>{}, vector<double >{});
//    sdb(sle, vector<int>{1}, vector<int>{}, vector<int>{}, vector<double >{});
//    sdb(sle, vector<int>{2}, vector<int>{}, vector<int>{}, vector<double >{});
//    sdb(sle, vector<int>{3}, vector<int>{}, vector<int>{}, vector<double >{});
//    sdb(sle, vector<int>{4}, vector<int>{}, vector<int>{}, vector<double >{});


//    ILE_MIN* ile_min = new ILE_MIN(4, g, lmrks, aggfunc);
//
//    sdb(ile_min, vector<int>(), mobjI, mobjE, mobjPos);
//    sdb(ile_min, vector<int>{0}, vector<int>{}, vector<int>{}, vector<double >{});
//    sdb(ile_min, vector<int>{1}, vector<int>{}, vector<int>{}, vector<double >{});
//    sdb(ile_min, vector<int>{2}, vector<int>{}, vector<int>{}, vector<double >{});
//    sdb(ile_min, vector<int>{3}, vector<int>{}, vector<int>{}, vector<double >{});
//    sdb(ile_min, vector<int>{4}, vector<int>{}, vector<int>{}, vector<double >{});


//    ILE_SUM* ile_sum = new ILE_SUM(4, g, vector<pair<int, double>>{lmrks[0], lmrks[1], lmrks[2], lmrks[3]}, aggfunc);
//
//    sdb(ile_sum, vector<int>(), mobjI, mobjE, mobjPos);
//    sdb(ile_sum, vector<int>{0}, vector<int>{}, vector<int>{}, vector<double >{});
//    sdb(ile_sum, vector<int>{1}, vector<int>{}, vector<int>{}, vector<double >{});
//    sdb(ile_sum, vector<int>{2}, vector<int>{}, vector<int>{}, vector<double >{});
//    sdb(ile_sum, vector<int>{3}, vector<int>{}, vector<int>{}, vector<double >{});
//    sdb(ile_sum, vector<int>{4}, vector<int>{}, vector<int>{}, vector<double >{});

    MovingObject::closeDB();
    return 0;
}
