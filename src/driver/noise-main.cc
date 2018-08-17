#include "noise.h"

int main(int argc, char** argv) {

  FILE* out_file = fopen("examples/outputs/gaussian-noise.raw", "w");
  unsigned buf_size = 1000;
  double buf_out[buf_size];
  NoiseG<double> noise;
  unsigned nsamples = 16000;
  unsigned nframes = nsamples / buf_size;

  for(long i = 0; i < nframes; i++) {

    if (noise.pull(buf_out, buf_size)) {
      fwrite(buf_out, sizeof(double), buf_size, out_file);
    } 
  }
}
