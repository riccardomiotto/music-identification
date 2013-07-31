/***
	compute the chroma hash value
***/


class ChromaHash
 { 
 	private: 
   	 	unsigned long* hashValue;
 	 	// type of the quatizaztion - 1: rank - 2: interval
 	 	short* hashGenre;
 	 	// kurtosis values
 	 	double* hashKurt;
 	 	// variance value
 	 	double* hashVariance;
 	 	int size, chromaSize;
 	 	double** chromaFeatures;
 	 	short chromaNumber;
  
  
   	public:
     	// constructors and destructor
     	ChromaHash (char* filename);
	 	ChromaHash (char* filename, short tonality);
     	~ChromaHash (void);
     
     	// retrieving variables
     	unsigned long* getHashValue ();
     
     	// retrieving hash words genre
     	short* getHashGenre ();
     
     	// retrieving kurtosis value
     	double* getHashKurtosis ();
     
     	// retrieving variance value
     	double* getHashVariance ();
     
     	// compute the hash value - public method
     	void hashing (int n, int begin);
     
     	// get the hash size
     	int getSize ();
     
     	// get the chroma features size
     	int getChromaSize ();
     
     	// get the number of chroma features (pca or not)
     	short getChromaNumber ();
     
     	// clean hash structure
     	void cleanHashStructure ();
     
     
   	private:
     	// load the chroma features
     	void loadChromaFeatures (char* filename, int nRows, int begin, int end, double** chromaFeatures, ifstream& chroma);
     
     	// compute hash values - classical(first) rank-based quantization
     	void computeHash_rankBased (int n, int begin);
     
     	// compute hash values - interval-based quantization
     	void computeHash_intervalBased (int n, int begin);
     
     	// compute hask values - hybrid interval-rank based quantization
     	void computeHash_hybrid (int n, int begin);
              
     	// sort chroma array
     	void sortChroma (double** features, int col, double** sortedChroma);
     
     	// check presence of chroma word - compacting chroma
     	int checkPresence (unsigned long* hash, int k);
     
     	// compactchroma hash words
     	void compactChroma (unsigned long* hash, short* genre, double* kurtosis_value, double* variance_value, int newSize, int n);
          
  };
 
