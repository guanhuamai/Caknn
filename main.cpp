#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <chrono>
#include "SafeRegion.h"

#include "SLEExpansion.h"

using namespace std;

bool readMovObj(ifstream& mfile, vector<int>& old,
                vector<int>& newI, vector<int>& newE, vector<double>& newP,
                string& line){

    newI.clear(), old.clear(), newE.clear(), newP.clear();

    int round = -1;

    if (line == "") getline(mfile, line);

    if (line == "") return false;  // when the line is "" return false to finish checking...

    if (mfile.is_open()){
        while (line != ""){
            vector<string> splitStr = Utility::split(line, '\t');

            int tmpround = Utility::my_stoi(splitStr[4]);

            if (tmpround != round && round != -1) return true;

            round = tmpround;

            int  oid(Utility::my_stoi(splitStr[1])),
                    snid(Utility::my_stoi(splitStr[7])),
                    enid(Utility::my_stoi(splitStr[8]));
            double x(Utility::my_stof(splitStr[5])), y(Utility::my_stof(splitStr[6]));

            if (snid > enid) swap(snid, enid);

            if (splitStr[0] == "disappearpoint")  old.push_back(oid);

            newI.push_back(oid);

            vector<int> adjN = Graph::getAdjacentNode(snid);  //g.nodes[snid].adjNodes;
            vector<int> adjE = Graph::getAdjacentEdge(snid);

            for (int i = 0; i < adjN.size(); i++){
                if (adjN[i] == enid) {
                    newE.push_back(adjE[i]);
                    break;
                }
            }
            pair<double, double> coord = Graph::getNodeCoordinates(snid);
            newP.push_back(Utility::euclidDist(coord.first, coord.second, x, y));

            getline(mfile, line);
        }
    }
    return true;
}

vector<pair<int, double>> readLmrks(string lmrkF){
    vector<pair<int, double>> res;

    vector<string> lmrkStr = Utility::readLines(lmrkF);
    int n(Utility::my_stoi(lmrkStr[0]));

    for (int i = 1; i <= n; i++){
        vector<string> splitStr = Utility::split(lmrkStr[i], '\t');
        int snid(Utility::my_stoi(splitStr[0])), enid(Utility::my_stoi(splitStr[1])),
                eid(Utility::my_stoi(splitStr[2]));

        double len1(Utility::my_stof(splitStr[3])),
                len2(Utility::my_stof(splitStr[4]));

        double len = snid < enid ? len1 : len2;
        res.push_back(pair<int, double> (eid, len));
    }
    return res;
}
//
//
//
void sdb(BaseExpansion* sdb, vector<int> old, vector<int> newI, vector<int> newE, vector<double> newP){
    size_t no(old.size()), nn(newI.size());
    for (size_t i = 0; i < no; i++) {
        SafeRegion::update(old[i], -1, DBL_MAX);
        SafeRegion::expand(sdb);
    }
    for (size_t i = 0; i < nn; i++){
        SafeRegion::update(newI[i], newE[i], newP[i]);
        SafeRegion::expand(sdb);
    }
}

void doExperiment(const char* path, const char* dataName,
                  int numLmrks, int numObj, int k, int objSpeed, int numQueries){
    stringstream nodePath, edgePath, lmrkPath, movObjPath;
    nodePath << path << dataName << ".cnode";
    edgePath << path << dataName << ".cedge";
    lmrkPath << path << dataName << ".lmrk";
    movObjPath << path << dataName << "." << to_string(numObj).c_str()
               << "." <<  to_string(objSpeed).c_str() << ".mobj";

    const char* DBNAME = "bt.mobj";
    MovingObject::setDB(DBNAME);


    Graph::graphFactory(nodePath.str(), edgePath.str());
    vector<pair<int, double>> lmrks = readLmrks(lmrkPath.str());
    lmrks.resize((unsigned)numLmrks);


    function<double (double, double)> aggfunc = Utility::aggsum;
    SafeRegion::buildSafeRegion(nodePath.str(), edgePath.str(), lmrks, k, aggfunc);

    ifstream mfile(movObjPath.str().c_str());

    vector<int> old, mobjI, mobjE;
    vector<double > mobjPos;

    string line = "";

    BaseExpansion* sle = new SLEExpansion();


    while (readMovObj(mfile, old, mobjI, mobjE, mobjPos, line)) {
        printf("things are %zu %zu %zu %zu\n", old.size(), mobjI.size(), mobjE.size(), mobjPos.size());
        sdb(sle, old, mobjI, mobjE, mobjPos);
    }

    delete sle;

    mfile.close();
}

int main()
{
    Graph::graphFactory("", "");
    chrono::time_point<chrono::system_clock> strt, end;
    strt = chrono::system_clock::now();
    doExperiment("/home/mgh/codes/C++/CaknnSR/data/", "alaska", 1, 50000, 3, 50, 50000);
    end  = chrono::system_clock::now();
    chrono::duration<double > elapsed = end - strt;
    return 0;
}
