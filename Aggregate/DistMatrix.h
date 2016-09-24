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
    DataCache(int csize, int blocklen);
    virtual ~DataCache();
    virtual void InitCache(int csize, int blength);
    virtual int getBlockLength();
    virtual void RefreshCache();
    virtual void DestroyCache();
    virtual bool getCacheBlock(char* buffer,int BlockId);
    virtual void storeCacheBlock(char* buffer,int BlockId);	// user's responsibility
    virtual void printPageAccess();//show the statistics
    virtual void RefreshStat();// make statistics zero
};


class DistMatrix
{

protected:
    FILE* fp;//file pointer
    char* filePrefix;//file name prefix


    int blocklength;
    DataCache* dc;
    B_Tree* bt;

    virtual void initDsk(int csize, int blength);//init local cache, btree, btree cache and matrix
    virtual int* hashBTkey(int snid, int enid);
    virtual size_t findAddrByBT(int snid, int enid);//order is irrelative

public:

    DistMatrix(int csize, int blength, char* filePrefix);
    virtual ~DistMatrix();
    virtual size_t writeDist(int snid, int enid, double dist);
    virtual double readDist(int snid, int enid);

};


#endif
