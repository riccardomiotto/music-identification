/***
	global song index using chroma values
***/

#include "GlobalIndex.h"


// constructors
GlobalIndex::GlobalIndex ()
 { this->indexName = new char [MAX_PATH];
   this->size = 0;
   this->index = NULL;
   this->idf = NULL;
 }
     
     
GlobalIndex::GlobalIndex (char* filename)
 { // save the name
   this->indexName = new char [MAX_PATH];
   strcpy (this->indexName, filename);
   // save the idf name
   string s = toString (filename);
   size_t found = s.find_last_of("/\\");
   s = s.substr(0,found) + "/IDF_" + s.substr(found+1);
   this->idfName = new char [MAX_PATH];
   strcpy (this->idfName, s.c_str());
   // initialize variables
   this->size = 0;
   this->index = NULL;
   this->idf = NULL;
 }


// destructor
GlobalIndex::~GlobalIndex (void)
 { if (this->indexName)
 	 delete [] this->indexName;
   if (this->idfName)
 	 delete [] this->idfName;
   if (this->index)
     delete [] this->index;
   if (this->idf)
     delete [] this->idf;
 }
     

// get global index
entry* GlobalIndex::getGlobalIndex ()
 { return this->index; }
     

// get index size
int GlobalIndex::getSize ()
 { return this->size; }
     

// get index name
char* GlobalIndex::getIndexName()
 { return this->indexName; }

 
// get the idf name
char* GlobalIndex::getIdfName ()
 { return this->idfName; }

 
// get idf structure
idfEntry* GlobalIndex::getIdfValues ()
 { return this->idf; }
 
 
// load the global index
bool GlobalIndex::loadIndex ()
 { // check if the gloal index already exists
   try
 	{ ofstream file;
 	  file.open (this->indexName, ifstream::in);
      file.close();
      // if empty then set size to 0
      if (file.fail())
       { this->size = 0;
       	 return false;
       }
      else
       { // if index exists, load it in memory
       	 ifstream file;
       	 file.open (this->indexName, ios::in);
       	 file>>this->size;
       	 this->index = new entry [this->size];
       	 for (int i=0; i<this->size; i++) 
       	  { string line;
       	  	file>>line;
       	  	this->index[i] = this->parseIndexLine (line);     
       	  }
       	 file.close ();
       	 return true;
       }	  
 	}
   catch (...)
    { this->size = 0;
      return false;
    }
 }
 
 
// load the idf strcuture
bool GlobalIndex::loadIdf()
 { // check if the idf index already exists
   try
 	{ ofstream file;
 	  file.open (this->idfName, ifstream::in);
      file.close();
      // if empty then set size to 0
      if (file.fail())
       { // load default 1 value if index already loaded
       	 if (this->size)
       	  { this->idf = new idfEntry [this->size];
       	    for (int i=0; i<this->size; i++)
       	     { this->idf[i].word = this->index[i].word;
       	       this->idf[i].idf  = 1;
       	     }
       	  }
       	 return false;
       }
      else
       { // if idf index exists, load it in memory
       	 ifstream file;
       	 file.open (this->idfName, ios::in);
       	 int n;
       	 file>>n;
       	 this->idf = new idfEntry [n];
       	 for (int i=0; i<n; i++) 
       	  { string line;
       	  	file>>line;
       	  	// parse the line
       	    idfEntry e;
       	    string separator = "|";
            vector<string> tokens;
            tokenize (line, tokens, separator);
            // get words
            this->idf[i].word = strtoul (tokens.at(0).c_str(),NULL,0);
            // get idf values
            this->idf[i].idf = strtod (tokens.at(1).c_str(),NULL);
            tokens.clear ();
       	  }
       	 file.close ();
       	 return true;
       }	  
 	}
   catch (...)
    { return false; }
 }

 
// add element
short GlobalIndex::addLocalIndex (entry* localIndex, int length)
 { // check if the index already exists or not
   if (this->size == 0)
 	{ this->size = length;
 	  this->index = new entry [this->size];
 	  for (int i=0; i<this->size; i++) this->index[i] = localIndex[i];
 	  return -1; 
 	}
   else
    { vector<int> newElements;
      // global index already exists
      int begin = 0;
      for (int i=0; i<length; i++)
       { // search the element in the index
       	 int found = search (localIndex[i].word, begin, this->size);
         if (found != -1)
       	  { // found the same features in the index - check recording
       	    for (int j=0; j<localIndex[i].positionList.size(); j++)
       	  	 { string id = localIndex[i].positionList.at(j).id;
       	  	   short ind = this->checkPresence(this->index[found],id);
       	  	   if (ind < 0)
       	  	    { // not already stored - add at the end
       	  	      this->index[found].positionList.push_back (localIndex[i].positionList.at(j));
       	  	    }
       	  	   else
       	  	    { // already stored - update content by replacing intervals
       	  	      this->index[found].positionList.at(ind).intervals.clear();
       	  	      this->index[found].positionList.at(ind).intervals = localIndex[i].positionList.at(j).intervals;
       	  	    }
       	  	 }
            begin = found;
       	  } 
       	 else
       	  newElements.push_back(i); 
       }
      // add the new features in the global index
      if (newElements.size() > 0)
       { entry* newIndex = new entry [this->size + newElements.size()];
         
         for (int j=0; j<this->size; j++)
       	   newIndex[j] = this->index[j];
             
      	 // add the new elements
         for (int i=0; i<newElements.size(); i++)
       	   newIndex[this->size+i] = localIndex[newElements.at(i)];
           
         // save new index
         if (this->index) delete [] this->index;
       	 this->index = newIndex;
         this->size += newElements.size(); 
         newElements.clear();
       }
      // sort the new global index
      sort (this->index, this->index+this->size, entry());
      return 0;	
    }    
 }
 
 
// save the index on the filesystem
bool GlobalIndex::save (entry* idx, int n)
 { try
    { ofstream file;
      file.open (this->indexName);
      file<<n<<"\n";
      for (int i=0; i<this->size; i++)
 	   { if (idx[i].size>0)
 	   	  { file<<idx[i].word<<"|";
 	   	    for (int j=0; j<idx[i].positionList.size()-1; j++)
 	   	     { file<<idx[i].positionList.at(j).id<<"{";
 	   	       for (int k=0; k<idx[i].positionList.at(j).intervals.size()-1; k++)
 	   	        file<<idx[i].positionList.at(j).intervals.at(k)<<"-";
 	   	        // last positions
 	   	        file<<idx[i].positionList.at(j).intervals.at(idx[i].positionList.at(j).intervals.size()-1)<<"},";
 	         }
 	        // last element
 	        file<<idx[i].positionList.at(idx[i].positionList.size()-1).id<<"{";
 	        for (int k=0; k<idx[i].positionList.at(idx[i].positionList.size()-1).intervals.size()-1; k++)
 	         file<<idx[i].positionList.at(idx[i].positionList.size()-1).intervals.at(k)<<"-";
 	        file<<idx[i].positionList.at(idx[i].positionList.size()-1).intervals.at(idx[i].positionList.at(idx[i].positionList.size()-1).intervals.size()-1)<<"}";	
 	        file<<"\n";
 	   	  }   	    
 	   }
	  file.close ();
	  return true;
    }
   catch (...)
    { return false; }	 
 }
 
 
// save the idf structure on the filesystem
bool GlobalIndex::saveIdf (int n)
 { try
    { ofstream file;
      file.open (this->idfName);
      file<<n<<"\n";
      for (int i=0; i<this->size; i++)
 	   { if (this->idf[i].idf != -1)
 	   	  file<<this->idf[i].word<<"|"<<this->idf[i].idf<<"\n";   	    
 	   }
	  file.close ();
	  return true;
    }
   catch (...)
    { return false; }
 } 
 
 
// cut top and back tail according a prefixed percentage on the average
int GlobalIndex::resize ()
 { this->idf = new idfEntry [this->size];
   // compute bounds using average percentage
   int maxWordDimension;
   int total = this->countTotalWordsPresence(maxWordDimension);
   float average = (float)total/(float)this->size;
   // upper bound on a small percentage of the top element
   int upBound   =  (int)ceil(maxWordDimension - (maxWordDimension*((float)UP_PERCENTAGE/100)));
   // lower bound on a percentage of the medium
   int downBound =  (int)ceil(average - (average*((float)DOWN_PERCENTAGE/100)));
   // cut index size
   int n = 0;
   for (int i=0; i<this->size; i++)
    { if ((this->index[i].size>upBound) || (this->index[i].size<=downBound))
       { this->index[i].size = -1;
       	 this->idf[i].idf = -1;
       }
      else 
       { n++;
       	 // compute idf values
       	 this->idf [i].word = this->index[i].word;
       	 this->idf[i].idf = log(((double)COLLECTION_SEGMENTS/this->index[i].size))/log(2);
       }
    }
   return n;
 }
     

// private functions


// binary search in the global index
int GlobalIndex::search (unsigned long key, int first, int last)
 { int ind = first;
   while ((ind<last) && (this->index[ind].word <= key))
    { if (this->index[ind].word == key) return ind;
      else ind++;
    }  
   return -1;
 }
 
 
// binary search in the global index
int GlobalIndex::binarySearch (unsigned long key, int first, int last)
 { if (first <= last) 
 	{ // compute mid point
      int mid = (first + last) / 2;
      if (key == this->index[mid].word) 
        return mid; 
      else 
       if (key < this->index[mid].word) 
        // call for the lower part of the array
        return binarySearch (key, first, mid-1);
       else
        // call for the upper part of the array
        return binarySearch (key, mid+1, last);
    }
   // not found
   return -1;  
 }
 
     
// parse a line of the index
entry GlobalIndex::parseIndexLine (string line)
 { entry e;
   position p;
   string separator = "|";
   vector<string> tokens;
   tokenize (line, tokens, separator);
   // get words
   e.word = strtoul (tokens.at(0).c_str(),NULL,0);
   // get file list
   string position = tokens.at(1);
   tokens.clear ();
   separator.clear ();
   separator = ",";
   tokenize (position, tokens, separator); 
   for (int i=0; i<tokens.size(); i++) 
    { int ind = tokens.at(i).find("{");
      // get the recording id
      p.id = tokens.at(i).substr (0,ind);
      // get the segments list
      string segments = tokens.at(i).substr (ind+1,tokens.at(i).size()-ind-2);
      string s = "-";
      vector<string> t;
      tokenize (segments, t, s);
      for (int j=0; j<t.size(); j++)
        p.intervals.push_back ((short)atoi(t.at(j).c_str()));
      e.positionList.push_back (p);
      p.intervals.clear();
    }  
   return e;
 }
 
 
// check presence of id in the entry e
short GlobalIndex::checkPresence (entry e, string id)
 { // check presence
   for (short i=0; i<e.positionList.size(); i++)
 	 if (e.positionList.at(i).id == id) return i;
   return -1;		
 }
 
 
// count the number of presence for each word
int GlobalIndex::countTotalWordsPresence (int& max)
 { int total = 0;
   max = 0;
   // list all the index
   for (int i=0; i<this->size; i++)
 	{ this->index[i].size = 0;
 	  for (int j=0; j<this->index[i].positionList.size(); j++)
 	    this->index[i].size += this->index[i].positionList[j].intervals.size();
 	  total += this->index[i].size;
 	  // check if more common entry
 	  if (this->index[i].size > max) 
 	    max = this->index[i].size;
 	}
   return total;
 }

 

 
