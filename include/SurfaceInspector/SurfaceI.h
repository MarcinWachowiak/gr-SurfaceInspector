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

#ifndef INCLUDED_SURFACEINSPECTOR_SURFACEI_H
#define INCLUDED_SURFACEINSPECTOR_SURFACEI_H

#ifdef ENABLE_PYTHON
#include <Python.h>
#endif

#include <boost/format.hpp>
#include <boost/circular_buffer.hpp>
#include <SurfaceInspector/api.h>
#include <gnuradio/sync_block.h>
#include <qapplication.h>
#include <qwtplot3d/qwt3d_surfaceplot.h>
#include <qwtplot3d/qwt3d_function.h>

namespace gr {
  namespace SurfaceInspector {

    /*!
     * \brief <+description of block+>
     * \ingroup SurfaceInspector
     *
     */
    class SURFACEINSPECTOR_API SurfaceI : virtual public gr::sync_block
    {
     public:
        typedef boost::shared_ptr<SurfaceI> sptr;
        
        /*!
        * \brief Return a shared_ptr to a new instance of SurfaceInspector::SurfaceI.
        *
        * To avoid accidental use of raw pointers, SurfaceInspector::SurfaceI's
        * constructor is in a private implementation
        * class. SurfaceInspector::SurfaceI::make is the public interface for
        * creating new instances.
        */
        static sptr make(float samp_rate, int vlen,int width, int height,int gwidth,int gheight,double maxz, int fps,char *xaxis,char* yaxis, char *zaxis,QWidget *parent=NULL);
        //virtual void set_rf_unit(int unit) = 0;
        //virtual void set_samp_rate_delay(double d_samp_rate_delay, int frame_size) = 0;
        //virtual void set_samp_rate_prop(double d_samp_rate_prop) = 0;
        //virtual void set_cfreq(float cfreq) = 0;

        #ifdef ENABLE_PYTHON
            virtual PyObject* pyqwidget() = 0;
        #else
            virtual void* pyqwidget() = 0;
        #endif
    };

  } // namespace SurfaceInspector
} // namespace gr

#endif /* INCLUDED_SURFACEINSPECTOR_SURFACEI_H */

