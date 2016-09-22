#include "../btree/b-tree.h"

#ifndef __DistMatrix
#define __DistMatrix




class DataCache
{

protected:
    char** cache;

    int blocklength;
    int cachesize;	 	// max. number of cache blocks

    // old: int *cache_cont; store transBlockID
    int *cache_block;	// block Id!

    int *lastTime;		// >=0 means used
    int nextTime;
    int LastBlockId;

    int SEQ_PAGE_ACCESS;
    int RAN_PAGE_ACCESS;
    int INDEX_PAGE_ACCESS;
    int DATA_PAGE_ACCESS;
    int CACHE_ACCESSED;

//this operation is to regulate cache in memory

public:
    int getBlockLength();
    void InitCache(int csize, int blength);
    void RefreshCache();
    void DestroyCache();
    bool getCacheBlock(char* buffer,int BlockId);
    void storeCacheBlock(char* buffer,int BlockId);	// user's responsibility
    void printPageAccess();//show the statistics
    void RefreshStat();// make statistics zero
};


class DistMatrix
{

protected:
    FILE* fp;//file pointer
    char* fnamePrefix;//file name prefix


    int blocklength;
    DataCache* cc;
    B_Tree* bt;


    virtual int* hashBTkey(int snid, int enid);
    virtual size_t findAddrByBT(int snid, int enid);

public:

    virtual void initDsk(int csize, int blength, char* filePrefix);
    virtual size_t writeDist(int snid, int enid, double dist);
    virtual double readDist(int snid, int enid);

};


#endif
