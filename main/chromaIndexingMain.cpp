/***
	indexing a music collection by chroma hash words
	
	@usage: ChromaIndexing <chroma files path> <index filename>
	
	@param chroma files: matrices n * 12, values tab-separated
	
	@config files: in directory "./config/"
***/


#include "GlobalIndex.h"

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


int main (int argc, char* argv[])
 {
   char temp [MAX_PATH];
   char currentDir[MAX_PATH];

   system ("clear");

   if (argc < 3)
    { cout<<endl<<" Usage:  ChromaIndexing <music files path> <index filename>" <<endl<<endl; }
   else
    { cout<< endl << " Music Identification - Dataset Chroma Indexing " <<endl<<endl;
    
      char filenameConfig [MAX_PATH] = "/config/chroma-config.txt";
      char segmentsNumber [MAX_PATH] = "/config/indexed-segments.txt";
	  
      // get working directory
      getcwd (currentDir, MAX_PATH);
      
      // get complete config file path
      char config_filename [MAX_PATH];
      strcpy (config_filename, currentDir);
      strcat (config_filename, filenameConfig);

      // get complete segments number file path
      char segmentsNumberFilename [MAX_PATH];
      strcpy (segmentsNumberFilename, currentDir);
      strcat (segmentsNumberFilename, segmentsNumber);
      
      char collectionDir[MAX_PATH];
      strcpy (collectionDir, argv[1]);
      
      char indexName[MAX_PATH];
      strcpy (indexName, argv[2]);
      
      // create the index
      ChromaIndex* index = new ChromaIndex ();
          
      // load configuration file
      if (!loadChromaSettingsFromFile (config_filename))
       { // file not found or not valid: defaults loaded
         cout<<" Configuration file not found or not valid: defaults loaded! "<<endl<<endl;
       }
	        	  
      // load segments number
      if (!loadSegmentsNumber(segmentsNumberFilename)) 
       { COLLECTION_SEGMENTS = 0;
       	 // file not found or not valid: defaults loaded
       	 cout<<" File for the Collection Segments Number: zero's value loaded! "<<endl<<endl;
       }
             
      // profiling variables
      clock_t start, finish;
      float duration;

      // directory process
      struct dirent *dp;
      int nElements = 0;
      
      DIR *dfd = opendir (collectionDir);
      vector<string> filename;
      start = clock();
      if (dfd != NULL)
       { while ((dp=readdir(dfd)) != NULL)
	      { if (strlen(dp->d_name) > 4)
	         { char estensione[4];
	           strncpy (estensione, &dp->d_name[strlen(dp->d_name)-3], 3);
	           estensione[3] = '\0';
	           if ((strcasecmp(estensione, CHROMA_EXT)==0))
		         filename.push_back ((string)dp->d_name);
	         }
	      }
	     closedir (dfd);
	     sort (filename.begin(), filename.end());
	     
	     // feature calculation
	     for (int i=0; i<filename.size(); i++)
	      { string audio = ((string)collectionDir) + filename.at(i);
		    string title = filename.at(i).substr(0, filename.at(i).size()-4);
		    nElements++;
            cout<<" "<<nElements<<" - "<<title<<endl;
            // compute and index the chroma features
			index->addElement ((char*)audio.c_str(), (char*)title.c_str(), 0);
		  }
		  
	     // indexing
         if (index->indexing ())
          { cout<<endl<<" Built Index composed of "<<index->getSize()<<" entries"<<endl;
          	// release the memory for the tree structure
          	index->clear ();
          	// get the old index
          	GlobalIndex* globalIndex = new GlobalIndex (indexName);
            // load old index
            cout<<endl<<" Loading previous index ... "<<endl;
          	globalIndex->loadIndex();
          	// create the new index
          	cout<<" Adding the new index ... "<<endl<<endl;
          	short y = globalIndex->addLocalIndex(index->getIndex(), index->getSize());  
          	// release memory
          	if (index) delete index; 
          	// resizing the index cutting top and back tail
          	cout<<" Resizing the index cutting the top and back tails and computing IDF values ... "<<endl;
          	int newSize = globalIndex->resize();     
          	cout<<" Index now composed by "<<newSize<<" entries "<<endl<<endl;
          	// saving the index 
            cout<<" Saving the index on the filesystem ... "<<endl<<endl;
          	if (globalIndex->save (globalIndex->getGlobalIndex(), newSize))
          	 { if (y == -1) 
          	     cout<<" Created new index in "<<indexName<<endl;
          	   else 
          	     cout<<" Updated index in "<<globalIndex->getIndexName()<<endl<<" Index now composed of "<<newSize<<" entries "<<endl<<endl;
          	 }
          	else
          	 cout<<" Error in saving the index in the filesystem "<<endl<<endl;
	        // saving the idf structure
	        if (globalIndex->saveIdf(newSize))
	         cout<<" Created the IDF structure in "<<globalIndex->getIdfName()<<" composed by "<<newSize<<" entries "<<endl;
	        else
	         cout<<" ERROR in saving the IDF structure "<<endl;
          	// release memory
          	if (globalIndex) delete (globalIndex);
          }
         else
          cout<<" ERROR in the indexing process "<<endl;
       } 
      else nElements = -1; 
      finish = clock();
      
      if (nElements==-1) 
        cout<<" Collection directory not found !!! "<<endl<<endl;
      else
       { if (nElements==0)
           cout<<endl<<" No elements in the collection to index !!!"<<endl<<endl;
         else 
           { cout<<endl<<" Number of the indexed elements: "<<nElements<<endl; 
           	 if (writeSegmentsNumber (segmentsNumberFilename, COLLECTION_SEGMENTS)) 
           	   cout<<" Indexed Segments Number updated to: "<<COLLECTION_SEGMENTS<<endl<<endl;
           	 else cout<<" Error in updating the Indexed Segments Number - Old value maintained "<<endl<<endl; 
             duration = (float)(finish - start) / CLOCKS_PER_SEC;
             printf (" Elaboration time for the indexing calculation = %.2f sec\n", duration);
             printf (" Mean elaboration time for each document = %.2f sec\n\n", duration/nElements);
           }
       }
    }

   cout<<" Press Enter to continue "<<endl;
   cin.getline(temp, MAX_PATH);
   return 0;
   
 }
 


