/***
	handle the information of an global index using chroma values of musical recordings
***/

#include "ChromaIndex.h"
 

class GlobalIndex
 { 
 	protected:
 	 	entry* index;
 	 	idfEntry* idf;
 	 	int size;
 	 	char* indexName;
 	 	char* idfName;
 	 
   	public:
    	 // constructors and destructor
     	GlobalIndex ();
     	GlobalIndex (char* filename);
     	~GlobalIndex (void);
     
     	// add localIndex to the globalIndex
     	short addLocalIndex (entry* localIndex, int length);
     
     	// load the global index
     	bool loadIndex ();
     
     	// load the idf strcuture
     	bool loadIdf ();
     
     	// get global index
     	entry* getGlobalIndex ();
     
     	// get idf structure
     	idfEntry* getIdfValues ();
     
     	// cut top and back tail according a prefixed percentage on the average
     	int resize ();
     
     	// get index size
     	int getSize ();
     
     	// get index name
     	char* getIndexName ();
     
     	// get the idf name
     	char* getIdfName ();
     
     	// save the index on the filesystem
     	bool save (entry* idx, int n);
     
     	// save the idf structure on the filesystem
     	bool saveIdf (int n);
     
   	protected:
     	// search in the global index
     	int search (unsigned long key, int first, int last);
     
     	// binary search in the global index
     	int binarySearch (unsigned long key, int first, int last);
     
     	// parse a line of the index
     	entry parseIndexLine (string line);
     
     	// check presence of id in the entry e
     	short checkPresence (entry e, string id);
     
     	// count the number of presence for each word
     	int countTotalWordsPresence (int& max);
             
  };
 
