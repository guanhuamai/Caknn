//
// Created by mgh on 4/2/17.
//

#ifndef CAKNNSR_EXPERIMENT_H
#define CAKNNSR_EXPERIMENT_H

#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <chrono>
#include "SafeRegion.h"
#include "SLEExpansion.h"
#include "IleMinExpansion.h"
#include "IleSumExpansion.h"

using namespace std;

enum TRAFFIC_FILE_TYPE {BRINKHOFF, COMPACT};
enum EXPANSION_TYPE {SLE, ILE_MIN, ILE_SUM};
enum AGGREGATE_TYPE {AGG_MIN, AGG_MAX, AGG_SUM};

unordered_map<string, int> ENUM_NAME_MAP = {{"BRINKHOFF", 0}, {"COMPACT", 1},
                                            {"SLE", 0}, {"ILE_MIN", 1}, {"ILE_SUM", 2},
                                            {"AGG_MIN", 0}, {"AGG_MAX", 1}, {"AGG_SUM", 2}};

class Experiment{
private:
    string configFilePath;  // the path of the configuration file
    string projectPath;  // the path of the projects
    string dataName;  // the relative path of the dataset, and the data name
    string dbname;  // the database name storing the moving objects
    string logname;  // the name of the log

    string metaConfig; // the meta configure string

    unsigned numLmrks;
    unsigned numOfObjects;  // number of moving objects
    unsigned k;
    unsigned objSpeed;  // speed of moving objects, use brinkhoff to control
    bool useLC;  // whether to use LC
    bool useMAD;  // whether to use MAD


    chrono::duration<double > expandTime;  // total time consumed in expansions
    chrono::duration<double > updateTime;  // total time consumed in updates
    int expandCnt;  // number of expand
    int updateCnt;  // number of update

    ofstream logFileStream;

    TRAFFIC_FILE_TYPE traffic_file_type;  // the file type of the moving object input file
    EXPANSION_TYPE expansion_type;  // the expansion type supported by the algorithm
    AGGREGATE_TYPE aggregate_type;  // the aggregate function supported in the query


    static Experiment* experiment;


    Experiment(string configFile){
        configFilePath = configFile;

        string timeStr = to_string(time(NULL));

        ifstream configFileStream;

        configFileStream.open(configFile.c_str());
        getline(configFileStream, metaConfig);
        configFileStream.close();

        stringstream configStream(metaConfig);

        configStream >> projectPath;
        configStream >> dataName;
        configStream >> dbname;  dbname = timeStr + "." + dbname;
        configStream >> logname;
        configStream >> numLmrks;
        configStream >> numOfObjects;
        configStream >> k;
        configStream >> objSpeed;


        string tmp;
        configStream >> tmp;
        useLC = tmp == "useLC";

        configStream >> tmp;
        useMAD = tmp == "useMAD";

        configStream >> tmp;
        traffic_file_type = (TRAFFIC_FILE_TYPE) (ENUM_NAME_MAP[tmp]);
        configStream >> tmp;
        expansion_type = (EXPANSION_TYPE) (ENUM_NAME_MAP[tmp]);
        configStream >> tmp;
        aggregate_type = (AGGREGATE_TYPE) (ENUM_NAME_MAP[tmp]);

        expandTime = chrono::duration<double>::zero();
        expandCnt = 0;
        updateTime = chrono::duration<double>::zero();
        updateCnt = 0;

        logFileStream.open(logname.c_str(), ios_base::app);
    }

    static void sdb(BaseExpansion* sdb, vector<int> old,
                     vector<int> newI, vector<int> newE, vector<double> newP);

    static void destructEverything(){
        Graph::destructGraph();
        LC::destructLC();
        MAD::destructMAD();
        MovingObject::destructMovingObject();
        Opf::destructOpf();
        PartialMatrix::destructPartialMatrix();
        Result::destructResult();
        SafeRegion::destructSafeRegion();
    }

    // trivial methods below
    static bool readMovObj1(ifstream& mfile, vector<int>& old,
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

                if (splitStr[0] == "disappearpoint")  {
                    old.push_back(oid);
                }else {

                    newI.push_back(oid);

                    vector<int> adjN = Graph::getAdjacentNode(snid);  //g.nodes[snid].adjNodes;
                    vector<int> adjE = Graph::getAdjacentEdge(snid);

                    for (int i = 0; i < adjN.size(); i++) {
                        if (adjN[i] == enid) {
                            newE.push_back(adjE[i]);
                            break;
                        }
                    }
                    pair<double, double> coord = Graph::getNodeCoordinates(snid);
                    double dist = Utility::euclidDist(coord.first, coord.second, x, y);
                    double elen = Graph::getEdgeLen(newE[newE.size() - 1]);
                    dist = dist > 0 ? dist : 0;
                    dist = dist < elen ? dist : elen;
                    newP.push_back(dist);
                }

                getline(mfile, line);
            }
        }
        return true;
    }

    static bool readMovObj2(ifstream& mfile, vector<int>& old,
                            vector<int>& newI, vector<int>& newE, vector<double>& newP,
                            string& line){

        newI.clear(), old.clear(), newE.clear(), newP.clear();
        int n = 0;
        mfile >> n;
        for (int i = 0; i < n; i++){
            int mid, eid;
            double pos;
            mfile >> mid >> eid >> pos;
            newI.push_back(mid);
            newE.push_back(eid);
            newP.push_back(pos);
        }
        return n > 0;
    }

    static bool readMovObj(ifstream& mfile, vector<int>& old,
                           vector<int>& newI, vector<int>& newE, vector<double>& newP,
                           string& line, TRAFFIC_FILE_TYPE trafficFileType){
        if (trafficFileType == BRINKHOFF) return readMovObj1(mfile, old, newI, newE, newP, line);
        else if (trafficFileType == COMPACT) return readMovObj2(mfile, old, newI, newE, newP, line);
        else return false;
    }


public:

    static void buildExperiment(string configFile){
        destructExperiment();
        experiment = new Experiment(configFile);


        stringstream nodePath, edgePath, lmrkPath;
        nodePath << experiment->projectPath << experiment->dataName << ".cnode";
        edgePath << experiment->projectPath << experiment->dataName << ".cedge";
        lmrkPath << experiment->projectPath << experiment->dataName << ".lmrk";

        MovingObject::buildMovingObject(experiment->dbname);

        Graph::buildGraph(nodePath.str(), edgePath.str());
        auto lmrks = Utility::readLmrks(lmrkPath.str());
        lmrks.resize(experiment->numLmrks);
        Graph::initLmrks(lmrks);

        LC::buildLC((unsigned)Graph::getNumEdges(), (unsigned)Graph::getNumLmrks());

        MAD::buildMAD((unsigned)Graph::getNumEdges());

        if(experiment->aggregate_type == AGG_MIN){
            SafeRegion::buildSafeRegion(experiment->numLmrks, Utility::aggmin,
                                        experiment->useLC, experiment->useMAD);
        }else if (experiment->aggregate_type == AGG_MAX){
            SafeRegion::buildSafeRegion(experiment->numLmrks, Utility::aggmax,
                                        experiment->useLC, experiment->useMAD);
        }else if (experiment->aggregate_type == AGG_SUM){
            SafeRegion::buildSafeRegion(experiment->numLmrks, Utility::aggsum,
                                        experiment->useLC, experiment->useMAD);
        }

        PartialMatrix::buildPartialMatrix();

        Opf::buildOpf();

        Result::buildResult(experiment->k);
    }

    static void destructExperiment(){
        destructEverything();
        if (experiment != NULL){
            experiment->logFileStream.close();
            delete experiment;
            experiment = NULL;
        }
    }

    static void doExperiment();

    static void saveStatistics(){

        ifstream configFileStream;
        configFileStream.open(experiment->configFilePath.c_str());
        string config;
        getline(configFileStream, config);

        experiment->logFileStream << "--------------------------------" << endl;

        experiment->logFileStream << config << endl;

        experiment->logFileStream << experiment->expandCnt
                                  << "\t" << experiment->expandTime.count() * 1000
                                  << "\t" << experiment->updateCnt
                                  << "\t" << experiment->updateTime.count() * 1000
                                  << "\t" << SafeRegion::getlcCnt()
                                  << "\t" << SafeRegion::getmadCnt()
                                  << endl;
    }

};

Experiment* Experiment::experiment = NULL;

void Experiment::sdb(BaseExpansion* sdb, vector<int> old,
                     vector<int> newI, vector<int> newE, vector<double> newP){


    chrono::time_point<chrono::system_clock> strt, end;

    size_t no(old.size()), nn(newI.size());
    for (size_t i = 0; i < no; i++) {
        strt = chrono::system_clock().now();
        SafeRegion::update(old[i], -1, DBL_MAX);
        end = chrono::system_clock().now();
        experiment->updateTime += end - strt;
        experiment->updateCnt += 1;

        strt = chrono::system_clock().now();
        int numExpandedEdges = SafeRegion::expand(sdb);
        end = chrono::system_clock().now();
        experiment->expandTime += end - strt;
        experiment->expandCnt += numExpandedEdges == 0 ? 0 : 1;
    }
    for (size_t i = 0; i < nn; i++){
        strt = chrono::system_clock().now();
        SafeRegion::update(newI[i], newE[i], newP[i]);
        end = chrono::system_clock().now();
        experiment->updateTime += end - strt;
        experiment->updateCnt += 1;

        strt = chrono::system_clock().now();
        int numExpandedEdges = SafeRegion::expand(sdb);
        end = chrono::system_clock().now();
        experiment->expandTime += end - strt;
        experiment->expandCnt += numExpandedEdges == 0 ? 0 : 1;
    }
//    PartialMatrix::display();
//    Opf::display();
//    Result::display();
}

void Experiment::doExperiment(){

    vector<int> old, mobjI, mobjE;
    vector<double > mobjPos;
    string line = "";

    BaseExpansion* sle = NULL;
    if (experiment->expansion_type == SLE){
        sle = new SLEExpansion(experiment->numLmrks);
    }else if (experiment->expansion_type == ILE_MIN){
        sle = new IleMinExpansion(experiment->numLmrks);
    }else if (experiment->expansion_type == ILE_SUM){
        sle = new IleSumExpansion(experiment->numLmrks);
    }

    stringstream movObjTrafficPath;  //
    movObjTrafficPath << experiment->projectPath << experiment->dataName << "."
                      << to_string(experiment->numOfObjects).c_str()
                      << "." <<  to_string(experiment->objSpeed).c_str() << ".mobj";

    ifstream mfile(movObjTrafficPath.str().c_str());

    int cnt = 0;
    while (readMovObj(mfile, old, mobjI, mobjE, mobjPos, line, experiment->traffic_file_type)) {
        sdb(sle, old, mobjI, mobjE, mobjPos);

        cnt += mobjI.size() + old.size();
        if (cnt > 500000){
            cnt -= 500000;
            printf("things are %zu %zu %zu %zu\n", old.size(), mobjI.size(), mobjE.size(), mobjPos.size());
            Opf::display();
            Result::display();
        }
    }

    delete sle;
    mfile.close();
}

#endif //CAKNNSR_EXPERIMENT_H
