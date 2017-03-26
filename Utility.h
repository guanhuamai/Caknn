//
// Created by mgh on 3/20/17.
//

#ifndef CAKNNSR_UTILITY_H
#define CAKNNSR_UTILITY_H

#include <algorithm>
#include <vector>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <climits>
#include <cfloat>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

class Utility{
    template<typename Out>
    static void split(const std::string &s, char delim, Out result) {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            *(result++) = item;
        }
    }

    Utility(){}

public:

    static double aggmin(double a, double b){ return min(a, b);}

    static double aggmax(double a, double b){ return max(a, b);}

    static double aggsum(double a, double b){
        if (a != DBL_MAX && b != DBL_MAX) return a + b;
        else return DBL_MAX;
    }

    static std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, std::back_inserter(elems));
        return elems;
    }

    static vector<string> readLines(string fname){
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

    static int my_stoi(string s){istringstream iss(s); int res; iss >> res; return res;}

    static double my_stof(string s){istringstream iss(s); double res; iss >> res; return res;}

    static double euclidDist(const double& x1, const double& y1, const double& x2, const double& y2){
        double dx = x1 - x2;
        double dy = y1 - y2;
        return sqrt(dx * dx + dy * dy);
    }

};


#endif //CAKNNSR_UTILITY_H
