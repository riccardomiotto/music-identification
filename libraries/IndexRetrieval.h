//------------------------------------------------------------------------------
//
//                               Index Retrieval
//
//------------------------------------------------------------------------------
//
// IndexRetrieval.h
//
// Resposability: music identification based on chroma features stored in the
//                pre-computed index
//
//------------------------------------------------------------------------------

#include "GlobalIndex.h"
#include "ChromaHash.h"


class IndexRetrieval:public GlobalIndex
 { 
   protected:
 	 queryHash* query;
 	 int querySize;
 	 int collectionSize;
 	 chromaResult* results;
 	 compactChromaResult* compactResults;
 	 int resultSize;
 	 
   public:
     // constructors and destructors
     IndexRetrieval (int nSongs);
     IndexRetrieval (char* filename, int nSongs);
     ~IndexRetrieval (void);
     
     // retrieval of parameters
     queryHash* getQuery ();
     int getQuerySize ();
     int getCollectionSize ();
     chromaResult* getResults ();
     compactChromaResult* getCompactResults ();
     int getResultSize ();
       
     // search the features
     bool identification (char* name);
     
     // clear results structure
     bool clear ();
   
   protected:   
     // retrieval function
     bool retrieve ();
     
     // search the features
     bool search (char* name);
     
     // compute the rank measure
     double computeRank (double** bag, short t, int n, int* firstSegment);
     
     // compute transposition bounds
     void computeTranspositionBounds (short* up, short* down);
      
   private:
     // compact the results finding the most important entries
     bool compactChromaResults ();
     
     // get the number of the collection entries stored in the database 
     int getCollectionDimension ();
     
     // get the number of the collection entries of a fixed duration stored in the database 
     int getCollectionDimension (int dim);
     
     // find the index of the largest element of a bag matrix
     int findMax (double** bag, short t, int n);
    
  };

