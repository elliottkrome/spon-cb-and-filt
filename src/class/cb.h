#include <iostream>






#ifndef SPON_CB
#define SPON_CB
#include <stdio.h>

template<typename T>
class Cb {
 public:

  static const char* name();

  explicit Cb(size_t size_a);

  ~Cb();

  virtual bool push(
      const T* array_a,
      size_t size_a);

  virtual bool pull(
      T* array_a,
      size_t size_a);

  // protected:
  size_t bsize;
  T* buffer;

  // read and write pointers:
  //  rptr: next location where new data can be stored     (in)
  //  wptr: next location where data to be written resides (out)
  //
  size_t rptr;
  size_t wptr;
  void debug();
};

template<typename T>
Cb<T>::Cb(size_t size_a) {
  
  bsize = size_a;        // store the buffer size
  buffer = new T[bsize]; // create a buffer to store data

  // initialize the read and write pointers
  //
  rptr = 0;         // set the read pointer to the beginning of the buffer
  wptr = bsize - 1; // set the write pointer to the end of the buffer
}

template<typename T>
Cb<T>::~Cb() {
  delete[] buffer;
}

template<typename T>
bool Cb<T>::push(const T* array_a, size_t size_a){

  // loop over the input data
  //
  for (long i = 0; i < size_a; i++) {

    // check for room on the circular buffer
    // if it is final iteration, we are good to go, else return false
    //  as we are trying to read into an index where there is data
    //  that hasn't been processed yet
    //
    if (rptr == wptr) {

      // if we have done a full loop and filled the buffer, then we can do more
      //
      if (i == size_a - 1) {
	return true;
      }

      // if rptr has caught up with wptr and we haven't done a full loop, we
      //  indicate this to the caller by returning false
      //
      else {
	return false;
      }
    }
    //
    // end of room check

    // if there is room left for more data to be read in, then do so
    //  and advance the pointer, wrapping at end of memory
    //
    else {
      buffer[rptr] = array_a[i];
      rptr = (rptr + 1) % bsize;
    }
    //
    // end of individual read
 
  }
  return true;
}

// This pulls data from the circular buffer to the output array.
// If there is insufficient data available, it returns false.
//
template<typename T>
bool Cb<T>::pull(T* array_a, size_t size_a) {
 
  // loop over the data and output
  //
  for (long i = 0; i < size_a; i++) {

    // if wptr has caught up to rptr, there is insufficient data
    //
    if (wptr == rptr)
      return false;

    wptr = (wptr + 1) % bsize;
    array_a[i] = buffer[wptr];
  }
  return true;
}

template<typename T>
const char* Cb<T>::name() {
  return "Cb";
}
/*
template<typename T>
void Cb<T>::debug() {
  std::cerr << "<---" << this->name() << " %s --->" << std::endl;
  std::cerr << " read pointer = " << rptr << " write pointer = " << wptr << std::endl;
  std::cerr << " buffer[] = ";
  for (long i = 0; i < bsize; i++) {
    std::cerr << buffer[i];
  } std::cerr << std::endl;
  std::cerr << "<--- done --->\n" << std::endl;
}
*/

#endif  // SPON_CB
