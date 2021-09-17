/* -*- c++ -*- */
/*
 * Copyright 2019 Free Software Foundation, Inc.
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



#include <SurfaceInspector/inspector_form.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cmath>
#include <qwt_transform.h>
#include <pmt/pmt.h>
#include <QApplication>
#include <QtWidgets>
#include <gnuradio/io_signature.h>
#include <qwt_plot_canvas.h>
#include <vector>

namespace Qwt3D
{

    void Plot::calculateHull()
    {
        if (actualData_p->empty())
            return;

        ParallelEpiped hull(Triple(DBL_MAX,DBL_MAX,DBL_MAX),Triple(-DBL_MAX,-DBL_MAX,-DBL_MAX));

        hull.minVertex.x = minx;
        hull.maxVertex.x = maxx;

        hull.minVertex.y = miny;
        hull.maxVertex.y = maxy;

        hull.minVertex.z = 0;
        hull.maxVertex.z = maxz;

        setHull(hull);
    }

    Plot::Plot()
    {
        setTitle("Channel Impulse Response");
        for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
        {
            coordinates()->axes[i].setMajors(0);
            coordinates()->axes[i].setMinors(0);
            coordinates()->axes[i].setAutoScale(true);
        }

        setShading(FLAT);
        setPlotStyle(FILLED);
        setCoordinateStyle(BOX); 
 
        setRotation(15,0,15);
        setScale(1.0,1.0,1.0);
        setZoom(1.0);

        updateData();
        updateGL();
    }
}


namespace gr
{
    namespace SurfaceInspector
    {

        inspector_form::inspector_form(QWidget *parent, int width, int height,int gwidth,int gheight,char *xaxis, char *yaxis, char *zaxis) : QMainWindow(parent)
        {
            plot = new Qwt3D::Plot() ;
            this->width = width;
            this->height = height;
            this->gwidth =gwidth;
            this->gheight = gheight;

            this->xaxis = xaxis;
            this->yaxis = yaxis;    
            this->zaxis = zaxis;
            
            unsigned int rows =  height;
            unsigned int columns = width;
            //double** d_data = new double[rows][columns];

            QGridLayout *gridLayout = new QGridLayout;
            gridLayout->addWidget(plot,0,0,2,2);

            QWidget *window = new QWidget();
            window->setMinimumSize(400, 300);
            window->setLayout(gridLayout);

            setCentralWidget(window); 
            
        }

        inspector_form::~inspector_form()
        {
            // unsigned int rows = height; 
            // for(int x=0; x<rows; x++){
            //     delete d_data[x];
            // }
            // delete[] d_data;
        }

        void inspector_form::update( std::vector<std::vector<double>> buffer2D,float maxz)//double**
        {
            unsigned int rows =  height;
            unsigned int columns = width;
            double minx=0.0,maxx=gwidth,miny=0,maxy=gheight;
            std::vector<double*> ptrs;
            


            for (auto& vec : buffer2D) {
                //   ^ very important to avoid `vec` being
                // a temporary copy of a `thing` element.
                ptrs.push_back(vec.data());
            }

            plot->minx = minx;
            plot->maxx = maxx;

            plot->miny = miny;
            plot->maxy = maxy;


            // if (maxz == 0) {
            //     for (int y = 0; y < columns ; y++) {
            //         for (int x = 0; x < rows ; x++) {
            //             if (buffer2D[y][x] > maxz) {
            //                 maxz = buffer2D[y][x];
            //             } 
        
            //         }
            //     }
            // }

            plot->maxz = maxz;
            //std::cout << "Trying to load from data\r\n" << std::endl;
            plot->loadFromData  (   ptrs.data(),
                                    rows,
                                    columns,
                                    minx,
                                    maxx,
                                    miny,
                                    maxy
                                );

            plot->coordinates()->axes[Qwt3D::X1].setLabel(true);
            plot->coordinates()->axes[Qwt3D::X1].setLabelString("Time");

            plot->coordinates()->axes[Qwt3D::Y1].setLabel(true);
            plot->coordinates()->axes[Qwt3D::Y1].setLabelString("Delay");

            plot->coordinates()->axes[Qwt3D::Z1].setLabel(true);
            plot->coordinates()->axes[Qwt3D::Z1].setLabelString("Crosscorrelation magnitude");

            plot->updateData();
            plot->updateGL();

            plot->repaint();
            repaint();
        }


        void inspector_form::customEvent(QEvent * event)
        {
            // When we get here, we've crossed the thread boundary and are now
            // executing in the Qt object's thread

            if(event->type() == UPDATE_EVENT) {
                handleUpdateEvent(static_cast<UpdateEvent *>(event));
            } else if(event->type() == ROT_EVENT) {
                handleRotEvent(static_cast<RotEvent *>(event));
            }

        }

        void inspector_form::handleUpdateEvent(const UpdateEvent *event)
        {
            update(event->getData(),event->getZaxis());
        }

        void inspector_form::handleRotEvent(const RotEvent *event)
        {
            if (event->getRotation() == true) {
                plot->setRotation(15,0,15);
                plot->setScale(1.0,1.0,1.0);
                plot->setZoom(1.0);
            } else {
                plot->setRotation(85,0,0);
                plot->setScale(1.0,1.0,1.0);
                plot->setZoom(2.0);
            }
        }

    }

}
