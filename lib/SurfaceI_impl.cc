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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "SurfaceI_impl.h"
#include <chrono>
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <QFile>
#include <iostream>
#include <QGuiApplication>
#include <QApplication>
#include <boost/format.hpp>
#include <boost/circular_buffer.hpp>
#include <QMainWindow>
#include <volk/volk.h>
#include <vector>

using namespace std::chrono;

namespace gr {
  namespace SurfaceInspector {

    SurfaceI::sptr SurfaceI::make(float samp_rate, int vlen,int width, int height,int gwidth,int gheight,double maxz,int fps,char*xaxis, char*yaxis, char *zaxis, QWidget *parent)
    {
      return gnuradio::get_initial_sptr (new SurfaceI_impl(samp_rate, vlen,width,height,gwidth,gheight,maxz,fps,xaxis,yaxis,zaxis,parent));
    }


    /*
     * The private constructor
     */
    SurfaceI_impl::SurfaceI_impl(float samp_rate, int vlen,int width, int height,int gwidth,int gheight,double maxz, int fps,char *xaxis,char* yaxis, char *zaxis,QWidget *parent)
      : gr::sync_block("SurfaceI",
              gr::io_signature::make(1, 1, sizeof(float)*vlen),
              gr::io_signature::make(0,0,0))
    {
            int updateTime = (int) width*height *(double) 1000/samp_rate;
            
            this->updateTime = updateTime;
            this->width = width;
            this->height = height;
            this->maxz = maxz;
            this->fps = fps;
            bool allocation_necessary = true;
            bool data_ready = false;
            //this->rows = height;
            //this->columns = width;
    

            if(qApp != NULL)
            {
                d_qApplication = qApp;
            }
            else
            {

                #if QT_VERSION >= 0x040500 && QT_VERSION < 0x050000
                                std::string style = prefs::singleton()->get_string("qtgui", "style", "raster");
                                QApplication::setGraphicsSystem(QString(style.c_str()));
                #endif

                char *d_argv;

                int d_argc = 1;
                d_argv = new char;
                d_argv[0] = '\0';

                if (d_qApplication == NULL)
                    d_qApplication = new QApplication(d_argc, &d_argv);

            }

            std::cout << "Starting inspector_form.\r\n" << std::endl;
            d_main_gui = new inspector_form(parent,width,height,gwidth,gheight,xaxis,yaxis,zaxis);
            std::cout << "Exited inspector_form.\r\n" << std::endl;

            std::cout << "Try to show plot" << std::endl;
            
            d_main_gui->show();
            std::cout << "Plot successfully shown.\r\n" << std::endl;

            /*Allocation of data and release problem solve!*/
            unsigned int rows = height; 
            unsigned int columns = width;
            std::cout << "Allocation of 2D buffer.\r\n" << std::endl;
            double** buffer2D = new double*[rows];
            for(int i = 0; i < rows; ++i)
               buffer2D[i] = new double[columns];
            std::cout << "Allocation of 2D buffer successfull.\r\n" << std::endl;

            std::vector<std::vector<double>> vector_buffer2D(rows, std::vector<double>(columns, 0));


            std::cout << "The size of vector_buffer2D in constructor is " << vector_buffer2D.size() << std::endl;
    }

    /*
     * Our virtual destructor.
     */
    SurfaceI_impl::~SurfaceI_impl()
    {
        //for(int i=0; i<rows; i++)
        //    volk_free((void*)buffer2D[i]);
        //volk_free((void*)buffer2D);
        std::cout << "Entering delete for loop\r\n" << std::endl;
        unsigned int rows = height; 
        for(int x=0; x<rows; x++)
        {
            delete[] buffer2D[x];
            //std::vector<std::vector<double>>().swap(vector_buffer2D[rows]);
        }
        delete buffer2D;
        std::vector<std::vector<double>>().swap(vector_buffer2D);
        std::cout << "Exiting delete for loop\r\n" << std::endl;
    }


    


    #ifdef ENABLE_PYTHON
        PyObject*
        SurfaceI_impl::pyqwidget()
        {
            PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
            PyObject *retarg = Py_BuildValue("N", w);
            return retarg;
        }
    #else
            void *
            SurfaceI_impl::pyqwidget()
            {
                return NULL;
            }
    #endif

    int SurfaceI_impl::work(int noutput_items,
                                           gr_vector_const_void_star& input_items,
                                           gr_vector_void_star& output_items)
    {
        const float *in = (const float *) input_items[0];

        unsigned int rows = height; 
        unsigned int columns = width;

        if(allocation_necessary){
            vector_buffer2D.resize(rows);
            for(int x=0; x<rows; x++){
                vector_buffer2D[x].resize(columns);
            }
        }

        //std::cout << "Starting storing of data...\r\n" << std::endl;
        //std::cout << "The size of buffer2D is " << vector_buffer2D.size() << std::endl;
        // Assign values to the elements
        int z = 0.0;
        for(int x=0; x<rows; ++x)
        {
            for(int y=0; y<columns; ++y)
            {
                //std::cout << "The value of in is " << in[z] << std::endl;
                vector_buffer2D[x][y] = in[z];
                //std::cout << "The value of index z is " << z << std::endl;
                z++;

            }
        }

            
        //std::cout << "Storing of data successful.\r\n" << std::endl;

        /* TO-DO: Configure repetition rate according to Nyquist for Doppler-frequency */
        milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());

        //std::cout << "The updateTime is set to " << updateTime << std::endl;
        if (ms > oldms+std::chrono::milliseconds(1000)){
            fcount = 0;
            oldms = ms;
        }

        if (fcount < fps){
            //std::cout << "Entering Update Procedure\r\n" << std::endl;
            QCoreApplication::postEvent(d_main_gui,new UpdateEvent(vector_buffer2D, this->maxz));
            // for(int x=0; x<rows;++x){
            //     for(int y=0; y<columns;++y){
            //         std::cout << "The value of buffer 2D at position " << x << " " << y << " is " << vector_buffer2D[x][y] << std::endl;
            //     }
            // }
            //std::cout << "Exiting Update Procedure\r\n" << std::endl;
            //allocation_necessary = true;

            // std::cout << "Trying to free memory\r\n" << std::endl;
            // for(int i = 0; i < rows; ++i)
            //     delete[] buffer2D[i];
            // delete[] buffer2D;
            // std::cout << "Freeing memory successful\r\n" << std::endl;
        }
        
        fcount++;

        return noutput_items;
    }


  } /* namespace SurfaceInspector */
} /* namespace gr */

