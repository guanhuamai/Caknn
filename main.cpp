#include <iostream>
#include "Aggregate/DistMatrix.h"

using namespace std;

int main()
{

    DistMatrix* mt = new DistMatrix(500, 4096, "test");

    for (int i = 0; i < 100; i++){

        mt->writeDist(i + 5, i + 6, i + 100.1);
//        if(i % 1000 == 0){
//            printf("%dth read dists\n", i);
//        }
    }

    printf("success load in btree");


    for (int i = 0; i < 100; i++){
        double dist = mt->readDist(i + 5, i + 6);
//        if(dist - (i + 100.1) > 0.00001 || dist - (i + 100.1) < -0.00001){
            printf("%lf\n", dist);
//        }

    }

    cout << "success" << endl;


    delete mt;


    return 0;
}
