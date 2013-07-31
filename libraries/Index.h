/***
	handle the information of an hash index
***/

#include <string.h>


class Index
 { 
	protected:
 	 	// name of the index
 	 	char* indexName;     
 	
     	// creation of the index
     	virtual bool create ();
     
     	// deletion of the index
     	virtual bool clear ();
     
     	// adding an element
     	virtual bool addElement (char* filename, char* title);
     
     	// retrieval of the query
     	virtual bool retrieve ();
     
   	public:
 	 	// constructors and destructors
 	 	Index ();
 	 	Index (char* name);
 	 	~Index (void);
 	 
 	 	// retrieving values
 	 	char* getIndexName ();
 	 
 	 	// setting values
 	 	void setIndexName (char* name);
 	  	 
  };
