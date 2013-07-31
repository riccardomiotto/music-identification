/***
	libraries, general function declaration, struct definitions
***/

// libraries
#include "Global.h" 
#include "Statistics.h"
#include "Index.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <math.h>
#include <dirent.h>
#include <sstream>


// namespaces
using namespace std;


/* functions */

// compare elements for quicksort
extern int compare (const void* a, const void* b);

// string tokenizer
extern void tokenize (string& str, vector<string>& tokens, string& delimiters);

// convert generic parameter to string
template <class Type> string toString (Type value)
 { stringstream sStream;
   sStream << value;     
   return sStream.str();
 }

// binary search in a ascending sorted generic array
template <class Type> int binarySearch (Type array [], int first, int last, Type key) 
 { if (first <= last) 
 	{ // compute mid point
      int mid = (first + last) / 2;
      if (key == array[mid]) 
        return mid; 
      else 
       if (key < array[mid]) 
        // call for the lower part of the array
        return binarySearch (array, first, mid-1, key);
       else
        // call for the upper part of the array
        return binarySearch (array, mid+1, last, key);
    }
   // not found
   return -1;  
 }

// computes statistics about the results
extern void computeStatistics (int* ranks, int queryNum);

// compute mean averag precision of a vector of results
extern float computeMeanAveragePrecision (vector<float> position);

// loads configuration parameters
extern bool loadChromaSettingsFromFile (char* settingsFilename);

// loads default parameters setting
extern bool loadDefaultChromaSetting (); 

// read the segments number
extern bool loadSegmentsNumber (char* s);

// read the segments number
extern bool loadTonalityInformation (char* s, short* tonality);

// write the segments number
extern bool writeSegmentsNumber (char* s, int n);


/* structure definition */

// position of the feature in the index
struct position
 { string id;
   vector<short> intervals;
 };
 
// node of the binary search tree 
struct node
 { unsigned long words;
   vector<position> positions;
   int parent, sx, dx;
 };

// compact index entry
struct entry
 { unsigned long word;
   vector<position> positionList;
   int size;
   // function to compare words
   bool operator () (const entry &a, const entry &b) const
    { return (a.word < b.word); }
 };
 
// query hash entry
struct queryHash
 { unsigned long word;
   short tonality;
   short genre;
   // kurtosis value to differ the retrieval
   double kurt;
   // variance value to differe retrieval
   double variance;
   // function to compare words
   bool operator () (const queryHash &a, const queryHash &b) const
    { return (a.word < b.word); }
 };
 
// result of the retrieval process
struct chromaResult
 { unsigned int id;
   double** bag;
 };

// compact result of the retrieval process
struct compactChromaResult
 { unsigned int id;
   double presence;
   unsigned short segment;
   unsigned short tonality;
   // function to compare results
   bool operator () (const compactChromaResult &a, const compactChromaResult &b) const
    { return (a.presence > b.presence); }
 };
  
// idf elements
struct idfEntry
 { unsigned long  word;
   double idf;
 };
 
 

