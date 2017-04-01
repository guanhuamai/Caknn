//
// Created by mgh on 3/24/17.
//

#ifndef CAKNNSR_RESULT_H
#define CAKNNSR_RESULT_H


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



class Result{
private:
    static bool cmp(pair<int, double >& p1, pair<int, double >& p2){
        return p1.second < p2.second;
    }

    vector<pair<int, double>> res;  //result.

    int k;

    static Result* resPtr;

    Result(){}

public:

    static void buildResult(int k){
        Result::resPtr = new Result();
        resPtr->k = k;
        resPtr->res.clear();
    }

    static double getKthDist(){
        if (resPtr->res.size() >= resPtr->k) return resPtr->res[0].second;
        return DBL_MAX;
    }

    static bool exist(int mid){
        for (const auto &e : resPtr->res){
            if (e.first == mid) return true;
        }
        return false;
    }

    static int getK(){
        return resPtr->k;
    }

    static void rebuild(vector<pair<int, double>>& candidates){
        resPtr->res.clear();
        for(const auto& e: candidates){
            push(e);
        }
    }

    static void pop(){
        pop_heap(resPtr->res.begin(), resPtr->res.end(), cmp);
        resPtr->res.pop_back();
    }

    static void push(pair<int, double > p){
        resPtr->res.push_back(p);
        make_heap(resPtr->res.begin(), resPtr->res.end(), cmp);
        while(resPtr->res.size() > resPtr->k){
            pop();
        }
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
        make_heap(resPtr->res.begin(), resPtr->res.end(), cmp);
    }

    static void display(){
        for (auto p: resPtr->res){
            cout << "mid: " << p.first << "   aggregate value: " << p.second << endl;
        }

    }

};

Result* Result::resPtr = NULL;


#endif //CAKNNSR_RESULT_H
