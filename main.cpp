#include "Experiment.h"
using namespace std;

//defect, the configuration comes as a splitted string...
string getInputString(char* splitStr[], int len){
    string res;
    for (int i = 1; i < len; i++){
        if (i != 1) res += " ";
        res += splitStr[i];
    }
    return res;
}


int main(int argc, char* argv[])
{
    chrono::time_point<chrono::system_clock> strt, end;

    string configStr = getInputString(argv, argc);

    cout << "--------------------------start running c++----------------" << endl;
    cout << "running configuration string: " << endl << configStr << endl;
    cout << "please confirmed" << endl << endl;

    Experiment::buildExperiment(configStr);

//    freopen("stdout1", "w", stdout);

    strt = chrono::system_clock::now();
    Experiment::doExperiment();
    end  = chrono::system_clock::now();

    chrono::duration<double > elapsed = end - strt;

    cout << "total time cost: " << elapsed.count() * 1000 << endl;
    cout << "MAD hit: " << SafeRegion::getmadCnt() << endl;
    cout << "lc hit: " << SafeRegion::getlcCnt() << endl;

    Experiment::saveStatistics();
    Experiment::destructExperiment();

    cout << "------------------------end running main----------------------" << endl;

    return 0;
}
