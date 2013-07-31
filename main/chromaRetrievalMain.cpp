/***
	music identification based on chroma features
	
	@usage: ChromaRetrieval <chroma query files path> <index filename>
	
	@param chroma files: matrices n * 12, values tab-separated
	
	@config files: in directory "./config/"
***/


#include "IndexRetrieval.h"

// chroma indexing parameters
int Q_LEV;                       
int HOP;
int SEC_LENGTH;                         
int BEGIN;   
int DURATION;      
int MAX_INTERVALS;
short N_SEGMENTS;
int CLUSTER_SIZE;
short SEMITONES;
short UP_PERCENTAGE;
short DOWN_PERCENTAGE;
int COLLECTION_SEGMENTS;
float ENG_THRESHOLD;
short DOWNSAMPLE_STEP;

// find the the exact match position of the query
int findMatchPosition (compactChromaResult* r, int n, string idQuery);

// find all the match of the query in the collection
float findPositions (compactChromaResult* r, int n, string idQuery, string* stat);

// statistics structure
vector<string> statistics;

// query lenght mean
double query_mean;


int main (int argc, char* argv[])
 {
   char temp [MAX_PATH];
   char currentDir[MAX_PATH];

   system ("clear");

   if (argc < 3)
    { cout<<endl<<" Usage: ChromaRetrieval <chroma query files path> <chroma index filename>" <<endl<<endl; }
   else
    { cout<< endl << " Music Identification - Chroma-based Retrieval " <<endl<<endl;

      char filenameConfig [MAX_PATH] = "/config/chroma-config.txt";

      // get working directory
      getcwd (currentDir, MAX_PATH);
      
      // get complete config file path
      char config_filename [MAX_PATH];
      strcpy (config_filename, currentDir);
      strcat (config_filename, filenameConfig);
        
      char queryDir[MAX_PATH];
      strcpy (queryDir, argv[1]);
      
      char indexName[MAX_PATH];
      strcpy (indexName, argv[2]);
         
      // load configuration file
      if (!loadChromaSettingsFromFile (config_filename))
       { // file not found or not valid: defaults loaded
         cout<<" Configuration file not found or not valid: defaults loaded! "<<endl<<endl;
       }

      // create the index
      // NOTE: specify "somehow" the number of songs in the collection; this depends on how you will store the collection
      int nSongs = 1000;
      IndexRetrieval* index = new IndexRetrieval (indexName, nSongs);
      query_mean = 0;
       
      // profiling variables
      clock_t start, finish;
      float duration;
      
      // load old index
      cout<<" Loading Music Index stored in "<<indexName<<" ... "<<endl;
      if (index->loadIndex())
       { cout<<" Index Correctly Loaded and composed of "<<index->getSize()<<" entries "<<endl<<endl;
         
         // loading the idf structure
         cout<<" Loading Music IDF values stored in "<<index->getIdfName()<<" ... "<<endl;
         if (index->loadIdf())  cout<<" IDF values correctly loaded "<<endl<<endl;
         else cout<<" ERROR in loading the IDF structure - Default 1 values loaded (bag of words approach) "<<endl<<endl; 
         
         // query processing
         struct dirent *dp;
         
         // initialize global rankings array
         short nQuery = 0;
         vector<float> position;
         int cont = 0;
        
         DIR *dfd = opendir (queryDir);
         start = clock();
         if (dfd != NULL)
          { while ((dp=readdir(dfd)) != NULL)
	         { if (strlen(dp->d_name) > 4)
	            { char extension[4];
	              strncpy (extension, &dp->d_name[strlen(dp->d_name)-3], 3);
	              extension[3] = '\0';
	              if ((strcasecmp(extension, CHROMA_EXT)==0))
		           { char query [MAX_PATH];
		             strcpy (query, queryDir);
		             strcat (query, dp->d_name);  
		             char title [MAX_PATH];
                     strncpy (title, dp->d_name, strlen (dp->d_name) - 3);
                     title[strlen(dp->d_name)-4] = '\0';
                     cout<<" "<<(nQuery+1)<<" - "<<title<<endl;
                     // search the query in the collection
                     double bestAvg = 0;
                     string bestStatistic = "";
                     int bestBegin = 0;
                     // select samples in the chroma files - considering 5 segments
                     // NOTE: you can change it to make it more dynamic
                     int seg = 1000;
                     for (int g=0; g<5; g++)
                      { BEGIN = seg;
                        string stat = "";
                        // "random" sampling
                        seg += 5000;
                        try
                         { index->identification (query);
                           vector<string> tokens;
                           string separator = "-";
                           string name = toString (title);
                           // get prefix (song title)
                           tokenize (name, tokens, separator);
                           // find matches
                           float avg = findPositions (index->getCompactResults(), index->getResultSize(), toString(tokens.at(0)), &stat);
                           cout<<"   Begin: "<<(g/SEC_LENGTH)<<" sec. -- Avg: "<<avg<<endl;
                           if (avg!=0) cont++;
                           if (avg > bestAvg) 
                            { bestAvg = avg;
                              bestStatistic = stat;
                              bestBegin = g;
						    }
                           // clear results structure
                           tokens.clear ();
                           index->clear(); 
                         }
                        catch (...) { cout<<" End of file "<<endl; }
                      }
                     position.push_back (bestAvg);
                     statistics.push_back (bestStatistic);
                     cout<<endl<<" Best match position: "<<1/bestAvg<< " --- Begin: "<<bestBegin<<" sec. "<<endl<<endl;
                     nQuery++;
                     cout<<endl;
		           }
	            }
	         }
	        finish = clock ();
	        // compute elaboration time
            duration = (float)(finish - start) / CLOCKS_PER_SEC;
            cout<<" Number of processed queries = "<<nQuery<<endl<<endl;
            printf (" Elaboration time for the retrieval task = %.2f sec\n", duration);
            printf (" Mean elaboration time for each query = %.2f sec\n\n", duration/nQuery);
            cout<<" Query correctly ranked within the top 100 positions = "<<cont<<endl<<endl;
   
            query_mean /= nQuery;
            cout<<" Mean Query Lenght of "<<query_mean<<" has values "<<endl<<endl;
            // print statistics
            // computeStatistics (position, nElements);
            float map = computeMeanAveragePrecision (position);
            cout<<" Mean Average Precision = "<<map<<"% "<<endl<<endl;
            
            statistics.clear ();
            
           }
         else
          cout<<" Music query directory not found!!! "<<endl<<endl;
         
       }
      else
        cout<<" Error in loading the Music Index "<<endl<<endl;   
      
      // release memory
      if (index) delete index;
    }
    
   cout<<" Press Enter to continue "<<endl;
   cin.getline (temp, MAX_PATH);
   return 0;

 }
 
 
 
/* functions */

int findMatchPosition (compactChromaResult* r, int n, string idQuery)
 { int firstMatch = -1;
   for (int i=0; i<n; i++)
    { if (toString(r[i].id) == idQuery)
       { if (firstMatch == -1) 
       	   firstMatch = (i+1);
       	 cout<<"    Match in position n. "<<(i+1)<<" - Presence = "<<r[i].presence<<" - Segment n. - "<<(r[i].segment+1)<<" - Tonality n. "<<r[i].tonality<<endl;
       }	 
    } 	
   return firstMatch;
 }
 
 
// find all the match of the query in the collection
float findPositions (compactChromaResult* r, int n, string title, string* stat)
 { // set max size
   if (CLUSTER_SIZE < n) n = CLUSTER_SIZE;
   float map = 0.0;
   int cont = 0;
   int found = 0;
   string detail = toString(r[0].presence) + "&";
   int i = 0;
   while ((found == 0) && (i<n))
    { string name = toString(r[i].id);
      vector<string> tk;
      string sep = "-";
      tokenize (name, tk, sep);
      name = tk.at (0);
      if (name == title)
       { cout<<"   Correct match in position "<<(i+1)<<" - Presence = "<<r[i].presence<<" - Tonality = "<<r[i].tonality<<" - Segment = "<<r[i].segment<<endl;
         if (i!=0) cout<<"   First rank: "<<toString(r[0].id)<<" - Presence = "<<r[0].presence<<" - Tonality = "<<r[0].tonality<<" - Segment = "<<r[0].segment<<endl;
       	 cont++;
       	 map += (float) ((float)cont/(float)(i+1));
         found = 1;
         detail += toString(r[i].presence) + "&" + toString(i+1);
       } 
      i++;
    }
   if (found == 0) detail = toString(0) + "&" + toString(0);
   *stat = detail;
   if (map != 0) map /= cont;
   return map;
 }
