#include <iostream>
#include "Aggregate/DistMatrix.h"
#include "Aggregate/AggFunc.h"


using namespace std;

int main()
{

    DistMatrix* mt = new DistMatrix(1500000, 6000, "test");

    for (int i = 0; i < 1000000000; i++){
        mt->writeDist(i + 5, i + 6, i + 100.1);
        int query = ((double)rand() / RAND_MAX) * i;
        double dist = mt->readDist(query + 5, query + 6);
        if(dist - (query + 100.1) > 0.00001 || dist - (query + 100.1) < -0.00001){
            printf("%d %lf\n", query, mt->readDist(query + 5, query + 6));
        }
        if(i % 10000 == 0){
            printf("%dth iteration! query = %d dist = %lf\n", i, query, dist);
        }
    }
    printf("success check btree\n");


    printf("success\n");


    delete mt;


    return 0;
}
