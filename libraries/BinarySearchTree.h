/***
	binary search tree structure for the index
***/

#include "Libraries.h"
 

class BinarySearchTree
 { 
 	private:
     	node* tree;
     	int size, maxSize;
     
   	public:
    	// constructors and destructor
     	BinarySearchTree ();
     	BinarySearchTree (int maxSize);
     	~BinarySearchTree (void);
     
     	// search an element in the tree
     	int search (node e);
     
     	// insert a new element in the tree
     	bool insert (unsigned long word, string id, short segment);
     
     	// clear the tree content
     	bool clear ();
     
     	// get the element at 'index'
     	node getElement (int index);
     
     	// get the tree
     	node* getBinarySearchTree ();
     
     	// get the size
     	int getSize ();
     
     	// get the max size
     	int getMaxSize ();
     
     	// print the array on the screen
     	void print ();
     
     	// save the array on the disk
     	bool save (char* filename);
     
   	private:
     	// increase the array size
     	bool doubleSize ();
     
  };
