/*blk_file.h:
  this file specifies the class BlockFile, a block file simulator
*/
#ifndef __BLK_FILE
#define __BLK_FILE

//----------------------------------------------------------
#include <stdio.h>
#include "f_def.h"
//----------------------------------------------------------

class BlockFile
{
public:
   FILE* fp;			// os file pointer
   char* filename;
   size_t blocklength;	    // length of a block
   size_t act_block; 	    // block # of fp's position (fp can stay at block boundaries)
   size_t number;		    // total # of blocks
   bool new_flag;		// specifies if this is a new file

     //=====================================================
   BlockFile(char* name, size_t b_length);
   			        // Filename und Blocklaenge (fuer neue Files)
   ~BlockFile();

   void put_bytes(char* bytes,int num)
		{ fwrite(bytes,num,1,fp);
	      /*char *add;
		  add = bytes;
	      for (int i = 0; i < num; i ++)
		  { fputc(*add, fp); add ++; }*/}

   void get_bytes(char* bytes,int num)
		{ fread(bytes,num,1,fp);
       	  /*char *add;
		  add = bytes;
	      for (int i = 0; i < num; i ++)
		  { *add = fgetc(fp); add ++; }*/}

   void fwrite_number(size_t num);

   size_t fread_number();

   void seek_block(size_t bnum)
		{ fseek(fp,(bnum-act_block)*blocklength,SEEK_CUR); }

   void read_header(char * header);

   void set_header(char* header);

   bool read_block(Block b,size_t i);

   bool write_block(Block b,size_t i);

   size_t append_block(Block b);

   bool delete_last_blocks(size_t num);

   bool file_new()
		{ return new_flag; }

   size_t get_blocklength()
		{ return blocklength; }

   size_t get_num_of_blocks()
		{ return number; }
};

//-------------------------------------------------------------------------
class CachedBlockFile : public BlockFile
{
public:
   enum uses {free,used,fixed};
   int ptr;		        // current position in cache
   int cachesize;		// //the number of blocks kept in memory
   int page_faults;

   int *cache_cont;	    // array of the indices of blocks that are in cache
   uses *fuf_cont; 		// indicator array that shows whether one cache block is free, used or fixed
   int  *LRU_indicator; // indicator that shows how old (unused) is a page in the cache
   bool  *dirty_indicator;  // indicator that shows if a page has been modified

   char **cache;   		// Cache

	//=====================================================

   CachedBlockFile(char* name, int blength, int csize);

   ~CachedBlockFile();

   int next();

   int in_cache(int index);	// liefert Pos. im Cache oder -1

   bool read_block(Block b,size_t i);

   bool write_block(Block b,size_t i);

   bool fix_block(size_t i);

   bool unfix_block(size_t i);

   void unfix_all();

   void set_cachesize(int s);

   void flush();
};


#endif // __BLK_FILE
