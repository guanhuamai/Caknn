#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <chrono>
#include "SafeRegion.h"
#include "SLEExpansion.h"
#include "Query.h"
#include "IleMinExpansion.h"

using namespace std;


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

void sdb(BaseExpansion* sdb, vector<int> old, vector<int> newI, vector<int> newE, vector<double> newP){
    size_t no(old.size()), nn(newI.size());
    for (size_t i = 0; i < no; i++) {
        SafeRegion::update(old[i], -1, DBL_MAX);
        SafeRegion::expand(sdb);
    }
    for (size_t i = 0; i < nn; i++){
        SafeRegion::update(newI[i], newE[i], newP[i]);
        SafeRegion::expand(sdb);
//        Opf::display();
//        PartialMatrix::display();
    }
    Opf::display();
    Result::display();
}

long long getThroughPut(int timeCost, int taskNum){
    return ((long long)taskNum * 50) / timeCost;
}

void doExperiment(const char* path, const char* dataName,
                  unsigned int numLmrks, int numObj, int k, int objSpeed,
                  bool flag){
    stringstream nodePath, edgePath, lmrkPath, movObjPath;
    nodePath << path << dataName << ".cnode";
    edgePath << path << dataName << ".cedge";
    lmrkPath << path << dataName << ".lmrk";
    movObjPath << path << dataName << "." << to_string(numObj).c_str()
               << "." <<  to_string(objSpeed).c_str() << ".mobj";

    const char* DBNAME = "bt.mobj";
    MovingObject::setDB(DBNAME);
    vector<pair<int, double>> lmrks = readLmrks(lmrkPath.str());
    lmrks.resize(numLmrks);
    function<double (double, double)> aggfunc = Utility::aggmax;


    SafeRegion::buildSafeRegion(nodePath.str(), edgePath.str(), lmrks, k, aggfunc);


    vector<int> old, mobjI, mobjE;
    vector<double > mobjPos;
    string line = "";
    BaseExpansion* sle = new SLEExpansion(numLmrks);

    string movObjPathStr = movObjPath.str();
    ifstream mfile(movObjPath.str().c_str());
    while (readMovObj(mfile, old, mobjI, mobjE, mobjPos, line, flag)) {
        printf("things are %zu %zu %zu %zu\n", old.size(), mobjI.size(), mobjE.size(), mobjPos.size());
        sdb(sle, old, mobjI, mobjE, mobjPos);
    }

    sle->display();

    delete sle;
    mfile.close();
}

int main()
{
//    freopen("log", "w", stdout);
    chrono::time_point<chrono::system_clock> strt, end;
    strt = chrono::system_clock::now();
    doExperiment("/home/mgh/codes/C++/CaknnSR/data/", "alaska", 2, 50000, 3, 50, true);
    end  = chrono::system_clock::now();
    chrono::duration<double > elapsed = end - strt;
    cout <<  "through put per 50ms: " << getThroughPut(int(elapsed.count() * 1000), 2507598) << endl;
    return 0;
}
