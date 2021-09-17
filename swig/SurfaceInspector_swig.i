/* -*- c++ -*- */

#define SURFACEINSPECTOR_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "SurfaceInspector_swig_doc.i"

%include "std_map.i"

namespace std {
  %template(map_float_vector) map<float, vector<float> >;
};

%{
#include "SurfaceInspector/SurfaceI.h"
%}

%include "SurfaceInspector/SurfaceI.h"
GR_SWIG_BLOCK_MAGIC2(SurfaceInspector, SurfaceI);
