/***
	handle the has index
***/

#include "Libraries.h"

// constructors
Index::Index ()
 { indexName = new char [MAX_PATH];
   strcpy (indexName, ""); 
 }

 
Index::Index (char* name)
 { indexName = new char [MAX_PATH];
   strcpy (this->indexName, name);  
 }


// destructor 
Index::~Index (void)
 { if (indexName)
     delete indexName;
 }

 
// retrieving values
char* Index::getIndexName ()
 { return indexName; }

 	 
// setting values
void Index::setIndexName (char* name)
 { strcpy (indexName, name); }

 
// creation of the index
bool Index::create ()
 { this->create (); }

 
// deletion of the index
bool Index::clear ()
 { this->clear (); }

     
// adding an element to the index
bool Index::addElement (char* filename, char* title)
 { this->addElement (filename, title); }


// retrieval of the query
bool Index::retrieve ()
 { this->retrieve (); }
