#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include "DistMatrix.h"




int DataCache::getBlockLength() {
	return blocklength;
}

void DataCache::InitCache(int csize, int blength) {
	LastBlockId=-2;
	nextTime=0;
	cachesize=csize;
	blocklength = blength;
	cache_block = new int[cachesize];

	cache = new char*[cachesize];
	lastTime=new int[cachesize];
	for (int i=0;i<cachesize;i++) {
		lastTime[i]=-1;
		cache_block[i]=-1;
		cache[i] = new char[blocklength];
	}
}

void DataCache::RefreshStat() {
	SEQ_PAGE_ACCESS=0;
	RAN_PAGE_ACCESS=0;
	INDEX_PAGE_ACCESS=0;
	DATA_PAGE_ACCESS=0;
	CACHE_ACCESSED=0;
}

void DataCache::RefreshCache() {	// call before query execution
	LastBlockId=-2;
	nextTime=0;
	for (int i=0;i<cachesize;i++) {
		lastTime[i]=-1;
		cache_block[i]=-1;
	}
}

void DataCache::DestroyCache() {
	// no need to flush the cache (since the query alg. are read-only )
	delete[] cache_block;
	delete[] lastTime;
	for (int i=0;i<cachesize;i++) delete[] cache[i];
	delete[] cache;
}

bool DataCache::getCacheBlock(char* buffer,int BlockId) {
	CACHE_ACCESSED++;
	for (int i=0;i<cachesize;i++)
		if ((cache_block[i]==BlockId)&&
			(lastTime[i]>=0)) {
				memcpy(buffer,cache[i],blocklength);
				lastTime[i]=nextTime++;
				return true;
			}
	return false;
}
// the place for counting number of page accesses
void DataCache::storeCacheBlock(char* buffer, int BlockId) {
	int index=-1;
	for (int i=0;i<cachesize;i++)	// search for empty block
		if (lastTime[i]<0) {index=i;	break;}

	if (index<0) {
		index=0;	// full, evict LRU block
		for (int i=0;i<cachesize;i++)
			if (lastTime[i]<lastTime[index]) index=i;
	}

	memcpy(cache[index],buffer,blocklength);
	cache_block[index]=BlockId;
	lastTime[index]=nextTime++;


	if (abs(LastBlockId-BlockId)<=1)
		SEQ_PAGE_ACCESS++;
	else
		RAN_PAGE_ACCESS++;
	LastBlockId=BlockId;
}

void DataCache::printPageAccess() {
	printf("Cache requests:\t%d\n",CACHE_ACCESSED);
	printf("Index page accesses:\t%d\n",INDEX_PAGE_ACCESS);
	printf("Data page accesses:\t%d\n",DATA_PAGE_ACCESS);
	//printf("Seq.:\t%d\n",SEQ_PAGE_ACCESS);
	//printf("Ran.:\t%d\n",RAN_PAGE_ACCESS);
	printf("Total page accesses:\t%d\n",INDEX_PAGE_ACCESS+DATA_PAGE_ACCESS);
}



void MatrixDsk::initMatrixDsk(int csize, int blength, char* filePrefix);

int* MatrixDsk::hashBTkey(int snid, int enid){
        int * key = new int[2];
        if (snid > enid){
            int tmp = snid;
            snid = enid;
            enid = tmp;
        }
        key[0] = snid; key[1] = enid;
        return key;
    }

size_t MatrixDsk::findAddrByBT(int snid, int enid){
    int* key = hashBTkey(snid, enid);
    size_t position = bt->findValueByKey(key, bt->root_ptr);

    delete []key;
    return position;
}

size_t MatrixDsk::writeDist(int snid, int enid, double dist){
    size_t position = ftell(fp);
    fwrite(&snid, sizeof(int), 1, fp);
    fwrite(&enid, sizeof(int), 1, fp);
    fwrite(&dist, sizeof(double), 1, fp);

    int* key = hashBTkey(snid, enid);

    bt->insertKV(key, position);//no need to close it here, will be deleted at the bottom of the tree

    delete []key;
    return position;
}

double MatrixDsk::readDist(int snid, int enid){
    double dist = -1.0;
    size_t addr = find_dist_addr(snid, enid);

    int blk_len = cc->getBlockLength();
    int blk_id = (int)addr / blk_len;
    char* buf = new char[blk_len];
    if (!cc->getCacheBlock(buf, blk_id)){
        fseek(fp, blk_id * blk_len, SEEK_SET);
        fread(buf, blk_len, 1, fp);
        cc->storeCacheBlock(buf, blk_id);
        fseek(fp, 0, SEEK_END);
    }
    memcpy(&dist, &buf[addr % blk_len], sizeof(double));
    delete []buf;
    return dist;

}



