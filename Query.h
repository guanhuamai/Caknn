//
// Created by mgh on 3/26/17.
//

#ifndef CAKNNSR_QUERY_H
#define CAKNNSR_QUERY_H

#include <fstream>
#include <vector>
#include "Utility.h"
#include "Graph.h"

using namespace std;



bool readMovObj1(ifstream& mfile, vector<int>& old,
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
            double dist = Utility::euclidDist(coord.first, coord.second, x, y);
            double elen = Graph::getEdgeLen(newE[newE.size() - 1]);
            dist = dist > 0 ? dist : 0;
            dist = dist < elen ? dist : elen;
            newP.push_back(dist);

            getline(mfile, line);
        }
    }
    return true;
}


bool readMovObj2(ifstream& mfile, vector<int>& old,
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
            double dist = Utility::euclidDist(coord.first, coord.second, x, y);
            double elen = Graph::getEdgeLen(newE[newE.size() - 1]);
            dist = dist > 0 ? dist : 0;
            dist = dist < elen ? dist : elen;
            newP.push_back(dist);

            getline(mfile, line);
        }
    }
    return true;
}


bool readMovObj(ifstream& mfile, vector<int>& old,
                vector<int>& newI, vector<int>& newE, vector<double>& newP,
                string& line, bool flag){
    if (flag) return readMovObj1(mfile, old, newI, newE, newP, line);
    return readMovObj2(mfile, old, newI, newE, newP, line);
}


#endif //CAKNNSR_QUERY_H
