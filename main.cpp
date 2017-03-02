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

Graph construct(string nodeF, string edgeF){
    Graph res;
    vector<string> nodeStr = readLines(nodeF);
    vector<string> edgeStr = readLines(edgeF);
    int n(my_stoi(nodeStr[0])), m(my_stoi(edgeStr[0]));
    for (int i = 1; i <= n; i++)res.nodes.push_back(Node(i - 1));

    for (int i = 1; i <= m; i++){
        vector<string> splitStr = split(edgeStr[i], '\t');
        int eid (my_stoi(splitStr[0])), snid(my_stoi(splitStr[1])), enid(my_stoi(splitStr[2]));
        double len(my_stof(splitStr[3]));
        res.edges.push_back(Edge(eid, len, snid, enid));
        res.nodes[snid].adjNodes.push_back(enid);
        res.nodes[snid].adjEdges.push_back(eid);

        res.nodes[enid].adjNodes.push_back(snid);
        res.nodes[enid].adjEdges.push_back(eid);
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


int main()
{
    Graph g = construct("/home/mgh/codes/C++/CaknnSR/test/test.cnode",
                        "/home/mgh/codes/C++/CaknnSR/test/test.cedge");

    vector<pair<int, double>> lmrks;
    lmrks.push_back(pair<int, double>(3, 2.0));
    lmrks.push_back(pair<int, double>(4, 2.0));

    function<double (double, double)> aggfunc = aggsum;
//    SLE* sle = new SLE(4, g, lmrks, aggfunc);
    vector<int> mobjI = {0, 1, 2, 3, 4};
    vector<int> mobjE = {3, 6, 5, 7, 1};
    vector<double > mobjPos = {3.0, 2.0, 3.0, 3.0, 3.0};

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


    ILE_SUM* ile_sum = new ILE_SUM(4, g, lmrks, aggfunc);

    sdb(ile_sum, vector<int>(), mobjI, mobjE, mobjPos);
    sdb(ile_sum, vector<int>{0}, vector<int>{}, vector<int>{}, vector<double >{});
    sdb(ile_sum, vector<int>{1}, vector<int>{}, vector<int>{}, vector<double >{});
    sdb(ile_sum, vector<int>{2}, vector<int>{}, vector<int>{}, vector<double >{});
    sdb(ile_sum, vector<int>{3}, vector<int>{}, vector<int>{}, vector<double >{});
    sdb(ile_sum, vector<int>{4}, vector<int>{}, vector<int>{}, vector<double >{});
    return 0;
}
