#include "../btree/b-tree.h"
#include <unordered_map>
#ifndef __DistMatrix
#define __DistMatrix


class DataCache
{

protected:
    char** cache;

    int blocklength;
    int cachesize;	 	// max. number of cache blocks

    // old: int *cache_cont; store transBlockID
    size_t *cache_block;	// block Id!

    int *lastTime;		// >=0 means used
    int nextTime;
    int LastBlockId;

    int SEQ_PAGE_ACCESS;
    int RAN_PAGE_ACCESS;
    int INDEX_PAGE_ACCESS;
    int DATA_PAGE_ACCESS;
    int CACHE_ACCESSED;



    struct BlkPos{                                  //position of block, inside of cache, use it as int, which is initialized to -1
        public:
        int c_index;
        BlkPos(){
            this->c_index = -1;
        }
        BlkPos(int c_index){
            this->c_index = c_index;
        }
        bool operator == (const BlkPos &c_index){
            if(c_index.c_index == this->c_index){
                return true;
            }
            return false;
        }
        bool operator != (const BlkPos &c_index){
            if(c_index.c_index != this->c_index){
                return true;
            }
            return false;
        }
        BlkPos& operator = (const BlkPos &c){
            this->c_index = c.c_index;
            return *this;
        }
    };
    std::unordered_map<size_t, BlkPos> umap;        //hash block id to cache position


//this operation is to regulate cache in memory

public:

    DataCache(int csize, int blocklen);
    virtual ~DataCache();
    virtual void InitCache(int csize, int blength);
    virtual int getBlockLength();
    virtual void RefreshCache();
    virtual void DestroyCache();
    virtual bool getCacheBlock(char* buffer,size_t BlockId);
    virtual void storeCacheBlock(char* buffer,size_t BlockId);	// user's responsibility
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
    virtual double readDist (int snid, int enid);

};


#endif
