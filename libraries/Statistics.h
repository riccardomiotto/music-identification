/***		
	compute statistical moments
***/

class Statistics 
 { 
	private:
		double* array;            
     	long n;                  
     	double mean, med, var, skew, kurt;  
     	// variable status
     	bool meanCalculated;  
     	bool varCalculated;   
     	bool sorted;          
   
   public:
     // constructor and destructor
     Statistics (double* inputArray, long dim);
     ~Statistics();

     // get statistical moments
     double getMean ();
     double getMedian (); 
     double getMode ();
     double getVariance();
     double getSkewness (); 
     double getKurtosis ();
     
     // compute statistical moments
     void setMean ();
     void setMedian ();
     void setVariance ();
     void setSkewness();
     void setKurtosis();
     
     // compute all moments
     void setStatisticalMoments ();

  };

