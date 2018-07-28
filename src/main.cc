#define STR_STDIN  "stdin"
#define STR_STDOUT "stdout"
#include "Filt.h"
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char** argv) {

  // variables to store parsed cmdl arguments in
  //
  bool help_flag, unspecified_io;
  double sample_freq, frame_dur, win_dur;
  std::string in_file_str, out_file_str, coefs_file;

  po::options_description po_general("General options");
  po::options_description po_all("Interface");
  po::options_description po_parameters("Parameters");
  po::options_description po_io("I/O:    (must specify)");

  po_general.add_options()
    ("help,h",     "produce this help message and exit");
  po_io.add_options()
    ("in-file,i",  po::value<std::string>(&in_file_str)->default_value(""), "file to process")
    ("out-file,o", po::value<std::string>(&out_file_str)->default_value(""), "file to output");    
  po_parameters.add_options()
    ("samp-freq,s",  po::value<double>(&sample_freq)->default_value(8000), "sampling frequency (samps / secs)")
    ("frame-dur,f",  po::value<double>(&frame_dur)->default_value(0.02, "0.02"),   "frame duration     (secs)")
    ("window-dur,w", po::value<double>(&win_dur)->default_value(0.04, "0.04"),     "window duration    (secs)")
    ("coefs-file,c", po::value<std::string>(&coefs_file)->default_value("none"), "filter coefficient file");    

  po_all.add(po_general).add(po_parameters).add(po_io);

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, po_all), vm);

  po::notify(vm);

  if (unspecified_io = (in_file_str.empty() || out_file_str.empty())) {
    std::cout << "\n unspecified input and / or output. \n\n";
  }
  if (vm.count("help") || unspecified_io) {
    std::cout << po_all;
    exit(0);
  }

  FILE* in_file = fopen(in_file_str.c_str(), "r");
  FILE* out_file = fopen(out_file_str.c_str(), "w");

  long M = frame_dur * sample_freq; // number of samples in window
  long N = win_dur   * sample_freq; // number of samples in frame

  double buf_in[M];   // define a buffer to hold the data from data in 
  double buf_out[M];  // define a buffer to hold the output data 

  long nsamples_read, nsamples_write, n = 0;

  // create a circular buffer
  //
  Filt<double> filter(N, N);  
  double MA_coefs[10] = {1, 0.5, 0, 0, 0, 0, 0, 0, 0, 0};
  double AR_coefs[10] = {-1, -0.1, 0, 0, 0, 0, 0, 0, 0, 0};
  filter.set_filter(AR_coefs, 10, MA_coefs, 10);

  while ((nsamples_read = fread(buf_in, sizeof(double), M, in_file)) > 0) {

    filter.push(buf_in, nsamples_read);
    
    if (filter.pull(buf_out, M)) {
      nsamples_write = fwrite(buf_out, sizeof(double), M, out_file);
    } 
  }

  // flush
  //
  filter.pull(buf_out, nsamples_read);
  fwrite(buf_out, sizeof(double), nsamples_read, out_file);
  return true;
}
