/* this file implements class B_Tree */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <fstream>
#include "b-tree.h"
#include "b-node.h"
#include "b-entry.h"
#include "b-def.h"
#include "../heap/heap.h"
#include "../blockfile/blk_file.h"
#include "../gadget/gadget.h"

/*****************************************************************
Use this constructor to create an empty b-tree
para:


Coded by Yufei Tao 30/12/01
*****************************************************************/

B_Tree::B_Tree()
{
    num = 0;
	quiet = 8;

	root_ptr = NULL;

	for (int i = 0; i < 100; i ++)
		debug_info[i] = -1;

	emergency = false;
}

/*****************************************************************
loads the tree from a tree file

para:
- fname:
- c:

Coded by Yufei Tao 31 July 08
*****************************************************************/

void B_Tree::init_restore(char *_fname, Cache *_c)
{
	char *fname = new char[strlen(_fname) + 10]; //allow 9 characters for extension name
	strcpy(fname, _fname);
	add_fname_ext(fname);

	//--=== testing if the file exists ===--
	FILE *fp = fopen(fname, "r");
	if (!fp)
	{
		printf("tree file %s does not exist\n", fname);
		delete []fname;
		error("", true);
	}

	fclose(fp);
	//--=== done testing the file existence ===--

    file = new BlockFile(fname, 0);
    cache = _c;

	root_ptr = NULL;
	char *header = new char [file -> get_blocklength()];
    file->read_header(header);
    read_header(header);
	delete [] header;
	delete [] fname;
}

/*****************************************************************
Coded by Yufei Tao, 3 aug 08
*****************************************************************/

void B_Tree::close()
{
	if (root_ptr)
	{
		delete root_ptr;
		root_ptr = NULL;
	}

	char *header = new char[file -> get_blocklength()];
    write_header(header);
    file -> set_header(header);
    delete [] header;

	if (cache){
        cache -> flush();
        delete cache;
        cache = NULL;
	}


    if (file)
	{
		delete file;
		file = NULL;
	}
}

//-----------------------------------------------

B_Tree::~B_Tree()
{
	if (root_ptr)
	{
		error("b_tree::~b_tree - root_ptr not null. you forgot to call close().\n", true);
	}

	if (file)
	{
		error("b_tree::~b_tree - file not null. you forgot to call close().\n", true);
	}
}

/*****************************************************************
This function deletes the root

Coded by Yufei Tao 29/12/01
*****************************************************************/

void B_Tree::delroot()
{
	delete root_ptr;
    root_ptr=NULL;
}

//-----------------------------------------------
/*
bool B_Tree::find_key(float _k)
{
	load_root();
	bool ret = root_ptr -> find_key(_k);
	delete root_ptr; root_ptr = NULL;
	return ret;
}
*/
//-----------------------------------------------

void B_Tree::insert(B_Entry *_new_e)
{
	if (_new_e->son != _new_e->leafson)
		error("B_Tree::insert - son and leafson unequal\n", true);

	load_root();

//--=== debugging ===--
/*
if (debug_info[0] == 0)
{
	delete root_ptr->entries[0];
	delete root_ptr->entries[1];
}
*/
//--=================--

	B_Node *new_nd = NULL;
	BINSRT c_ret = root_ptr -> insert(_new_e, &new_nd);
	if (c_ret == B_OVRFLW)
	{
		B_Node *new_root = new_one_node();
		new_root->init(root_ptr->level+1, this);
			//the first paramemter just needs to be any value larger than 0
		new_root -> level = root_ptr -> level + 1;
		new_root -> add_new_child(root_ptr);
		new_root -> add_new_child(new_nd);
		root = new_root -> block;
		root_ptr = new_root;
	}

//--=== debugging ===--
/*
if (debug_info[0] == 0)
{
	delete root_ptr->entries[0];
	delete root_ptr->entries[1];
}
*/
//--=================--

	delroot();
}

//-----------------------------------------------

void B_Tree::load_root()
{
	if (root_ptr == NULL)
	{
		root_ptr = new_one_node();
		root_ptr->init(this, root);
	}
}

//-----------------------------------------------
/*
void B_Tree::qry_agg(int _k1, int _k2)
{
	load_root();
	root_ptr -> qry_agg(_k1, _k2, B_MAXINT);
	delete root_ptr;
	root_ptr = NULL;
}
*/
//-----------------------------------------------

int B_Tree::read_header(char *_buf)
{
	int i = 0;
	memcpy(&root, _buf, sizeof(root));
	i += sizeof(root);

	memcpy(&keysize, &(_buf[i]), sizeof(keysize));
	i += sizeof(keysize);

	return i;
}

//-----------------------------------------------

int B_Tree::write_header(char *_buf)
{
	int i = 0;
	memcpy(_buf, &root, sizeof(root));
	i += sizeof(root);

	memcpy(&(_buf[i]), &keysize, sizeof(keysize));
	i += sizeof(keysize);

	return i;
}

/*****************************************************************
define the rank of an record o as the sum of the weights of all the records with keys smaller than o.key
this function returns the record whose rank is "just larger" than given a rank value,
para:
rank:
coded by yufei tao june 2003
*****************************************************************/
/*
B_Entry *B_Tree::rank_find(float _rank)
{
	load_root();
	B_Entry *cret=root_ptr->rank_find(_rank);
	delroot();
	return cret;
}
*/

/*****************************************************************
this function deletes an entry from the b-tree

para:
- del_e: the entry to be deleted

the return value is false if the record is not found
coded by yufei tao june 2003
*****************************************************************/

bool B_Tree::delete_entry(B_Entry * _del_e)
{
	bool ret = true;
	load_root();
	BDEL cret = root_ptr->delete_entry(_del_e);
	if (cret == B_UNDRFLW)
	{
		root=root_ptr->entries[0]->son;
	}
	delroot();
	if (cret == B_NOTFOUND)
		ret = false;
	return ret;
}

/*****************************************************************
bulkloads a tree. the records of the dataset must have been sorted
in ascending order of their keys

para:
- fname: dataset

Coded by Yufei Tao, 4 aug 08
*****************************************************************/




/*****************************************************************
bulkload a tree. difference from bulkload() is that here the
sorted dataset is in memory.

-para-
ds:		dataset
n:		cardinality

-return-
0		success
1		failure

-by-
yf tao

-last touch-
19 aug 09
*****************************************************************/


/*****************************************************************
retrieves the next entry from a data file

Coded by Yufei Tao, 31 July 08
*****************************************************************/

void B_Tree::fread_next_entry(FILE *_fp, B_Entry *_d)
{
	fscanf(_fp, "%zu ", &(_d->son));
	for (int i = 0; i < keysize; i ++)
		fscanf(_fp, "%d", &(_d->key[i]));

	fscanf(_fp, "\n");

	_d->leafson = _d->son;
}

/*****************************************************************
This function adds an extension ".b" to the file name

Parameters:
- _fname: the file name before adding the extension

Coded by Yufei Tao, 31 July 08
*****************************************************************/

void B_Tree::add_fname_ext(char * _fname)
{
	strcat(_fname, ".b");
}

/*****************************************************************
creates a new tree

Parameters:
- fname: the file name before adding the extension
- blength: page size
- c:
- keysize:

Coded by Yufei Tao, 31 July 08
*****************************************************************/

void B_Tree::init(char *_fname, int _b_length, Cache *_c, int _keysize)
{
	char *fname = new char[strlen(_fname) + 10]; //allow 9 characters for extension name
	strcpy(fname, _fname);
	add_fname_ext(fname);

	FILE *fp = fopen(fname, "r");
	if (fp)
	{
		fclose(fp);
		printf("The file \"%s\" exists. Replace? (y/n)", fname);
		char c = getchar(); getchar();
		if (c != 'y' && c != 'Y')
			error("", true);
		remove(fname);
	}

    file = new BlockFile(fname, _b_length);
    cache = _c;
	keysize = _keysize;

	//init the first node-----------------------------------------
	//we will deliberately waste a node so that all nodes' addresses are positive
    root_ptr = new_one_node();
	root_ptr->init(0, this);
    root = root_ptr->block;
	delroot();

	root_ptr = new_one_node();
	root_ptr->init(0, this);
	root = root_ptr->block;
	delroot();
	//------------------------------------------------------------

	delete []fname;
}

/*****************************************************************
DESCRIPTION
Traverse the tree and collect information.

PARAMETERS:
- _info (out): carries the collected info outside the function.
               Currently, _info[0] is the number of objects in the tree, _info[1] the number of nodes, and
			   _info[2] the node capacity.

RETURN
1 -- Everything all right.
0 -- Error found.

KNOWN ISSUES:
None.

PROGRAMMER
Yufei Tao

DATE OF LAST UPDATE
18 Nov. 08
*****************************************************************/

int B_Tree::traverse(float *_info)
{
	last_leaf = -1;
	last_right_sibling = -1;

	load_root();
	_info[2] = (float) root_ptr->capacity;
	int ret = root_ptr->traverse(_info);
	delroot();

	return ret;
}

/*****************************************************************
by yufei tao, 31 july 08
*****************************************************************/

B_Entry *B_Tree::new_one_entry()
{
	B_Entry *e = new B_Entry();

	return e;
}

/*****************************************************************
by yufei tao, 31 july 08
*****************************************************************/

B_Node *B_Tree::new_one_node()
{
	B_Node *nd = new B_Node();

	return nd;
}


/*****************************************************************
by guanhua mai,
*****************************************************************/

void B_Tree::insertKV(int* key, size_t valueAddr){
    B_Entry* et = this->new_one_entry();
    et->init(this, 0);
    et->son = num++;
    et->addr = valueAddr;
    for(int i = 0; i < this->keysize; i++){
        et->key[i] = key[i];
    }
    et->leafson = et->son;
    this->insert(et);                       //no need to close it here, will be deleted at the bottom of the tree
}


size_t B_Tree::findValueByKey(int* key, B_Node* node){//find key from root to the bottom of the tree

    size_t ret = (size_t) -1;//convert -1 to the maximum value, use as a flag

    int follow = node->max_lesseq_key_pos(key);

	if (node->is_eq_key_pos(key, follow))
	{
		ret = node->entries[follow]->addr;
		return ret;
	}

	if (follow != -1 && node->level != 0)
	{
		B_Node *succ = node->entries[follow]->get_son();
		ret = findValueByKey(key, succ);
		//delete succ;
		node->entries[follow]->del_son();
	}

	return ret;
}
