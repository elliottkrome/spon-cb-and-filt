#include <random>
#include <chrono>

template<typename T>
class NoiseG {
 public:

  static const char* name();

  NoiseG();

  bool pull(
      T* array_a,
      size_t size_a);

 private:
  double mean_d;
  double var_d;
  std::default_random_engine engine_d;
  std::normal_distribution<T> dist_d;
};

template<typename T>
NoiseG<T>::NoiseG() {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  engine_d = std::default_random_engine(seed);
  dist_d = std::normal_distribution<T>(0, 1); 
}

template<typename T>
bool NoiseG<T>::pull(
    T* array_a, 
    size_t size_a) {
 
  for (long i = 0; i < size_a; i++) {
    array_a[i] = dist_d(engine_d);
  }
  return true;
}

template<typename T>
const char* NoiseG<T>::name() {
  return "NoiseG";
}
