#include <iostream>
#include "Aggregate/DistMatrix.h"

using namespace std;

int main()
{

    DistMatrix* mt = new DistMatrix(500, 200, "test");

    for (int i = 0; i < 1000; i++){

        mt->writeDist(i + 5, i + 6, i + 100.1);
    }
    printf("success load in btree\n");


    for (int i = 0; i < 1000; i++){
        double dist = mt->readDist(i + 5, i + 6);
        if(dist - (i + 100.1) > 0.00001 || dist - (i + 100.1) < -0.00001){
            printf("%lf\n", mt->readDist(i + 5, i + 6));
        }

    }
    printf("success\n");


    delete mt;


    return 0;
}
