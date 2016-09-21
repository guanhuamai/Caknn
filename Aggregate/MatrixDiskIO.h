#include "../btree/b-tree.h"

#ifndef __Disk_Matrix
#define __Disk_Matrix




class DataCache{

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


class DskContrlr{

protected:
    FILE* fp;//file pointer
    char* fnamePrefix;//file name prefix
    DataCache *cc;
    long faddr;

    int blocklength;

    B_Tree* bt;


    int* hashBTkey(int snid, int enid){
        int * key = new int[2];
        if (snid > enid){
            int tmp = snid;
            snid = enid;
            enid = tmp;
        }
        key[0] = snid; key[1] = enid;
        return key;
    }

    size_t find_dist_addr(int snid, int enid){
        int* key = hashBTkey(snid, enid);
        size_t position = bt->findValueByKey(key, bt->root_ptr);

        delete []key;
        return position;
    }

public:

    void InitDskContrlr(int csize, int blength, char* filePrefix);




    size_t write_dist(int snid, int enid, double dist){
        size_t position = ftell(fp);
        fwrite(&snid, sizeof(int), 1, fp);
        fwrite(&enid, sizeof(int), 1, fp);
        fwrite(&dist, sizeof(double), 1, fp);

        int* key = hashBTkey(snid, enid);

        bt->insertKV(key, position);//no need to close it here, will be deleted at the bottom of the tree

        delete []key;
        return position;
    }

    double read_dist(int snid, int enid){
        double dist = -1.0;
        size_t addr = find_dist_addr(snid, enid);

        int blk_len = cc->getBlockLength();
        int blk_id = (int)addr / blk_len;
        char* buf = new char[blk_len];
        if (!cc->getCacheBlock(buf, blk_id)){
            fseek(fp, blk_id * blk_len, SEEK_SET);
            fread(buf, blk_len, 1, fp);
            cc->storeCacheBlock(buf, blk_id);
        }
        memcpy(&dist, &buf[addr % blk_len], sizeof(double));
        delete []buf;
        return dist;

    }






};


#endif
