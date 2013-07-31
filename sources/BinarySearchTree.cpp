/***
	binary search tree structure for the chroma index
***/

#include "BinarySearchTree.h"

// constructors
BinarySearchTree::BinarySearchTree (int maxSize)
 { this->maxSize = maxSize;
   this->tree = new node [this->maxSize];
   this->size = 0;
 }

 
BinarySearchTree::BinarySearchTree ()
 { this->maxSize = 1;
   this->tree = new node [this->maxSize];
   this->size = 0;
 }

 
// destructor
BinarySearchTree::~BinarySearchTree (void)
 { if (this->tree)
     delete [] this->tree;
 }

 
// search an element in the tree
int BinarySearchTree::search (node e)
 { return 0; }

     
// insert a new element in the tree
bool BinarySearchTree::insert (unsigned long word, string id, short segment)
 { // check tree dimension
   if (size == maxSize) this->doubleSize();
   // empty tree
   if (this->size == 0)
    { tree[size].words = word;
      tree[size].parent = tree[size].sx = tree[size].dx = -1;
      position p;
      p.id = id;
      p.intervals.push_back (segment);
      tree[size].positions.push_back (p);
      size++;
    }
   else
    { int index = 0;
      int current = -1;
      while ((index!=-1) && (word != this->tree[index].words))
       { current = index;
       	 if (word > tree[current].words) 
       	   index = tree[current].dx;
       	 else
       	   index = tree[current].sx;                            
       }
      // element already present in the index 
      if (index >= 0)
       { // search id
       	 short found = 1;
       	 int i = 0;
       	 while ((found == 1) && (i<tree[index].positions.size()))
       	  { if (tree[index].positions.at(i).id == id) found = 0;
       	  	else i++;
       	  }
       	 if (found == 1)
       	   { // not found - add new position
       	   	 position p;
       	   	 p.id = id;
       	   	 p.intervals.push_back (segment);
       	   	 tree[index].positions.push_back (p);
       	   }
       	 else
       	  { // found - add segment if not already present
       	  	short check = 1;
       	  	short j = 0;
       	  	while ((check == 1) && (j<tree[index].positions.at(i).intervals.size()))
       	  	 { if (tree[index].positions.at(i).intervals.at(j) == segment) check = 0;
       	  	   else j++;
       	  	 }
       	  	// segment not found - add it
       	  	if (check == 1)
       	  	 tree[index].positions.at(i).intervals.push_back (segment);
       	  }
       }
      else
       { // insert new element
       	 tree[size].words = word;
         tree[size].parent = current;
         tree[size].sx = tree[size].dx = -1;
         position p;
         p.id = id;
         p.intervals.push_back (segment);
         tree[size].positions.push_back (p);
         // link the new element to the tree
         if (word > tree[current].words)
          tree[current].dx = size;
         else
          tree[current].sx = size;      
         size++;
       }
    }
   return true; 
 }


// clear the tree content
bool BinarySearchTree::clear ()
 { // empty the tree content
   if (this->tree)
    delete [] tree;
   this->tree = new node [this->maxSize];
   return true;
 }

     
// get the element at position 'index'
node BinarySearchTree::getElement (int index)
 { return this->tree[index]; }

     
// get the tree
node* BinarySearchTree::getBinarySearchTree ()
 { return this->tree; }

     
// get the size
int BinarySearchTree::getSize ()
 { return this->size; }

      
// get the max size
int BinarySearchTree::getMaxSize ()
 { return this->maxSize; }

  
// print the array on the screen sorted as stored (array)
void BinarySearchTree::print ()
 { for (int i=0; i<this->size; i++)
 	{ cout<<" "<<tree[i].words<<" "<<tree[i].parent<<" "<<tree[i].sx<<" "<<tree[i].dx<<" ";
 	  for (int j=0; j<tree[i].positions.size()-1; j++)
 	   { cout<<tree[i].positions.at(j).id<<"{";
 	   	 for (int k=0; k<tree[i].positions.at(j).intervals.size()-1; k++)
 	   	   cout<<tree[i].positions.at(j).intervals.at(k)<<"-";
 	   	 // last positions
 	   	 cout<<tree[i].positions.at(j).intervals.at(tree[i].positions.at(j).intervals.size()-1)<<"},";
 	   }
 	  // last element
 	  cout<<tree[i].positions.at(tree[i].positions.size()-1).id<<"{";
 	  for (int k=0; k<tree[i].positions.at(tree[i].positions.size()-1).intervals.size()-1; k++)
 	    cout<<tree[i].positions.at(tree[i].positions.size()-1).intervals.at(k)<<"-";
 	  cout<<tree[i].positions.at(tree[i].positions.size()-1).intervals.at(tree[i].positions.at(tree[i].positions.size()-1).intervals.size()-1)<<"}";	
 	  cout<<endl; 	    
 	}
 }


// save the array on the disk 
bool BinarySearchTree::save (char* filename)
 { try
 	{ ofstream file;
      file.open (filename);
      for (int i=0; i<this->size; i++)
 	   { file<<tree[i].words<<"|"<<tree[i].parent<<"|"<<tree[i].sx<<"|"<<tree[i].dx<<"|";
 	     for (int j=0; j<tree[i].positions.size()-1; j++)
 	      { file<<tree[i].positions.at(j).id<<"{";
 	   	    for (int k=0; k<tree[i].positions.at(j).intervals.size()-1; k++)
 	   	      file<<tree[i].positions.at(j).intervals.at(k)<<"-";
 	   	    // last positions
 	   	    file<<tree[i].positions.at(j).intervals.at(tree[i].positions.at(j).intervals.size()-1)<<"},";
 	      }
 	     // last element
 	     file<<tree[i].positions.at(tree[i].positions.size()-1).id<<"{";
 	     for (int k=0; k<tree[i].positions.at(tree[i].positions.size()-1).intervals.size()-1; k++)
 	       file<<tree[i].positions.at(tree[i].positions.size()-1).intervals.at(k)<<"-";
 	     file<<tree[i].positions.at(tree[i].positions.size()-1).intervals.at(tree[i].positions.at(tree[i].positions.size()-1).intervals.size()-1)<<"}";	
 	     file<<"\n"; 	    
 	   }
	  file.close ();
	  return true;
 	}
   catch (...)
    { return false; }	  
 }


// private functions

// increase the array size
bool BinarySearchTree::doubleSize ()
 { this->maxSize = this->maxSize * 2;
   node* newTree = new node [this->maxSize];
   for (int i=0; i<this->size; i++)
     newTree[i] = this->tree[i];
   // delete old one
   if (this->tree) delete [] this->tree;
   this->tree = newTree;
 }
