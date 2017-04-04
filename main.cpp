#include "Experiment.h"
using namespace std;

int main()
{
    chrono::time_point<chrono::system_clock> strt, end;

    Experiment::buildExperiment("../config/experiment.config");

//    freopen("stdout1", "w", stdout);

    strt = chrono::system_clock::now();
    Experiment::doExperiment();
    end  = chrono::system_clock::now();

    chrono::duration<double > elapsed = end - strt;

    cout << "total time cost: " << elapsed.count() * 1000 << endl;
    cout << "through put per 50ms: " << Utility::getThroughPut(int(elapsed.count() * 1000), 2507598) << endl;
    cout << "MAD hit: " << SafeRegion::getmadCnt() << endl;
    cout << "lc hit: " << SafeRegion::getlcCnt() << endl;

    Experiment::saveStatistics();
    Experiment::destructExperiment();

    return 0;
}
