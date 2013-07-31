/***
	general method implementation
	
	@declaration: Libraries.h
***/

#include "Libraries.h"


// function to compare elements during QuickSort
int compare (const void * a, const void * b) { return ( *(int*)a - *(int*)b ); }

 
// string tokenizer
void tokenize (string& str, vector<string>& tokens, string& delimiters)
 { // skip delimiters at beginning.
   string::size_type lastPos = str.find_first_not_of (delimiters, 0);
   // find first "non-delimiter".
   string::size_type pos = str.find_first_of (delimiters, lastPos);
   while (string::npos!=pos || string::npos!=lastPos)
    { // found a token, add it to the vector.
      tokens.push_back (str.substr (lastPos, pos-lastPos));
      // skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of (delimiters, pos);
      // find next "non-delimiter"
      pos = str.find_first_of (delimiters, lastPos);
    }
 }
 
  
// computes statistics about the results
void computeStatistics (int* ranks, int queryNum)
 { float mean_average_precision = 0.0;
   int queryMatched = 0;
   float probability [9];
   for (int i=0; i<9; i++) probability[i] = 0;
   
   for (int i=0; i<queryNum; i++) 
    { if (ranks[i] != 0)
       { // probability
         if (ranks[i] == 1)   probability[0]++;
         if (ranks[i] <= 3)   probability[1]++;
         if (ranks[i] <= 5)   probability[2]++;
         if (ranks[i] <= 10)  probability[3]++;
         if (ranks[i] <= 20)  probability[4]++;
         if (ranks[i] <= 50)  probability[5]++;
         if (ranks[i] <= 100) probability[6]++;
         if (ranks[i] <= 150) probability[7]++;
         if (ranks[i] <= 200) probability[8]++;
         // mean average precision
         mean_average_precision += float (1.0/(float)ranks[i]);;
         queryMatched++;
       }
    }

   for (int i=0; i<9; i++)
     probability[i] = (probability[i]/queryMatched) * 100;
   mean_average_precision = (mean_average_precision/queryMatched) * 100;

   // print final results
   cout<<" TOTAL RESULTS: "<<endl<<endl;

   cout<<" Probability:   =  1   --->  "<<probability[0]<<"% "<<endl;
   cout<<"               <=  3   --->  "<<probability[1]<<"% "<<endl;
   cout<<"               <=  5   --->  "<<probability[2]<<"% "<<endl;
   cout<<"               <= 10   --->  "<<probability[3]<<"% "<<endl;
   cout<<"               <= 20   --->  "<<probability[4]<<"% "<<endl;
   cout<<"               <= 50   --->  "<<probability[5]<<"% "<<endl;
   cout<<"               <= 100  --->  "<<probability[6]<<"% "<<endl;
   cout<<"               <= 150  --->  "<<probability[7]<<"% "<<endl;
   cout<<"               <= 200  --->  "<<probability[8]<<"% "<<endl;

   cout<<endl<<" Mean Average Precision = "<<mean_average_precision<<"% "<<endl;

 }
 
 
// loads chroma configuration parameters
bool loadChromaSettingsFromFile (char* settingsFilename)
 { FILE* stream;
   char buffer[MAX_DATA];
   int dataLen;
   if ((stream = fopen (settingsFilename, "r+t")) != NULL)
    { dataLen = fread (buffer, sizeof (char), MAX_DATA, stream);
      if (dataLen)
	   { try
	      { //string support;
            sscanf (buffer,"%d; %d; %d;\n\n %d; %d;\n\n %d; %hd;\n\n %hd;\n\n %hd; %hd;\n\n %f;\n\n %hd;\n", &Q_LEV, &HOP, &SEC_LENGTH, &BEGIN, &DURATION, &MAX_INTERVALS, &N_SEGMENTS, &SEMITONES, &UP_PERCENTAGE, &DOWN_PERCENTAGE, &ENG_THRESHOLD, &DOWNSAMPLE_STEP);
	      }
	     catch(...)
	      { loadDefaultChromaSetting ();
	        fclose (stream);
            return false;
	      }
	   }
      fclose (stream);
      return true;
    }
   else 
    { loadDefaultChromaSetting ();
      return false;	
    }
 }
 
 
// loads default parameters setting
bool loadDefaultChromaSetting () 
 { // quantization step
   Q_LEV = 6;
   // indexing hop size - 5 seconds
   HOP = 1204;
   // begin of the query
   BEGIN = 0;
   // second related to chroma bins
   SEC_LENGTH = 172;
   // duration of the query - 8 seconds
   DURATION = 1376; 
   // max segments number
   MAX_INTERVALS = 150;
   // intervals considered in the retrieving
   N_SEGMENTS = 2;                                   
   // semitones used in the query transposition (half up and half down)
   SEMITONES = 4;
   // percentage to cut the index tails
   UP_PERCENTAGE = 80;
   DOWN_PERCENTAGE = 80;
   // total chroma energy threshold value
   ENG_THRESHOLD = 0.2;
   // downsamplig step in chroma hashing computation
   DOWNSAMPLE_STEP = 3;              
 }
 
 
// read the segments number
bool loadTonalityInformation (char* s, short* tonality)
 { try
 	{ ifstream file;
 	  file.open (s, ios::in); 
      // if empty then set number to 0
      if (file.fail()) return false;
      // if file exists, load tonality ifnroamtio
	  int i=0;
	  string info;
	  while (getline (file, info))
	   { // parse row
		 vector<string> tokens;
         string separator = ";";
         tokenize (info, tokens, separator);
         tonality[i] = atoi(tokens.at(1).c_str());
		 i++;
		 tokens.clear();
	   }
      file.close ();
      return true;	  
 	}
   catch (...)
    { return false; }
 }
 
 
// read the segments number
bool loadSegmentsNumber (char* s)
 { try
 	{ ifstream file;
 	  file.open (s, ios::in); 
      // if empty then set number to 0
      if (file.fail()) return 0;
      // if file exists, load segments number in memory
      file>>COLLECTION_SEGMENTS;
      file.close ();
      return true;	  
 	}
   catch (...)
    { return false; }
 }


// write the segments number
bool writeSegmentsNumber (char* s, int n)
 { try
    { ofstream file;
      file.open (s);
      file<<n<<"\n";
	  file.close ();
	  return true;
 	}
   catch (...)
    { return false; }	 
 }
 
 
// compute mean averag precision of a vector of results
float computeMeanAveragePrecision (vector<float> position)
 { float map = 0.0;
   for (int i=0; i<position.size(); i++)
     map += position.at(i);
   map = (map/position.size()) * 100;
   return map;  
 }
 
 
// compute variance of double array
float variance (double* v, int n)
 { // compute mean   
   double m = 0;
   for (int i=0; i<n; i++) m += v[i];
   m /= n;
   double var = 0;
   for (int i=0; i<10; i++)
    var +=  pow((v[i]-m),2);
   var /= n;
   return var; 
 }


 

 

