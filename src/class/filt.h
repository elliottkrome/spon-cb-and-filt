#ifndef SPON_FILT
#define SPON_FILT

#include "cb.h"
#include "tinyxml2.h"
#include <vector>
#include <sstream>

template<typename T>
class Filt : public Cb<T> {
 public:

  static const char* name();

  explicit Filt(
      size_t buffer_size_a,
      size_t max_mem_size_a);

  ~Filt();

  bool reset_mem();

  bool pull(
      T* array_a,
      size_t size_a);

  bool set_filter(
      const T* a,  // autoregressive (AR) filter coefficients
      size_t   p,  // number of AR coefficients
      const T* b,  // moving average (MA) filter coefficients
      size_t   q); // number of MA coefficients -in

  bool from_xml(tinyxml2::XMLElement* elem);
  bool array_from_xml_pchar(
      T* array_to_fill_a,
      const char* array_to_parse_a);
 private:
  long filter_ptr_d;
  long max_mem_d;
  long n_MA_coefs_d;
  long n_AR_coefs_d;
  T* AR_coefs_d;
  T* mem_d;
  T* MA_coefs_d;
};

template<typename T>
Filt<T>::Filt(
    size_t buffer_size_a,
    size_t max_mem_a) :
  Cb<T>(buffer_size_a),
  max_mem_d(max_mem_a),
  filter_ptr_d(0) {

  mem_d      = new T[max_mem_d];
  AR_coefs_d = new T[max_mem_d];
  MA_coefs_d = new T[max_mem_d];

  // zero out memory
  //
  for (int i = 0; i < max_mem_d; i++) {
    mem_d[i] = 0;
  }
}
template<typename T>
Filt<T>::~Filt() {
  delete [] mem_d;
  delete [] AR_coefs_d;
  delete [] MA_coefs_d;
}

template<typename T>
bool Filt<T>::pull(
    T*     array_a,    // out: filtered output
    size_t size_a)  {  // in:  number of samples to filter

  long tmp_ptr;     // keeps filter ptr wrapped in circBuff during loops
  long n = 0;       // to loop over number of samples to process
  long i = 0;       // to loop over number of MA coefficients 
  long j = 0;       // to loop over number of AR coefficients
  T left_hand_acc;  // to accumulate (MA . mem_d) 
  T right_hand_acc; // to accumulate (AR . mem_d)

  // perform sample processing loop size_a times
  //  n used to index values placed in output array_a upon one loop completion
  //
  for (n = 0; n < size_a; n++ ) {

    // if this->wptr has caught up to this->rptr, there is insufficient data
    //
    if (this->wptr == this->rptr)
      return false;

    // advance pointer, wrapping around if necessary
    //
    this->wptr = (this->wptr + 1) % this->bsize;

    // advance mem_d pointer, wrapping around if necessary
    //
    filter_ptr_d = (filter_ptr_d + 1) % max_mem_d;

    // summer setup:
    //  place next input in left_hand_acc (multiplied by the appropriate coef)
    //  reset right_hand_acc to 0
    //
    left_hand_acc = AR_coefs_d[0] * this->buffer[this->wptr];
    right_hand_acc = 0;

    // accumulate filter's memory vector dotted with AR coefficient vector
    //
    for (j = 1; j < n_AR_coefs_d; j++) {

      // update tmp_ptr with j value
      //  note that j looks "backward" in memory, hence the '-' operation
      //
      tmp_ptr = (max_mem_d + filter_ptr_d - j) % max_mem_d;

      // perform accumulation
      // 
      left_hand_acc = left_hand_acc + (AR_coefs_d[j] * mem_d[tmp_ptr]);
    }

    // store new memory in mem_d
    //
    mem_d[filter_ptr_d] = left_hand_acc;
   
    // accumulate filter's memory vector dotted with MA coefficient vector
    //
    for(i = 0; i < n_MA_coefs_d; i++) {

      // update tmp_ptr with i value
      //  note that i looks "backward" in memory, hence the '-' operation
      //
      tmp_ptr = (max_mem_d + filter_ptr_d - i) % max_mem_d;

      // perform accumulation
      //
      right_hand_acc = right_hand_acc + (MA_coefs_d[i] * mem_d[tmp_ptr]);
    }
    
    // place one processed sample in output buffer
    //
    array_a[n] = right_hand_acc;
  }
  return true;
}

template<typename T>
bool Filt<T>::set_filter(
    const T* a,   // autoregressive (AR) filter coefficients
    size_t   p,   // number of AR coefficients
    const T* b,   // moving average (MA) filter coefficients
    size_t   q) { // number of MA coefficients

  if (p > max_mem_d || q > max_mem_d) {
    return false;
  }

  n_AR_coefs_d = p;
  n_MA_coefs_d = q; 

  for (long i = 0; i < n_AR_coefs_d; i++) {
    AR_coefs_d[i] = a[i];
  }
  for (long i = 0; i < n_MA_coefs_d; i++) {
    MA_coefs_d[i] = b[i];
  }
  return true;
}

template<typename T>
bool Filt<T>::reset_mem() {
  for (long i = 0; i < max_mem_d; i++) {
    mem_d = 0;
  }
  return true;
}

#include <iostream>
template<typename T>
bool Filt<T>::from_xml(tinyxml2::XMLElement* elem_a) {
  const char* n_ar = elem_a->Attribute("numAR");
  const char* n_ma = elem_a->Attribute("numMA");
  const char* AR = elem_a->FirstChildElement("AR")->GetText();
  const char* MA = elem_a->FirstChildElement("MA")->GetText();

  if (n_ar == NULL ||
      n_ma == NULL ||
      AR   == NULL ||
      MA   == NULL) {
    return false;
  }

  array_from_xml_pchar(AR_coefs_d, AR) &&
  array_from_xml_pchar(MA_coefs_d, MA);

  for (int i = 0; i < 2; i++) {
    std::cout << MA_coefs_d[i] << std::endl;
  }
}
// TODO: all of this is sloppy
// where should it go?
template<typename T>
bool Filt<T>::array_from_xml_pchar(
    T* array_to_fill_a,
    const char* array_to_parse_a) {
  double temp_val;
  unsigned index = 0;
  std::istringstream ss(array_to_parse_a);
  while(ss >> temp_val && index < max_mem_d) {
    array_to_fill_a[index] = temp_val;
    index++;
  }
}

#endif  // SPON_FILT
