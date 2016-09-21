
#ifndef __Disk_Matrix
#define __Disk_Matrix



class DskContrlr{

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
    int getBlockLength();
    void InitCache(int csize);
    void RefreshCache();
    void DestroyCache();
    bool getCacheBlock(char* buffer,int BlockId);
    void storeCacheBlock(char* buffer,int BlockId);	// user's responsibility
    void printPageAccess();
    void RefreshStat();
};


#endif
