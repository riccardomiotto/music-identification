/***
	load the chroma matrix and compute the hash value
***/

#include "Libraries.h"
#include "ChromaHash.h"


// constructor
ChromaHash::ChromaHash (char* filename)
 { int nCols, nRows;
   // load the chroma matrix from file
   ifstream chroma (filename, ios::in); 
   // get rows number
   chroma >> nRows;
   // get columns number
   chroma >> nCols;
   this->chromaSize = nCols;  
   this->chromaNumber = nRows;
   this->chromaFeatures = new double* [nRows];
   for (int i=0; i<nRows; i++)
    { this->chromaFeatures[i] = new double [nCols];
      for (int j=0; j<nCols; j++) this->chromaFeatures[i][j] = 0.0; 
    }
   // get chroma values
   loadChromaFeatures (filename, nRows, 0, nCols, this->chromaFeatures, chroma);
   chroma.close ();
   this->hashValue = NULL;
 }
 
 
// constructor with transposition
ChromaHash::ChromaHash (char* filename, short tonality)
 { int nCols, nRows;
   // load the chroma matrix from file
   ifstream chroma (filename, ios::in); 
   // get rows number
   chroma >> nRows;
   // get columns number
   chroma >> nCols;
   this->chromaSize = nCols;  
   this->chromaNumber = nRows;
   this->chromaFeatures = new double* [nRows];
   for (int i=0; i<nRows; i++)
    { this->chromaFeatures[i] = new double [nCols];
      for (int j=0; j<nCols; j++) this->chromaFeatures[i][j] = 0.0; 
    }
   // get chroma values
   loadChromaFeatures (filename, nRows, 0, nCols, this->chromaFeatures, chroma);
   chroma.close ();
   
   // transpose according tonality
   if (tonality > 0)
    { for (int i=0; i<nCols; i++)
       { double* temp = new double [12];
	     for (int j=0; j<12; j++)
		   temp[j] = this->chromaFeatures[j][i];
	     short index = tonality;
	     short j = 0;
	     while (j<12)
	      { this->chromaFeatures[j][i] = temp[index];
	        j++;
			index++;
	        if (index == 12) index = 0;
          }
	   }
	}
   this->hashValue = NULL;
   
 }
 
 
// destructor
ChromaHash::~ChromaHash (void)
 { if (this->chromaFeatures)
 	{ for (int i=0; i<this->chromaNumber; i++)
 		delete [] this->chromaFeatures[i];
 	  delete [] this->chromaFeatures;
 	} 
   if (this->hashValue)
     delete [] this->hashValue;
   if (this->hashGenre) delete [] this->hashGenre;
   if (this->hashKurt) delete [] this->hashKurt;
 }


// clean hash structure
void ChromaHash::cleanHashStructure ()
 { // hash values
   if (this->hashValue)
    { delete [] this->hashValue;
      this->hashValue = NULL;
    }
   // clean statistical information
   if (this->hashGenre) 
    { delete [] this->hashGenre;
      this->hashGenre = NULL;
    }
   if (this->hashKurt) 
    { delete [] this->hashKurt;
      this->hashKurt = NULL;
    }
   if (this->hashVariance) 
    { delete [] this->hashVariance;
      this->hashVariance = NULL;
    }
 }
 
 
// retrieving variables
unsigned long* ChromaHash::getHashValue ()
 { return this->hashValue; }
 
 
// retrieving hash words genre
short* ChromaHash::getHashGenre ()
 { return this->hashGenre; }
 
 
// retrieving kurtosis value
double* ChromaHash::getHashKurtosis ()
 { return this->hashKurt; }
 
 
// retrieving variance value
double* ChromaHash::getHashVariance ()
 { return this->hashVariance; }
 
 
int ChromaHash::getSize ()
 { return this->size; }
 
     
int ChromaHash::getChromaSize ()
 { return this->chromaSize; }
 
 
short ChromaHash::getChromaNumber ()
 { return this->chromaNumber; }
 
  
// compute the hash value - public method
void ChromaHash::hashing (int n, int begin)
 { // compute hash value using first rank-based quantization
   this->computeHash_rankBased (n,begin); 
   
   // compute hash values using interval-based quantization
   // this->computeHash_intervalBased (n,begin);
   
   // compute hash values using hybrid-based quantization
   // this->computeHash_hybrid (n, begin);
   
 }
 
 
// private functions

// load the chroma features - read each line and tokenize it
void ChromaHash::loadChromaFeatures (char* filename, int nRows, int begin, int end, double** chromaFeatures, ifstream& chroma)
 { string line;
   string separator = ";";
   for (int i=begin; i<end; i++)
    { chroma >> line;
      vector<string> tokens;
      tokenize (line, tokens, separator);
      for (int j=0; j<nRows; j++)
       chromaFeatures[j][i] = (double) atof (tokens[j].c_str ());
      tokens.clear ();
    }
 }
  
  
// compute hash values
void ChromaHash::computeHash_rankBased (int n, int begin)
 { // release memory
   if (this->hashValue) this->cleanHashStructure ();
 	
   // initialize variables
   unsigned long* qc = new unsigned long [n];
   short* q_genre = new short [n];
   double* kurtosis_value = new double [n];
   double* variance_value = new double [n];
   for (int i= 0; i<n; i++) 
    { qc[i] = 1;
      // quatization genre is 1 (rank)
      q_genre[i] = 1;
      kurtosis_value[i] = 0;
      variance_value[i] = 0;
    }
   int* qb = new int [this->chromaNumber];
   for (int i= 0; i<this->chromaNumber; i++) 
    qb[i] = 0;
   int finalSize = 0;
   short bins = this->chromaNumber-1;
     
   // single value representation with downsample factor 
   for (int k=0; k<n; k=k+DOWNSAMPLE_STEP)
    { // rank based quantization 
      int col = k + begin;
      
      // check threshold sum energy value
      double sumChroma = 0;
      for (int j=0; j<this->chromaNumber; j++) sumChroma = sumChroma + this->chromaFeatures[j][col];
     
      // check energy threshold bound
      if (sumChroma > ENG_THRESHOLD) 
       { // normalize and temporaly save chroma features
         double* temp = new double [this->chromaNumber];
         for (int j=0; j<this->chromaNumber; j++) 
          { this->chromaFeatures[j][col] /= sumChroma;
            temp[j] = this->chromaFeatures[j][col];
	      }
	      
	     // compute kurtosis
	     Statistics* stat = new Statistics (temp, this->chromaNumber);
	     stat->setKurtosis();
	     double kurt = stat->getKurtosis ();
	     variance_value[k] = stat->getVariance ();
	     
	     // release memory
	     if (stat) delete stat;
	     if (temp) delete [] temp;
	     
	     // set quantization step
	     short q_step = Q_LEV;
	     if (kurt >= 9) q_step = 3;
	     else
	      { if (kurt >= 3.5) q_step = 5;
	        else 
	         q_step = Q_LEV;   
		  }
		 
		 if (kurt >= 2)
		  { kurtosis_value[k] = kurt;
	        double** sortedChroma = new double*[this->chromaNumber];
            for (int i=0; i<this->chromaNumber; i++) 
             { sortedChroma[i] = new double [2];
       	       sortedChroma[i][0] = sortedChroma[i][1] = 0;
             }
      
            // rank-based quantization
            sortChroma (this->chromaFeatures, col, sortedChroma);
            for (int j=0; j<this->chromaNumber; j++)
             { if ((j<(q_step-1)) && (sortedChroma[bins-j][0]>0))
       	        qb [(int)(sortedChroma[bins-j][1])-1] = j;
       	       else
       	        qb[(int)(sortedChroma[bins-j][1])-1] = Q_LEV - 1; 
             }
               
            // hashing
            qc[k] = 0;
            for (int j=0; j<this->chromaNumber; j++)
              qc[k] = (Q_LEV * qc[k]) + qb[j]; 
                
            // check if not already computed - compacting chroma
            if (this->checkPresence(qc,k) == 1) qc[k] = 1;
            else 
              finalSize++;
      
            // release memory
            if (sortedChroma) 
             { for (int i=0; i<this->chromaNumber; i++) delete [] sortedChroma[i]; 
       	       delete [] sortedChroma;
             }
          }
       }
    }
     
   // save results
   compactChroma (qc, q_genre, kurtosis_value, variance_value, finalSize, n);
   
   // release memory
   if (qb) delete [] qb;
   if (qc) delete [] qc;
   if (q_genre) delete [] q_genre;
   if (kurtosis_value) delete [] kurtosis_value;
   if (variance_value) delete [] variance_value;
    
 }


// compute hash values
void ChromaHash::computeHash_intervalBased (int n, int begin)
 { // release memory
   if (this->hashValue) this->cleanHashStructure ();
   
   // initialize variables
   unsigned long* qc = new unsigned long [n];
   short* q_genre = new short [n];
   for (int i= 0; i<n; i++) 
    { qc[i] = 1;
      // quatization genre is 2 (interval)
      q_genre[i] = 2; 
    }
   int* qb = new int [this->chromaNumber];
   for (int i= 0; i<this->chromaNumber; i++) qb[i] = 0;
   int finalSize = 0;
      
   short bins = this->chromaNumber-1;
   
   // single value representation    
   for (int k=0; k<n; k=k+DOWNSAMPLE_STEP)
    { // interval based quantization 
      int col = k + begin;
       
      // check threshold sum energy value
      double sumChroma = 0;
      for (int j=0; j<this->chromaNumber; j++) sumChroma = sumChroma + this->chromaFeatures[j][col];
     
      if (sumChroma > ENG_THRESHOLD) 
       { // normalize and temporaly save chroma features
         double* temp = new double [this->chromaNumber];
         for (int j=0; j<this->chromaNumber; j++) 
          { this->chromaFeatures[j][col] /= sumChroma;
            temp[j] = this->chromaFeatures[j][col];
	      }
	     
         // sort normalized features
         double** sortedChroma = new double*[this->chromaNumber];
         for (int i=0; i<this->chromaNumber; i++) 
          { sortedChroma[i] = new double [2];
       	    sortedChroma[i][0] = sortedChroma[i][1] = 0;
          }
         sortChroma (this->chromaFeatures, col, sortedChroma);
         
         // rank interval-based
         for (int j=0; j<this->chromaNumber; j++)
          { if (j<Q_LEV)
             { // find interval (5 intervals, the lowest is mapped as Q_LEV)
          	   if (sortedChroma[bins-j][0]<0.05) qb[(int)(sortedChroma[bins-j][1])-1] = 0;
          	   if ((sortedChroma[bins-j][0]>=0.05) && (sortedChroma[bins-j][0]<0.1)) qb[(int)(sortedChroma[bins-j][1])-1] = 1;
          	   if ((sortedChroma[bins-j][0]>=0.1) && (sortedChroma[bins-j][0]<0.2)) qb[(int)(sortedChroma[bins-j][1])-1] = 2;
          	   if ((sortedChroma[bins-j][0]>=0.2) && (sortedChroma[bins-j][0]<0.4)) qb[(int)(sortedChroma[bins-j][1])-1] = 3;
          	   if (sortedChroma[bins-j][0]>=0.4) qb[(int)(sortedChroma[bins-j][1])-1] = 4; 
          	 }
          	else 
          	 qb[(int)(sortedChroma[bins-j][1])-1] = 0;
          }
          
         // hashing
         qc[k] = 0;
         for (int j=0; j<this->chromaNumber; j++)
           qc[k] = (5 * qc[k]) + qb[j];  
              
         // check if not already computed - compacting chroma
         if (this->checkPresence(qc,k) == 1) qc[k] = 1;
         else finalSize++;
      
         // release memory
         if (sortedChroma) 
          { for (int i=0; i<this->chromaNumber; i++) delete [] sortedChroma[i]; 
       	    delete [] sortedChroma;
          }  
       }
    }     
   
   // save results
   compactChroma (qc, q_genre, NULL, NULL, finalSize, n);
   
   // release memory
   if (qb) delete [] qb;
   if (qc) delete [] qc;
   if (q_genre) delete [] q_genre;
 }  
 
 
// compute hask values - hybrid interval-rank based quantization
void ChromaHash::computeHash_hybrid (int n, int begin)
 { // release memory
   if (this->hashValue) this->cleanHashStructure ();
 	
   // initialize variables
   int dim = n*2;
   unsigned long* qc = new unsigned long [dim];
   short* q_genre = new short [dim];
   double* kurtosis_value = new double [n];
   double* variance_value = new double [n];
   for (int i= 0; i<dim; i++) 
    { qc[i] = 1;
      // quatization genre is mixed
      q_genre[i] = 0;
      kurtosis_value[i] = 0;
      variance_value[i] = 0;
    }
   int* qb_rank = new int [this->chromaNumber];
   int* qb_interval = new int [this->chromaNumber];
   for (int i=0; i<this->chromaNumber; i++) 
    { qb_rank[i] = 0;
      qb_interval[i] = 0;
    }
   int finalSize = 0;
   short bins = this->chromaNumber - 1;
   
   int ind = 0;
     
   // single value representation with downsample factor 
   for (int k=0; k<n; k=k+DOWNSAMPLE_STEP)
    { 
      // rank based quantization 
      int col = k + begin;
      
      // check threshold sum energy value
      double sumChroma = 0;
      for (int j=0; j<this->chromaNumber; j++) sumChroma = sumChroma + this->chromaFeatures[j][col];

      if (sumChroma > ENG_THRESHOLD)
       { // normalize and temporaly save chroma features
         double* temp = new double [this->chromaNumber];
         for (int j=0; j<this->chromaNumber; j++) 
          { this->chromaFeatures[j][col] /= sumChroma;
            temp[j] = this->chromaFeatures[j][col];
	      }
	      
	     // compute kurtosis
	     Statistics* stat = new Statistics (temp, this->chromaNumber);
	     stat->setKurtosis();
	     double kurt = stat->getKurtosis ();
	     variance_value[k] = stat->getVariance ();
	     // release memory
	     if (stat) delete stat;
	     if (temp) delete [] temp;
	     
	     // set quantization step
	     short q_step = Q_LEV;
	     if (kurt >= 9) q_step = 3;
	     else
	      { if (kurt >= 4) q_step = 5;
	        else 
	         q_step = Q_LEV;   
		  } 
		 kurtosis_value[k] = kurt;
       	 
       	 double** sortedChroma = new double*[this->chromaNumber];
         for (int i=0; i<this->chromaNumber; i++) 
          { sortedChroma[i] = new double [2];
       	    sortedChroma[i][0] = sortedChroma[i][1] = 0;
          }
       
         // quantization
         sortChroma (this->chromaFeatures, col, sortedChroma);
         for (int j=0; j<this->chromaNumber; j++)
          { // find rank
            if ((j<(q_step-1)) && (sortedChroma[bins-j][0]>0))
       	      qb_rank[(int)(sortedChroma[bins-j][1])-1] = j;
       	    else
       	      qb_rank[(int)(sortedChroma[bins-j][1])-1] = Q_LEV - 1;
       	    
       	    // find interval (5 intervals, the lowest is mapped as 0)
          	if (j<(q_step-1))
          	 { if (sortedChroma[bins-j][0]<0.05) qb_interval[(int)(sortedChroma[bins-j][1])-1] = 0;
          	   if ((sortedChroma[bins-j][0]>=0.05) && (sortedChroma[bins-j][0]<0.1)) qb_interval[(int)(sortedChroma[bins-j][1])-1] = 1;
          	   if ((sortedChroma[bins-j][0]>=0.1) && (sortedChroma[bins-j][0]<0.2)) qb_interval[(int)(sortedChroma[bins-j][1])-1] = 2;
          	   if ((sortedChroma[bins-j][0]>0.2) && (sortedChroma[bins-j][0]<0.4)) qb_interval[(int)(sortedChroma[bins-j][1])-1] = 3;
          	   if (sortedChroma[bins-j][0]>=0.4) qb_interval[(int)(sortedChroma[bins-j][1])-1] = 4;   
          	 }
            else 
          	 qb_interval[(int)(sortedChroma[bins-j][1])-1] = 0;
          }
         
         // hashing rank values
         qc[ind] = 0;
         for (int j=0; j<this->chromaNumber; j++)
          qc[ind] = (Q_LEV * qc[ind]) + qb_rank[j];
          
         // check if not already computed - compacting chroma
         if (this->checkPresence(qc,ind) == 1) qc[ind] = 1;
         else 
          { finalSize++;
            q_genre[ind] = 1;
          }
          
         ind++;
         
         // hashing interval values
         qc[ind] = 0;
         for (int j=0; j<this->chromaNumber; j++)
          qc[ind] = (5 * qc[ind]) + qb_interval[j];
            
         // check if not already computed - compacting chroma
         if (this->checkPresence(qc,ind) == 1) qc[ind] = 1;
         else 
          { finalSize++;
            q_genre[ind] = 2;
          }
         
         ind++;
         
         // release memory
         if (sortedChroma) 
          { for (int i=0; i<this->chromaNumber; i++) delete [] sortedChroma[i]; 
       	    delete [] sortedChroma;
          }
     
       }
    }   
     
   // save results
   compactChroma (qc, q_genre, kurtosis_value, variance_value, finalSize, n);
   
   // release memory
   if (qb_rank) delete [] qb_rank;
   if (qb_interval) delete [] qb_interval;
   if (qc) delete [] qc;
   if (q_genre) delete [] q_genre;
   
 }
 
 
// sort chroma array
void ChromaHash::sortChroma (double** features, int col, double** sortedChroma)
 { // create local chroma array
   double* temp = new double[this->chromaNumber];
   for (int i=0; i<this->chromaNumber; i++) temp[i] = features[i][col];
    // sort the values
   for (int i=0; i<this->chromaNumber; i++)
    { double minValue = 1000000000;
      int minInd = 0;
      for (int j=0; j<this->chromaNumber; j++)
        if ((temp[j]>=0) && (temp[j]<minValue)) 
         { minValue = temp[j];
           minInd = j;
         } 
      sortedChroma[i][0] = minValue;
      sortedChroma[i][1] = minInd + 1;    
      temp[minInd] = -1;  
    }
   if (temp) 
    delete [] temp; 	
 }
 
 
 // check presence of chroma word - compacting chroma
 int ChromaHash::checkPresence (unsigned long* hash, int k)
  { int j = k-1;
    short equal = 0;
    while ((j >= 0) && (equal == 0))
     { if ((hash[j] == hash[k]))
        equal = 1;
       else 
        j--;
     }
    return equal;
  }
  
  
// compactchroma hash words
void ChromaHash::compactChroma (unsigned long* hash, short* q_genre, double* kurtosis_value, double* variance_value, int newSize, int n)
 { this->size = newSize; 
   this->hashValue = new unsigned long [newSize];
   this->hashGenre = new short [newSize];
   this->hashKurt  = new double [newSize];
   this->hashVariance = new double [newSize];
   int j = 0;
   for (int i=0; i<n; i++)
    { if (hash[i]!=1) 
    	{ this->hashValue[j] = hash[i];
    	  this->hashGenre[j] = q_genre[i];
    	  //this->hashKurt[j] = kurtosis_value[i];
    	  //this->hashVariance[j] = variance_value[i];
    	  j++;
    	}
    }
   
 }
 
