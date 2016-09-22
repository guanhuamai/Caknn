#include <iostream>
#include "Aggregate/DistMatrix.h"

using namespace std;

int main()
{
    cout << "Hello world!" << endl;


    DistMatrix* mt = new DistMatrix(100, 100, "test");

    for (int i = 0; i < 100000; i++){
        mt->writeDist(i, i + 1, i * 100.1);
    }

    delete mt;


    return 0;
}
