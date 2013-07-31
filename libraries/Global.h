/***
	global variables
***/

#define CHROMA_EXT  "txt"
#define MAX_PATH     300
#define MAX_DATA    1000


/* indexing parameters */

// quantization step
extern int Q_LEV;   
                   
// indexing hop size
extern int HOP;                          

// chroma bin related to a second
extern int SEC_LENGTH;                   

// chroma query begin
extern int BEGIN;                        

// chroma query duration size
extern int DURATION;                     

// number of max intervals in the indexed recordings
extern int MAX_INTERVALS;                

// number of the intervals considered in the retrieving
extern short N_SEGMENTS;                 

// number of semitones used in the query transposition (half up and half down)
extern short SEMITONES;                  

// up percentage to cut the top tail of the index
extern short UP_PERCENTAGE;              

// down percentage to cut the bottom tail of the index  
extern short DOWN_PERCENTAGE;            

// number of the indexed segments (file: collectionSegmentsNumber.txt)
extern int COLLECTION_SEGMENTS;          

// total chroma energy threshold value 
extern float ENG_THRESHOLD;              

// downsamplig step in chroma hashing computation
extern short DOWNSAMPLE_STEP;   

// mean of the query length
extern double query_mean;                




