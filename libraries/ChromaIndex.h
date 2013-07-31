/***
	handle the chroma hash index
***/

#include "BinarySearchTree.h"

#define TREE_SIZE 5000


class ChromaIndex:public Index
 {  
 	private:
 		// index stored as binary search tree
 	  	BinarySearchTree* index;
 	  	// index stored as compact structure
 	  	entry* compactIndex;
 	  	int size;
 	 
 	
 	public:
 	  	// constructors and destructors
 	  	ChromaIndex ();
 	  	~ChromaIndex (void);
 	  
 	  	// creation of the index
      	bool create ();
         
      	// deletion of the index stored as tree structure
      	bool clear ();
     
      	// adding an element
      	bool addElement (char* filename, char* title, short tonality);
      
      	// adding an element - with transposition
      	bool addTransposedElement (char* filename, char* title);
      
      	// retrieval of the query
      	bool retrieve ();
     
      	// get the index
      	entry* getIndex ();
      
      	// get the size of index
      	int getSize ();
      
      	// compact the index in the final structure
      	bool indexing ();
      
      
    private:       
    	// get the ID fo the processed recording
    	string getID (char* title);
        
    	// in-order index tree walk - save the index in the compact structure
      	void inOrderTreeWalk (int x, int& ind);
        
  };

