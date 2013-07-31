/***
	statistical moments
***/

#include "Libraries.h"


// constructor
Statistics::Statistics (double* inputArray, long dim) 
 { // data array
   if (dim) 
    { this->array = new double [dim];
      this->n = dim;
      for (int i=0; i<this->n; i++) 
        this->array[i] = inputArray[i];    
    } 
   else 
    { this->array = NULL;
      this->n = 0;
    }
   // statistical moments 
   mean = 0;
   med = 0;
   var = 0;
   skew = 0;
   kurt = 0;
   meanCalculated = false;
   varCalculated = false;
   sorted = false;
}


// destructor
Statistics::~Statistics () 
 { this->n=0;
   delete[] this->array;
 }


// get statistical moments
double Statistics::getMean ()
 { return this->mean; }

 
double Statistics::getMedian ()
 { return this->med; }

     
double Statistics::getVariance()
 { return this->var; }

     
double Statistics::getSkewness ()
 { return this->skew; }

     
double Statistics::getKurtosis ()
 { return this->kurt; }

 
//	calculate mean
void Statistics::setMean () 
 { for (int i=0; i<n; i++) 
     mean += array[i];
   mean /= double(n);
   // set status mean 
   meanCalculated = true;
 }


//	calculate median 
void Statistics::setMedian () 
 { if (!sorted) 
    { std::sort (array, array+n);
      // set sort status
      sorted=true;
    }
   // if odd number of elements, median is in the middle of the sorted array
   if (n%2==1) 
     med = array[n/2];
   // if even number of elements, median is avg of two elements in the middle 
   else 
     med = (array[n/2] + array[(n/2)-1])/2;   
 }


//	calculate variance
void Statistics::setVariance () 
 { // check mean
   if (!meanCalculated) this->setMean ();
   // variance
   for (int i=0; i<n; i++) 
     var += pow((array[i]-mean), 2.0);
   var /= (double)n;   
   // set status variance
   varCalculated = true;
 }


//	calculate skewness
void Statistics::setSkewness() 
 { // check mean
   if (!meanCalculated) this->setMean ();
   // check variance
   if (!varCalculated) this->setVariance ();
   // skewness
   for (int i=0; i<n; i++) 
     skew += (array[i]-mean)*(array[i]-mean)*(array[i]-mean);
   skew *= sqrt (double(n))/pow(var*double(n), 1.5);
 }


//	calculate kurtosis
void Statistics::setKurtosis () 
 { // check mean
   if (!meanCalculated) this->setMean ();
   // check variance
   if (!varCalculated) this->setVariance ();
   // kurtosis 
   for (int i=0; i<n; i++) 
     kurt += pow((array[i]-mean), 4.0);
   kurt /= double(n)*pow(var, 2.0); 
 }


//  calculate all statistics
void Statistics::setStatisticalMoments () 
 { this->setMean ();
   this->setMedian ();
   this->setVariance ();
   this->setSkewness ();
   this->setKurtosis ();
 }