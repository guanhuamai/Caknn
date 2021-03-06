#ifndef __B_Tree
#define __B_Tree

#include "b-def.h"
#include "../blockfile/cache.h"
//-----------------------------------------------

class BlockFile;
class B_Entry;
class B_Node;
class Cache;
class Heap;

class B_Tree : public Cacheable
{

protected:

    size_t num;

public:
	//--==write to disk==--
	size_t root;                                                       //live in memory
	int keysize;													//size of the key array of each entry

	//--== others ==--
	B_Node *root_ptr;                                               //not live in memory

	//--=== for debugging ===--
	size_t last_leaf;
	size_t last_right_sibling;
	int debug_info[100];
	int quiet;														//control how often info is displayed
																	//higher value means fewer msgs
	bool emergency;

	//--== functions ==--
	B_Tree();
	virtual ~B_Tree();
	virtual void add_fname_ext(char * _fname);
	//virtual void bulkload(char *_fname);
	//virtual int bulkload2(void *_ds, int _n);
	//virtual void build_from_file(char *_dsname);
	virtual void close();
	virtual bool delete_entry(B_Entry * _del_e);
	virtual void delroot();
	//virtual bool find_key(float _k);
	virtual void fread_next_entry(FILE *_fp, B_Entry *_d);
	virtual void init(char *_fname, int _b_length, Cache *_c, int _keysize);
	virtual void init_restore(char *_fname, Cache *_c);
	virtual void insert(B_Entry *_new_e);
	virtual void load_root();
	virtual B_Entry *new_one_entry();
	virtual B_Node *new_one_node();
	//void qry_agg(int _k1, int _k2);
	virtual size_t read_header(char *_buf);
	virtual int read_next_entry(void **_ptr, B_Entry *_d){return 1;};
	virtual int traverse(float *_info);
	virtual size_t write_header(char *_buf);



    virtual void insertKV(int* key, size_t valueAddr);
    virtual size_t findValueByKey(int* key, B_Node* rootNode);
	//B_Entry *rank_find(float _rank);
};

//-----------------------------------------------

#endif
