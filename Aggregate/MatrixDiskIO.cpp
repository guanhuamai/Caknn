#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include "MatrixDiskIO.h"




int DskContrlr::getBlockLength() {
	return blocklength;
}

void DskContrlr::InitCache(int csize, int blength) {
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

void DskContrlr::RefreshStat() {
	SEQ_PAGE_ACCESS=0;
	RAN_PAGE_ACCESS=0;
	INDEX_PAGE_ACCESS=0;
	DATA_PAGE_ACCESS=0;
	CACHE_ACCESSED=0;
}

void DskContrlr::RefreshCache() {	// call before query execution
	LastBlockId=-2;
	nextTime=0;
	for (int i=0;i<cachesize;i++) {
		lastTime[i]=-1;
		cache_block[i]=-1;
	}
}

void DskContrlr::DestroyCache() {
	// no need to flush the cache (since the query alg. are read-only )
	delete[] cache_block;
	delete[] lastTime;
	for (int i=0;i<cachesize;i++) delete[] cache[i];
	delete[] cache;
}


bool DskContrlr::getCacheBlock(char* buffer,int BlockId) {
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

// user's responsibility
// the place for counting number of page accesses
void DskContrlr::storeCacheBlock(char* buffer, int BlockId) {
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

void DskContrlr::printPageAccess() {
	printf("Cache requests:\t%d\n",CACHE_ACCESSED);
	printf("Index page accesses:\t%d\n",INDEX_PAGE_ACCESS);
	printf("Data page accesses:\t%d\n",DATA_PAGE_ACCESS);
	//printf("Seq.:\t%d\n",SEQ_PAGE_ACCESS);
	//printf("Ran.:\t%d\n",RAN_PAGE_ACCESS);
	printf("Total page accesses:\t%d\n",INDEX_PAGE_ACCESS+DATA_PAGE_ACCESS);
}


