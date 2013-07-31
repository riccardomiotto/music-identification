/***
	hash index using chroma values
***/

#include "ChromaIndex.h"
#include "ChromaHash.h"


// constructors and destructors
ChromaIndex::ChromaIndex ()
 { this->index = new BinarySearchTree (TREE_SIZE); 
   this->compactIndex = NULL;
 }

 	  	  
// destructor
ChromaIndex::~ChromaIndex (void)
 { if (this->index)
 	 delete this->index;
   if (this->compactIndex)
     delete [] this->compactIndex;
 }

 
// creation of the index
bool ChromaIndex::create ()
 { this->index = new BinarySearchTree (TREE_SIZE); 
   this->compactIndex = NULL;
   return true; 
 }

         
// deletion of the index
bool ChromaIndex::clear ()
 { try
    { if (this->index)
       { delete this->index;
       	 this->index = NULL;
       }
      return true;
    }
   catch(...)
    { return false; } 
 }

     
// adding an element
bool ChromaIndex::addElement (char* filename, char* id, short tonality)
 { ChromaHash* hash = new ChromaHash (filename, tonality);
   
   int nIntervals = (int) (floor((hash->getChromaSize()-HOP)/HOP));
   if (nIntervals == 0) nIntervals = 1;
    
   // compute the hash 
   for (short q=0; q<nIntervals; q++)
    { int first, last, size;
      if (nIntervals > 1)
       { first = q * HOP;
         last = (q+2) * HOP - 1;
       }
      else
       { first = 0;
       	 last = hash->getChromaSize() - 1; 
       }
      size = last - first + 1;
 
      // compute hash features
      hash->hashing (size,first);
      
      // save features in the index (binary search tree)
      for (int i=0; i<hash->getSize(); i++)
        this->index->insert (hash->getHashValue()[i], id, (q+1));
    }
   
   // update the indexed segments number
   // COLLECTION_SEGMENTS += nIntervals; 
    
   if (hash) delete hash;
 
   return true; 
 }

 
// adding an element - with transposition
bool ChromaIndex::addTransposedElement (char* filename, char* id)
 { ChromaHash* hash = new ChromaHash (filename);
   
   int nIntervals = (int) (floor((hash->getChromaSize()-HOP)/HOP));
   
   // compute the hash 
   for (short q=0; q<nIntervals; q++)
    { int first = q * HOP;
      int last = (q+2) * HOP - 1;
      int size = last - first + 1;
 
      // compute hash features
      hash->hashing (size,first);
      
      // transpose the hash features
      int transposedSize = SEMITONES * hash->getSize ();
      unsigned long* word = new unsigned long [transposedSize]; 
      short up, down;
      // set the number of transposition
      if (SEMITONES == 1)
       { up = 12;
       	 down = 1;
       }
      else
       { up = 12 - (SEMITONES/2);
         down = SEMITONES/2;
       } 
      // compute hash transpositions
      int ind = 0;
      for (int i=0; i<hash->getSize(); i++)
       { unsigned long w = hash->getHashValue()[i];
         for (int j=0; j<12; j++)
    	  { if ((j<down) || (j>=up))
    	     { word[ind] = w;
               ind++;
    	     }
            // tonality shift
            unsigned long r = w % Q_LEV;
            w = (unsigned long) floor (w / Q_LEV);
            w = w + (unsigned long)(r * (pow(Q_LEV, 11)));
    	  }  
       }
      
      // save features in the index (binary search tree)
      for (int i=0; i<transposedSize; i++)
       this->index->insert (word[i], id, (q+1));
        
      // release memory
      if (word) delete [] word;
    }
    
   if (hash) delete hash;
   
   return true; 
 }

      
// retrieval of the query
bool ChromaIndex::retrieve ()
 { return true; }

  
// get the compact index
entry* ChromaIndex::getIndex ()
 { return this->compactIndex; }

 
// get the size of index
int ChromaIndex::getSize ()
 { return this->size; }

  
// compact the index in the final structure
bool ChromaIndex::indexing ()
 { try
 	{ this->size = this->index->getSize();
 	  if (this->size > 0)
 	   { this->compactIndex = new entry [this->size];
 	     int ind = 0;
 	     this->inOrderTreeWalk(0, ind);     
 	     return true;
 	   }
 	  else
 	   return false;
 	}
   catch(...)
    { return false; }
 }

  
// private functions

// in-order index tree walk - save the index in the compact structure
void ChromaIndex::inOrderTreeWalk (int x, int& ind)
 { if (x != -1)
 	 { this->inOrderTreeWalk(this->index->getBinarySearchTree()[x].sx, ind);
 	   // save entry
 	   entry e;
 	   e.word = this->index->getBinarySearchTree()[x].words;
 	   e.positionList = this->index->getBinarySearchTree()[x].positions;
 	   this->compactIndex[ind] = e;
 	   ind++;
 	   this->inOrderTreeWalk(this->index->getBinarySearchTree()[x].dx, ind);
 	 }
 }

