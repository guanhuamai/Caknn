#ifndef __CACHE
#define __CACHE

#include "f_def.h"
#include <unordered_map>

//----------------------------------------------------------------
class Cache;
class BlockFile;

class Cacheable   //inherit this class if you wish to design an external
                  //memory structure that can be cached
{
public:
	BlockFile *file;
	Cache *cache;
};

//----------------------------------------------------------------
class Cache
{
public:
   enum uses {free,used,fixed};	// for fuf_cont
   int ptr;		        //current position in cache
   int cachesize;		//the number of blocks kept in memory
   int blocklength;
   int page_faults;
   size_t *cache_cont;	    // array of the indices of blocks that are in cache
   Cacheable **cache_tree;  // array of ptrs to the correspondent Cacheables where the blocks belong to
   uses *fuf_cont; 		//indicator array that shows whether one cache block is free, used or fixed
   int  *LRU_indicator; //indicator that shows how old (unused) is a page in the cache
   bool *dirty_indicator;  //indicator that shows if a cache page has been written

   char **cache;   		// Cache


   struct BlkPos{//position of block, inside of cache, use it as int, which is initialized to -1
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
   std::unordered_map<size_t, BlkPos> umap;//hash block id to cache position


	//=====================================================

   int next();

   int in_cache(size_t index, Cacheable *rt);

   Cache(int csize, int blength);

   ~Cache();

   bool read_block(Block b, size_t i, Cacheable *rt);

   bool write_block(Block b, size_t i, Cacheable *rt);


   void set_cachesize(int s);

   void flush();
};


#endif // __CACHE
