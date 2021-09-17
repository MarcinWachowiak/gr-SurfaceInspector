/* -*- c++ -*- */
/*
 * Copyright 2021 Dobler.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_SURFACEINSPECTOR_SURFACEI_IMPL_H
#define INCLUDED_SURFACEINSPECTOR_SURFACEI_IMPL_H

#include <SurfaceInspector/SurfaceI.h>
#include <SurfaceInspector/inspector_form.h>
#include <boost/format.hpp>
#include <chrono>
#include <boost/circular_buffer.hpp>
#include <math.h>
#include <qapplication.h>
#include <qwtplot3d/qwt3d_surfaceplot.h>
#include <qwtplot3d/qwt3d_function.h>
#include "boost/multi_array.hpp"
#include <cassert>


using namespace std::chrono;

namespace gr {
  namespace SurfaceInspector {

    class SurfaceI_impl : public SurfaceI
    {
        private:
            QApplication *d_qApplication;
            int width, height;

            double maxz;

            int fcount;

            int fps;
            bool allocation_necessary=true;
            bool data_ready=false;
            int updateTime;


            //unsigned int rows; 
            //unsigned int columns;

            double** buffer2D;
            milliseconds oldms;
            std::vector<std::vector<double>> vector_buffer2D;

        public:
            SurfaceI_impl(float samp_rate, int vlen,int width, int height,int gwidth,int gheight,double maxz, int fps,char *xaxis,char* yaxis, char *zaxis,QWidget *parent);
            ~SurfaceI_impl();

            inspector_form *d_main_gui;
            // Where all the action really happens
            int work(int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items);

            #ifdef ENABLE_PYTHON
                PyObject* pyqwidget();
            #else
                void* pyqwidget();
            #endif

    };

  } // namespace SurfaceInspector
} // namespace gr

#endif /* INCLUDED_SURFACEINSPECTOR_SURFACEI_IMPL_H */

