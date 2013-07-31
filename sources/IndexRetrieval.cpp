/***
	retrieval based on chroma features stored in the pre-computed index
***/

#include "IndexRetrieval.h"


// constructors
IndexRetrieval::IndexRetrieval (int nSongs):GlobalIndex()
 { this->query = NULL;
   this->querySize = 0;
   this->collectionSize = nSongs;
   // prepare results structure
   this->results = new chromaResult [this->collectionSize];
   for (int i=0; i<this->collectionSize; i++)
    { this->results[i].id = i + 1;
      this->results[i].bag = new double* [MAX_INTERVALS];
      for (int j=0; j<MAX_INTERVALS; j++)
       { this->results[i].bag[j] = new double [12];
       	 for (int k=0; k<12; k++) this->results[i].bag[j][k] = 0;
       }      
    }
   // compact results
   this->compactResults = NULL;
   // results size
   this->resultSize = this->collectionSize;
 }

 
IndexRetrieval::IndexRetrieval (char* filename, int nSongs):GlobalIndex(filename)
 { // load the idf structure name
   string s = toString (filename);
   size_t found = s.find_last_of("/\\");
   s = s.substr(0,found) + "/IDF_" + s.substr(found+1);
   this->idfName = new char [MAX_PATH];
   strcpy (this->idfName, s.c_str());
   this->query = NULL;
   this->querySize = 0;
   this->collectionSize = nSongs;
   // prepare results structure
   this->results = new chromaResult [this->collectionSize];
   for (int i=0; i<this->collectionSize; i++)
    { this->results[i].id = i + 1;
      this->results[i].bag = new double* [MAX_INTERVALS];
      for (int j=0; j<MAX_INTERVALS; j++)
       { this->results[i].bag[j] = new double [12];
       	 for (int k=0; k<12; k++) this->results[i].bag[j][k] = 0;
       }      
    }
   // compact results
   this->compactResults = NULL;
   // results size
   this->resultSize = this->collectionSize;
 }

 
// destructor
IndexRetrieval::~IndexRetrieval (void)
 { if (this->query)
     delete [] this->query;
   if (this->results)
     delete [] this->results;
   if (this->compactResults)
     delete [] this->compactResults;
 }
 
 
// retrieval of parameters
queryHash* IndexRetrieval::getQuery ()
 { return this->query; }


int IndexRetrieval::getQuerySize ()
 { return this->querySize; }

 
int IndexRetrieval::getCollectionSize ()
 { return this->collectionSize; }

 
chromaResult* IndexRetrieval::getResults ()
 { return this->results; }


compactChromaResult* IndexRetrieval::getCompactResults ()
 { return this->compactResults; }

 
int IndexRetrieval::getResultSize()
 { return this->resultSize; }

 
// query identification
bool IndexRetrieval::identification (char* name)
 { // search the features
   this->search (name);
   // compact results
   this->compactChromaResults();  
 }

 
// clear results structure
bool IndexRetrieval::clear ()
 { // clear results
   for (int i=0; i<this->collectionSize; i++)
    { this->results[i].id = 0;
      for (int j=0; j<MAX_INTERVALS; j++)
       for (int k=0; k<12; k++) this->results[i].bag[j][k] = 0;      
    }
   // clear the compact results
   if (this->compactResults)
    { for (int i=0; i<this->resultSize; i++)
       { this->compactResults[i].id = 0;
         this->compactResults[i].presence = 0;
         this->compactResults[i].segment = 0;
         this->compactResults[i].tonality = 0;
       }
    }
   // clear the query
   if (this->query) delete [] this->query;
   this->query = NULL;
   this->querySize = 0;
 }



// protected functions
 
// search the features
bool IndexRetrieval::search (char* name)
 { // load the chroma features and compute the hash valuea
   ChromaHash* hash = new ChromaHash (name);
   hash->hashing (DURATION, BEGIN);
   // save query hash values as first tonality
   this->querySize = hash->getSize ();
   this->query = new queryHash [this->querySize];
   for (int i=0; i<hash->getSize(); i++)
    { this->query[i].word = hash->getHashValue()[i];
      this->query[i].tonality = 1;  
      this->query[i].genre = hash->getHashGenre()[i];
      this->query[i].kurt = hash->getHashKurtosis()[i];
      this->query[i].variance = hash->getHashVariance()[i];
    }
    
   // release memory
   if (hash) delete hash;  
   
   // compute transposition bounds
   short up=0, down=0;
   this->computeTranspositionBounds (&up, &down);
    
   // retrieval of the features
   for (int i=0; i<SEMITONES; i++)
    { // transpose features - tonality shift
      short q_step;
      for (int j=0; j<this->getQuerySize(); j++)  
       { // quantization step according the hash vaue genre
       	 if (this->query[j].genre == 1) q_step = Q_LEV;
         else q_step = 5;
         // compute transpose
       	 unsigned long w = this->query[j].word;
         unsigned long r = w % q_step;
         w = (unsigned long) floor ((double)w/q_step);
         w = w + (unsigned long)(r * (pow(q_step, 11)));
	     this->query[j].word = w;
         this->query[j].tonality = i + 2;
       }          
	}
    this->retrieve ();
 }

      
// retrieval function
bool IndexRetrieval::retrieve()
 { // retrieve chroma features
   query_mean += (double) this->querySize;
   for (int i=0; i<this->querySize; i++)
    { // do the binary search
      int found = this->binarySearch (this->query[i].word, 0, this->size-1);
      // check found and the chroma feature presence
      if (found >= 0)
       { // get the positions
       	 for (int j=0; j<this->index[found].positionList.size(); j++)
       	  { // get the id
       	  	int id = atoi (this->index[found].positionList.at(j).id.c_str());
       	  	// get the intervals
       	  	for (int k=0; k<this->index[found].positionList.at(j).intervals.size(); k++)
       	  	 { // increment the results entry (bag of words)
       	  	   results[id-1].bag[this->index[found].positionList.at(j).intervals.at(k)-1][this->query[i].tonality-1] ++;
       	  	   // increment the results entry (IDF) 
                   // results[id-1].bag[this->index[found].positionList.at(j).intervals.at(k)-1][this->query[i].tonality-1] += this->idf[found].idf;
       	  	 }
       	  }
       }  
    }
   return true;
 }

 
// compute the rank measure
double IndexRetrieval::computeRank (double** bag, short t, int n, int* firstSegment)
 { double avg = 0;
   for (int i=0; i<N_SEGMENTS; i++)
    { // find the max element in result
      int max = this->findMax (bag, t, MAX_INTERVALS);
      if (i==0) *firstSegment = max;
      // save max value
      avg += bag[max][t]; 
      // update local bag
      bag[max][t] = -1;
    }       
   avg /= (double)N_SEGMENTS;  
   return avg;
 }

 
// compute transposition bounds
void IndexRetrieval::computeTranspositionBounds (short* up, short* down)
 { // compute transposition bounds
   if (SEMITONES <= 0)
    { // no transpositions
      *up = 12;
      *down = 1;
    }
   else
    { if (SEMITONES > 5)
       { // all transpositions
         *up = *down = 11; 
       }
      else
       { // SEMITONES * 2 transpositions
       	 *up = 12 - SEMITONES;
       	 *down = SEMITONES + 1;
       }
    }
 }
 
 
// private functions

// compact the results
bool IndexRetrieval::compactChromaResults ()
 { // prepare the results array
   this->compactResults = new compactChromaResult [this->resultSize];
   for (int i=0; i<this->resultSize; i++)
    { this->compactResults[i].id = 0;
      this->compactResults[i].presence = 0;
      this->compactResults[i].segment = 0;
      this->compactResults[i].tonality = 0;
    }
   
   // compute rank value for each element of the collection
   for (int i=0; i<this->collectionSize; i++)
    { short maxTonality = -1;
      double maxRank = -1;
      int maxSegment = -1;
      
      // compute rank value for each tonality   
      this->compactResults[i].id = i + 1;
      for (short j=0; j<12; j++)
       { // compute rank
         int localMaxSegment;
         double rank = this->computeRank (this->results[i].bag, j, MAX_INTERVALS, &localMaxSegment);
         // check max rank value
         if (rank > maxRank)
          { maxRank = rank;
            maxTonality = j + 1;
            maxSegment = localMaxSegment;
		  }  
       }
       
      // save best rank value
      this->compactResults[i].presence = maxRank;  
      this->compactResults[i].segment = maxSegment;
      this->compactResults[i].tonality = maxTonality;
    }   	
      
   // sort the results
   sort (this->compactResults, this->compactResults + this->resultSize, compactChromaResult()); 
   
 }
 
   
// find the largest element of a bag vector
int IndexRetrieval::findMax (double** bag, short t, int n)
 { int maxInd = -1;
   double maxValue = -1;
   for (int i=0; i<n; i++)
    { if (bag[i][t]>maxValue) 
       { maxValue = bag[i][t];
         maxInd = i;
	   }
	}
   return maxInd;
}


