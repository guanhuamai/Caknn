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


    static void findLoc (vector<pair<int, double>>& v, int id, size_t& s){
        while (s < v.size() && v[s].first != id) s++;
    }


    static void replaceV(vector<pair<int, double>>& v, int i, pair<int, double> p){
        swap(v[i], v[v.size() - 1]), v.pop_back();
        v.push_back(p);
    }

    static void eraseV(vector<pair<int, double>>& v, int i){swap(v[i], v[v.size() - 1]), v.pop_back();}

    static bitset<64> getKey(int nid, int lid){bitset<64> k(nid); k = k<<32, k|=lid;  return k;}
};

struct PairLess{bool operator () (pair<int, double >& a, pair<int, double>& b) const { return a.second < b.second;}};

class Opf{  // use to monitor safe region objects
private:

    // moving objects id inside of the safe region, or protential future objects
    priority_queue<pair<int, double >, vector<pair<int, double >>, PairLess> heapOpf;

    unordered_map<int, pair<int, double >> opf;

    static Opf* opfPtr;

    Opf(){};  // ban initialization

public:

    static void insert(int mid, int eid, int pos, int dist){
        opfPtr->opf[mid] = pair<int, double >(eid, pos);
        opfPtr->heapOpf.push(pair<int, double >(mid, dist));
    }

    static pair<int, double> getMin(){ // return the first element found in opf
        while (!opfPtr->heapOpf.empty()){
            if (opfPtr->opf.find(opfPtr->heapOpf.top().first) == opfPtr->opf.end())
                opfPtr->heapOpf.pop();
            return pair<int, double >(opfPtr->heapOpf.top().first, opfPtr->heapOpf.top().second);
        }

        return pair<int, double >(INT_MAX, DBL_MAX);
    }

    static void erase(int mid){
        opfPtr->opf.erase(mid);
    }

    static bool exist(int mid){
        return opfPtr->opf.find(mid) == opfPtr->opf.end();
    }
};

class Result{
private:
    static bool cmp(pair<int, double >& p1, pair<int, double >& p2){
        return p1.second < p2.second;
    }

    vector<pair<int, double>> res;  //result.

    int k;

    static Result* resPtr;

public:

    static void resInit(int k){
        resPtr = new Result();
        resPtr->k = k;
        resPtr->res.clear();
    }

    static double getKthDist(){
        if (resPtr->res.size() >= resPtr->k) return resPtr->res[0].second;
        return DBL_MAX;
    }

    static bool isExist(int mid){
        for (const auto &e : resPtr->res){
            if (e.first == mid) return true;
        }
        return false;
    }

    static void pop(){
        pop_heap(resPtr->res.begin(), resPtr->res.end(), cmp);
        resPtr->res.pop_back();
    }

    static void push(pair<int, double >& p){
        resPtr->res.push_back(p);
        push_heap(resPtr->res.begin(), resPtr->res.end(), cmp);
    }

    static void erase(int mid){
        int i = 0;
        while (i < resPtr->res.size()){
            if (resPtr->res[i].first == mid) break;
            i++;
        }
        if (i < resPtr->res.size()){
            resPtr->res.erase(resPtr->res.begin() + i);
        }
    }




};

#endif //CAKNNSR_UTILITY_H
