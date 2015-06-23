//
// Copyright 2014 CADET Bioinformatics Laboratory
//                University of Manchester, UK
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//



#ifndef FL_VIEWER_H
#define FL_VIEWER_H

#include <iostream>
#include <vector>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>  
#include <Eigen/Dense>
#include <boost/filesystem.hpp>
#include <omp.h>

#include <FL/Fl.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_draw.H>
#include <FL/Fl_File_Chooser.H>

#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>

#define NOMINMAX
#include <windows.h> 
#include <process.h>

#include "FL_vizGUI.h"

using namespace std;


#define NOTHREAD 0
#define FPS 10 // frames per second: change, as needed 

CRITICAL_SECTION g_cs; 

myOverlay *main_GUI; // the main GUI window, myOverlay class is defined in FL_vizGUI.h

int img_w; // width of the raw image
int img_h; // heigh of the raw image
float rt_min = 0 ; // the minimum rt in the dataset
float rt_max = 1;  // the maximum rt in the dataset
float mz_min = 0;  // the minimum mz in the dataset
float mz_max = 1;  // the maximum mz in the dataset
float rt_min_d = 0 ; // the minimum rt displayed in box_main
float rt_max_d = 1;  // the maximum rt displayed in box_main
float mz_min_d = 0;  // the minimum mz displayed in box_main
float mz_max_d = 1;  // the maximum mz displayed in box_mainopen

ClrMap COLOR_TABLE = cubeHelix;

Reconstructer *recon;
SMVStreamer *stream;

int chunk_size = 10000;
string in_file;

int tLoaded = 0; // flag to present if a R-tree is loaded

// === thread control ===
int stopStream = 0; // flag to show if the current stream need to be stopped
unsigned dwThreadIdCur;
HANDLE  hThreadCur; 

typedef struct sImgDATA{
	string in_file;
	double rt_min;
	double rt_max;
	double mz_min;
	double mz_max;
	int chunk_size;
	int tLoaded;
}sImgDATA, *PsImgDATA;

unsigned __stdcall threadShowImg( void* lpParam )
{
	cout << endl << ">===>> Beginning of the threadShowImg function... ";
	// get the input data
	PsImgDATA pImgData;
	pImgData = (PsImgDATA)lpParam;
	
	const string _in_file = pImgData->in_file;
	double _rt_min = pImgData->rt_min;
	double _rt_max = pImgData->rt_max;
	double _mz_min = pImgData->mz_min;
	double _mz_max = pImgData->mz_max;
	int _chunk_size = pImgData->chunk_size;
	int _tLoaded = pImgData->tLoaded;

	stream->stream_cs_gui(_mz_min, _mz_max, _rt_min, _rt_max, _chunk_size, recon, main_GUI, _tLoaded);
		
	cout << endl << "			<<===< End of the threadShowImg function " << endl;
	return 0;
}


void imgThd(const string& _in_file, double _rt_min, double _rt_max, double _mz_min, double _mz_max, int _chunk_size, int _tLoaded)
{
	PsImgDATA pData;
	pData = (PsImgDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                sizeof(sImgDATA));
	if( pData == NULL )
        {
            ExitProcess(2);
        }
		
	// Generate data for thread to work with.
	pData -> in_file = _in_file;
	pData -> rt_min = _rt_min;
	pData -> rt_max = _rt_max;
	pData -> mz_min = _mz_min;
	pData -> mz_max = _mz_max;
	pData -> chunk_size = _chunk_size;
	pData -> tLoaded = _tLoaded;
	
	if (hThreadCur != NULL) 
	{
		//stop the executing stream thread
		stopStream = 1; 
		
		// check if current stream thread is terminated
		cout << endl << ">+++>> To terminate current decoding thread ... " << endl;
		double sS_time1 = omp_get_wtime();
		double time2;
		int printed = 0;
		for (;;)
		{
			DWORD result = WaitForSingleObject( hThreadCur, 0);

			if (printed == 0)
			{
			cout << endl << "++++++ Wait for the signal from the thread ++++++ " << endl;
			printed = 1;
			}

			time2 = omp_get_wtime();
 
			if (result == WAIT_OBJECT_0) 
			{
				// the thread handle is signalled - the thread has terminated
				DWORD exitcode;

				BOOL rc = GetExitCodeThread( hThreadCur, &exitcode);
				if (!rc) {
					// return is zero, handle error from GetExitCodeThread()...
				}
				else if (rc != STILL_ACTIVE)
				{
					// the thread has terminated
					stopStream = 0;
					hThreadCur = NULL;
					DeleteCriticalSection(&g_cs);
					break;
				}
			}
			else if (time2 - sS_time1 > 2.0)
			{
				TerminateThread(hThreadCur, 0);

				stopStream = 0;
				hThreadCur = NULL;
				DeleteCriticalSection(&g_cs);
				break;
			}
		}
		cout << endl << "	<<+++< Time spent to stop current decoding thread:" << omp_get_wtime() - sS_time1 << endl;
			
		InitializeCriticalSection(&g_cs);  

		// create a new stream thread
		hThreadCur = (HANDLE)_beginthreadex( NULL, 0, &threadShowImg, pData, 0, &dwThreadIdCur);

		if (hThreadCur == NULL) 
			{
			   ExitProcess(4);
			}
	}
	else
	{	
		// Create the stream thread 
		hThreadCur = (HANDLE)_beginthreadex( NULL, 0, &threadShowImg, pData, 0, &dwThreadIdCur);

		if (hThreadCur == NULL) 
			{
			   ExitProcess(4);
			}
	}
}


void show_img(const string& _in_file, double _rt_min, double _rt_max, double _mz_min, double _mz_max, int _chunk_size, int _tLoaded)
{
	cout << endl << ">==>=> Beginning of the show_img function... ";

	InitializeCriticalSection(&g_cs);  // for debug purpose only

	stream->stream_cs_gui(_mz_min, _mz_max, _rt_min, _rt_max, _chunk_size, recon, main_GUI, _tLoaded);
	
	cout << endl << "	<=<==< End of the show_img function ";
	return;
}


void sld_rt_cb(Fl_Widget *,void *) 
{
  	float rt_cur = main_GUI->sld_rt->value() * (rt_max_d - rt_min_d) + rt_min_d; 
	float mz_cur = main_GUI->sld_mz->value() * (mz_max_d - mz_min_d) + mz_min_d;
	char s[40];
	int cur_row, cur_col;
	float cur_intens;
	
	sprintf(s, "%.2f", rt_cur);
	main_GUI->txt_curTime->value(s);

	cur_row = (int)(main_GUI->sld_rt->value()* (main_GUI->imgData.rows()-1));
	cur_col = (int)(main_GUI->sld_mz->value()* (main_GUI->imgData.cols()-1));
	cur_intens = main_GUI->imgData(cur_row, cur_col);
	
	sprintf(s, "%.2f,  %.2f,  %.2f", rt_cur, mz_cur, cur_intens);
	main_GUI->opt_cross->value(s);

	main_GUI->redraw_overlay();
	main_GUI->box_spectro->redraw();
}


void sld_mz_cb(Fl_Widget *,void *) 
{
	float mz_cur = main_GUI->sld_mz->value() * (mz_max_d - mz_min_d) + mz_min_d;
	float rt_cur = main_GUI->sld_rt->value() * (rt_max_d - rt_min_d) + rt_min_d; 
	char s[40];
	int cur_row, cur_col;
	float cur_intens;
	
	sprintf(s, "%.2f", mz_cur);
	main_GUI->txt_curMZ->value(s);

	cur_row = (int)(main_GUI->sld_rt->value()* (main_GUI->imgData.rows()-1));
	cur_col = (int)(main_GUI->sld_mz->value()* (main_GUI->imgData.cols()-1));
	cur_intens = main_GUI->imgData(cur_row, cur_col);
	
	sprintf(s, "%.2f,  %.2f,  %.2f", rt_cur, mz_cur, cur_intens);
	main_GUI->opt_cross->value(s);
	
	main_GUI->redraw_overlay();
	main_GUI->box_chroma->redraw();
}


void set_label()
{
	char s[30];
	float mz2, mz3, mz4, mz5, rt2, rt3, rt4, rt5;

	sprintf(s, "%.2f,  %.2f,  %.2f", rt_min_d, mz_min_d, main_GUI->imgData(1, 1));
	main_GUI->opt_cross->value(s);
	 
	mz2 = mz_min_d + (mz_max_d - mz_min_d)/5;
	mz3 = mz_min_d + 2*(mz_max_d - mz_min_d)/5;
	mz4 = mz_min_d + 3*(mz_max_d - mz_min_d)/5;
	mz5 = mz_min_d + 4*(mz_max_d - mz_min_d)/5;
	
	rt2 = rt_min_d +(rt_max_d - rt_min_d)/5;
	rt3 = rt_min_d +2*(rt_max_d - rt_min_d)/5;
	rt4 = rt_min_d +3*(rt_max_d - rt_min_d)/5;
	rt5 = rt_min_d +4*(rt_max_d - rt_min_d)/5;
	
	sprintf(s, "%.2f", mz_min_d);
	main_GUI->txt_minMZ->value(s);
	
	sprintf(s, "%.2f", mz_max_d);
	main_GUI->txt_maxMZ->value(s);
	
	sprintf(s, "%.2f", mz2);
	main_GUI->txt_MZ2->value(s);	

	sprintf(s, "%.2f", mz3);
	main_GUI->txt_MZ3->value(s);
	
	sprintf(s, "%.2f", mz4);
	main_GUI->txt_MZ4->value(s);
	
	sprintf(s, "%.2f", mz5);
	main_GUI->txt_MZ5->value(s);
	
	sprintf(s, "%.2f", rt_min_d);
	main_GUI->txt_minRT->value(s);

	sprintf(s, "%.2f", rt_max_d);
	main_GUI->txt_maxRT->value(s);
	
	sprintf(s, "%.2f", rt2);
	main_GUI->txt_RT2->value(s);
	
	sprintf(s, "%.2f", rt3);
	main_GUI->txt_RT3->value(s);
	
	sprintf(s, "%.2f", rt4);
	main_GUI->txt_RT4->value(s);
	
	sprintf(s, "%.2f", rt5);
	main_GUI->txt_RT5->value(s);
}


void open_cb(Fl_Widget *,void *)
{
	// deactive the colormap selection
	main_GUI->m_clrmap->deactivate(); 
	main_GUI->main_menu->redraw();

	main_GUI->box_main->activate();
	main_GUI->box_nav->activate();

	main_GUI->box_main->redraw();
	main_GUI->box_nav->redraw();
	
	if (tLoaded) 
	{
		tLoaded = 0;
	}
	
	// Create the file chooser, and show it
    Fl_File_Chooser chooser(".",                           // directory
                            "*.idx",                       // filter
                            Fl_File_Chooser::SINGLE,       // chooser type
                            "Choose an idx file (.idx)");  // title
    chooser.show();

    // Block until user picks something.
    while(chooser.shown())
        { Fl::wait(); }

    // User hit cancel?
    if ( chooser.value() == NULL )
        { fprintf(stderr, "(User hit 'Cancel')\n"); return; }

	//string in_file;
	in_file = chooser.value();

    // Print what the user picked
    fprintf(stderr, "\n============================================================\n");
	fprintf(stderr, "The selected idx file: '%s'\n", in_file);

	string basename;
	int lastdot = in_file.find_last_of("."); 
	basename = (lastdot == string::npos) ? basename : in_file.substr(0, lastdot);

    cout << "Reading " << basename << ".txt" << endl;
	ostringstream oss; oss << basename << ".txt";
	ifstream ifs(oss.str().c_str());
	ifs >> mz_min >> mz_max >> rt_min >> rt_max; 

	mz_min_d = mz_min;
	mz_max_d = mz_max;
	rt_min_d = rt_min;
	rt_max_d = rt_max;

	set_label();
	
	boost::filesystem::path viz_path(in_file);
	boost::filesystem::path viz_dir = viz_path.parent_path();
	ostringstream oss2; oss2 << viz_dir.stem().string() << ".out";
	cout << oss2.str() << endl;
	
	if (recon != NULL)
		delete recon;
	if (stream != NULL)
		delete stream;
		
	recon = new Reconstructer(oss.str(), img_w, img_h);
	stream = new SMVStreamer(in_file);

	if (NOTHREAD)
		show_img(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
	else
		imgThd(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);

	tLoaded = 1;  // thumb overview is displayed
}



void draw_clrBar()
{
	// draw the color bar
	unsigned char * cMapPix;
	int cMap_w, cMap_h, cMap_d;
	cMap_w = main_GUI->box_cMap->w();
	cMap_h = main_GUI->box_cMap->h();
	cMap_d = 3;
	
	
	cMapPix = new unsigned char [cMap_h * cMap_w * cMap_d];
	COLOUR cTmp;
	for (int i = 0; i < cMap_w; i++)
	{
		cTmp = GetColour(i, 0, 255);

		for (int j = 0; j < cMap_h; j++)
		{
			int index = j * cMap_w * cMap_d + i * cMap_d;

			cMapPix[index] = int(cTmp.r*255.9999);
			cMapPix[index+1] = int(cTmp.g*255.9999);
			cMapPix[index+2] = int(cTmp.b*255.9999);
		}
	}


	Fl_RGB_Image  *  cMap_RGB_img; 
	cMap_RGB_img = new Fl_RGB_Image(cMapPix, cMap_w, cMap_h); //main_GUI->box_cMap->w(), main_GUI->box_cMap->h(), 3);
	main_GUI->box_cMap->image(cMap_RGB_img);
}



void cubh_cb(Fl_Widget *,void *)
{
	main_GUI->m_cubh->setonly();
	COLOR_TABLE = cubeHelix;
	draw_clrBar();
	main_GUI->box_cMap->redraw();
}


void jet_cb(Fl_Widget *,void *)
{
	main_GUI->m_jet->setonly();
	COLOR_TABLE = matlabJet;
	draw_clrBar();
	main_GUI->box_cMap->redraw();
}


void s_jet_cb(Fl_Widget *,void *)
{
	main_GUI->m_s_jet->setonly();
	COLOR_TABLE = simpleJet;
	draw_clrBar();
	main_GUI->box_cMap->redraw();
}


void zoom_in_cb(Fl_Widget *,void *)
{
	tLoaded = 1;
	
	int step_num = 7;
	float rt_step, mz_step;

	rt_step = (rt_max_d - rt_min_d)/step_num;
	mz_step = (mz_max_d - mz_min_d)/step_num;
	
	rt_min_d = (rt_min_d + rt_step) > rt_max ? rt_max - 1 : (rt_min_d + rt_step);
	rt_max_d = (rt_max_d - rt_step) < rt_min ? rt_min + 1 : (rt_max_d - rt_step);

	mz_min_d = (mz_min_d + mz_step) > mz_max ? mz_max - 1 : (mz_min_d + mz_step);
	mz_max_d = (mz_max_d - mz_step) < mz_min ? mz_min + 1 : (mz_max_d - mz_step);

	set_label();
	
	if (NOTHREAD)
		show_img(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
	else
		imgThd(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
}


void zoom_out_cb(Fl_Widget *,void *)
{
	tLoaded = 1;

	int step_num = 7;
	float rt_step, mz_step;
	
	rt_step = (rt_max_d - rt_min_d)/step_num;
	mz_step = (mz_max_d - mz_min_d)/step_num;
	
	rt_min_d = (rt_min_d - rt_step) < rt_min ? rt_min : (rt_min_d - rt_step);
	rt_max_d = (rt_max_d + rt_step) > rt_max ? rt_max : (rt_max_d + rt_step);

	mz_min_d = (mz_min_d - mz_step) < mz_min ? mz_min : (mz_min_d - mz_step);
	mz_max_d = (mz_max_d + mz_step) > mz_max ? mz_max : (mz_max_d + mz_step);

	set_label();
	
	if (NOTHREAD)
		show_img(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
	else
		imgThd(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
}


void pan_down_cb(Fl_Widget *,void *)
{
	tLoaded = 1;

	int step_num = 7;
	float step;
	step = (rt_max_d - rt_min_d)/step_num;
	
	rt_min_d = (rt_min_d + step) > rt_max ? rt_max - 1 : (rt_min_d + step);
	rt_max_d = (rt_max_d + step) > rt_max ? rt_max : (rt_max_d + step);

	set_label();

	if (NOTHREAD)
		show_img(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
	else
		imgThd(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
}


void pan_up_cb(Fl_Widget *,void *)
{
	tLoaded = 1;

	int step_num = 7;
	float step;
	step = (rt_max_d - rt_min_d)/step_num;
	
	rt_min_d = (rt_min_d - step) < rt_min ? rt_min : (rt_min_d - step);
	rt_max_d = (rt_max_d - step) < rt_min ? rt_min + 1 : (rt_max_d - step);

	set_label();

	if (NOTHREAD)
		show_img(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
	else
		imgThd(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
}


void pan_left_cb(Fl_Widget *,void *)
{
	tLoaded = 1;

	int step_num = 7;
	float step;
	step = (mz_max_d - mz_min_d)/step_num;
	
	mz_min_d = (mz_min_d - step) < mz_min ? mz_min : (mz_min_d - step);
	mz_max_d = (mz_max_d - step) < mz_min ? mz_min + 1 : (mz_max_d - step);

	set_label();

	if (NOTHREAD)
		show_img(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
	else
		imgThd(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
}


void pan_right_cb(Fl_Widget *,void *)
{
	tLoaded = 1;

	int step_num = 7;
	float step;
	step = (mz_max_d - mz_min_d)/step_num;
	
	mz_min_d = (mz_min_d + step) > mz_max ? mz_max - 1 : (mz_min_d + step);
	mz_max_d = (mz_max_d + step) > mz_max ? mz_max : (mz_max_d + step);

	set_label();

	if (NOTHREAD)
		show_img(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
	else
		imgThd(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
}


void box_nav_cb(Fl_Widget *,void *)
{
	tLoaded = 1;

	rt_min_d = rt_min;
	rt_max_d = rt_max;
	mz_min_d = mz_min;
	mz_max_d = mz_max;

	set_label();
	
	if (NOTHREAD)
		show_img(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
	else
		imgThd(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
}


void box_main_cb(Fl_Widget *,void *)
{
	tLoaded = 1;

	float rt1, mz1, rt2, mz2;
	
	// region or point?
	int min_pixel_range = 10;
	if (abs(main_GUI->box_main->r_x2 - main_GUI->box_main->r_x1) < min_pixel_range || abs(main_GUI->box_main->r_y2 - main_GUI->box_main->r_y1)< min_pixel_range)
	{
		main_GUI->box_main->dFlag = 0;
	}
	
	if (main_GUI->box_main->dFlag == 1)
	{
		cout << ">>>>>>>> zoom into the selected region..." << endl;
		
		mz1 = mz_min_d + (mz_max_d - mz_min_d)*(main_GUI->box_main->r_x1 - main_GUI->box_main->x())/main_GUI->box_main->w();
		mz2 = mz_min_d + (mz_max_d - mz_min_d)*(main_GUI->box_main->r_x2 - main_GUI->box_main->x())/main_GUI->box_main->w();
		rt1 = rt_min_d + (rt_max_d - rt_min_d)*(main_GUI->box_main->r_y1 - main_GUI->box_main->y())/main_GUI->box_main->h();
		rt2 = rt_min_d + (rt_max_d - rt_min_d)*(main_GUI->box_main->r_y2 - main_GUI->box_main->y())/main_GUI->box_main->h();

		if ( rt2-rt1 < numeric_limits<double>::min() || mz2-mz1 < numeric_limits<double>::min())
		{
			cout << "Range out of resolution! " << endl;
		}
		else
		{
			rt_min_d = rt1;
			rt_max_d = rt2;
			mz_min_d = mz1;
			mz_max_d = mz2;
			
			set_label();
			
			if (NOTHREAD)
				show_img(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
			else
				imgThd(in_file, rt_min_d, rt_max_d, mz_min_d, mz_max_d, chunk_size, tLoaded);
		}
		
		cout << "			<<<<<<<<< zoom done! " << endl;
	}
	else
	{
		cout << ">>>>>> click without drag!" << endl;

		main_GUI->sld_rt->value((double)(main_GUI->box_main->y1 - main_GUI->box_main->y())/(double)(main_GUI->box_main->h()));
		main_GUI->sld_mz->value((double)(main_GUI->box_main->x1 - main_GUI->box_main->x())/(double)(main_GUI->box_main->w()));
		
		main_GUI->sld_rt->do_callback();
		main_GUI->sld_mz->do_callback();
		
		cout << "			<<<<<< updated to the selected position! " << endl;
	}
}
	
	
void box_spectro_cb(Fl_Widget *,void *)
{
  int 	i,
		min_xx,  // position of the x axis start
		max_xx,  // position for the last plot point
		min_yy;  
  float stData,  // position for line start point
		edData,  // position for line end point
		xx_step, // position step on the x axis
		yy_ratio; //ration to map data value into the box range
  int yOff = 5;
  double mCounts = 0.0;
  float scale;

  int cur_row;

  fl_color(FL_BLUE); // set the line color
  
  min_xx = main_GUI->box_spectro->x();
  max_xx = main_GUI->box_spectro->x() + main_GUI->box_spectro->w();
  min_yy = main_GUI->box_spectro->y() + main_GUI->box_spectro->h() - yOff;
  
  EnterCriticalSection(&g_cs);
  
  xx_step = (float)(main_GUI->imgData.cols())/(float)(main_GUI->box_spectro->w()); 
  cur_row = (int)(main_GUI->sld_rt->value()* (main_GUI->imgData.rows()-1));

  for (i = 1; i < main_GUI->imgData.cols(); i++)
	  mCounts = mCounts > main_GUI->imgData(cur_row, i) ? mCounts : main_GUI->imgData(cur_row, i);

	  
	  
  scale = log(mCounts)/log(main_GUI->frmMaxCounts);

  yy_ratio = scale*(float)(main_GUI->box_spectro->h() - 2*yOff) / mCounts;

  stData = main_GUI->imgData(cur_row,0) * yy_ratio;

   for (i = 1; i < main_GUI->box_spectro->w(); i++)
  {
    int t = int(i * xx_step);
	edData = main_GUI->imgData(cur_row,t) * yy_ratio;

	fl_line(min_xx-1+i, min_yy-stData, min_xx+i, min_yy-edData);
	stData = edData;
  }
   
  LeaveCriticalSection(&g_cs); 
}


void box_chroma_cb(Fl_Widget *,void *)
{
  int 	i,
		min_xx,  // position of the x axis start
		max_xx,  // position for the last plot point
		min_yy;  
  float stData,  // position for line start point
		edData,  // position for line end point
		xx_step, // position step on the x axis
		yy_ratio; //ration to map data value into the box range
  int yOff = 5;
  double mCounts = 0.0;
  float scale; // scale between log(mCounts) and log(frmMaxCounts)

  int cur_col;

  fl_color(FL_BLUE); // set the line color
		
  min_xx = main_GUI->box_chroma->y();
  max_xx = main_GUI->box_chroma->y() + main_GUI->box_chroma->h();
  min_yy = main_GUI->box_chroma->x() + yOff;

  
  EnterCriticalSection(&g_cs); 
   
  xx_step = (float)(main_GUI->imgData.rows())/(float)(main_GUI->box_chroma->h()); 
  cur_col = (int)(main_GUI->sld_mz->value()* (main_GUI->imgData.cols()-1));
  
  for (i = 1; i < main_GUI->imgData.rows(); i++)
	  mCounts = mCounts > main_GUI->imgData(i, cur_col) ? mCounts : main_GUI->imgData(i, cur_col);

  scale = log(mCounts)/log(main_GUI->frmMaxCounts);

  yy_ratio = scale*(float)(main_GUI->box_chroma->w() - 2*yOff) /mCounts;

  
  stData = main_GUI->imgData(0, cur_col) * yy_ratio;

   for (i = 1; i < main_GUI->box_chroma->h(); i++)
  {
    int t = int(i * xx_step);
	edData = main_GUI->imgData(t, cur_col) * yy_ratio;
	
	fl_line(min_yy+stData, min_xx-1+i, min_yy+edData, min_xx+i);
	stData = edData;
  }

  LeaveCriticalSection(&g_cs); 	
}


class ClosePad : public Fl_Window 
{
	void ok_cb2(Fl_Widget *w) 
	{
		Fl_Window::~Fl_Window();
	}
	
	static void ok_cb(Fl_Widget *w, void *data)
	{
		ClosePad *mypad = (ClosePad*)data;
		mypad->ok_cb2(w);
	}

public:
	ClosePad(int X,int Y,int W=200,int H=60,const char *L=0):Fl_Window(X,Y,W,H,L) 
	{
		Fl_Output *txt;
		txt = new Fl_Output(120, 5, 0, 25, "File closed!");
		
		Fl_Button *b;
		b = new Fl_Button(110, 35, 60, 20, "OK");
		b->callback(ok_cb, (void*)this);
	}
};


void close_cb(Fl_Widget *,void *)
{
	main_GUI->box_main->deactivate();
	main_GUI->box_nav->deactivate();

	main_GUI->m_clrmap->activate();
	Fl::check();
	
	stopStream = 1; 

	ClosePad *CPad;
	CPad = new ClosePad(0, 0);
	CPad->position(main_GUI->x(), main_GUI->y());
	CPad->show();
}


void quit_cb(Fl_Widget *,void *)
{
	exit(0);
}



void timer_cb(void *v) { 
   Fl::repeat_timeout(double(1.0)/FPS,timer_cb); 
} // timer_cb() 


int fl_viewer()
{
	// === initialise the view ===
	fl_register_images(); 
	Fl::get_system_colors(); 

	main_GUI = new myOverlay(1650, 1010);

	img_w = main_GUI->box_main->w();
	img_h = main_GUI->box_main->h();

	main_GUI->sld_rt->callback(sld_rt_cb);
	main_GUI->sld_mz->callback(sld_mz_cb);
	main_GUI->m_open->callback(open_cb);
	main_GUI->btn_open->callback(open_cb);
	main_GUI->m_close->callback(close_cb);
	main_GUI->m_exit->callback(quit_cb);
	main_GUI->btn_exit->callback(quit_cb);
	main_GUI->m_cubh->callback(cubh_cb);
	main_GUI->m_jet->callback(jet_cb);
	main_GUI->m_s_jet->callback(s_jet_cb);

	main_GUI->pan_down->callback(pan_down_cb);
	main_GUI->pan_up->callback(pan_up_cb);
	main_GUI->pan_left->callback(pan_left_cb);
	main_GUI->pan_right->callback(pan_right_cb);
	
	main_GUI->zoom_in->callback(zoom_in_cb);
	main_GUI->zoom_out->callback(zoom_out_cb);
	
	main_GUI->box_main->callback(box_main_cb);
	main_GUI->box_nav->callback(box_nav_cb);
	main_GUI->box_spectro->callback(box_spectro_cb);
	main_GUI->box_chroma->callback(box_chroma_cb);

	main_GUI->imgData.setZero(main_GUI->box_main->h(), main_GUI->box_main->w());
	main_GUI->maxCounts = 0.0;
	main_GUI->frmMaxCounts = 0.0;
	main_GUI->RGB_data = new unsigned char[img_w * img_h * 3];
	main_GUI->omics_RGB_img = new Fl_RGB_Image(main_GUI->RGB_data,img_w,img_h,3);
	main_GUI->omics_img_disp = new Fl_Image(img_w,img_h,3);

	char s[30];

	sprintf(s, "%.2f", mz_min_d);
	main_GUI->txt_minMZ->value(s);
	
	sprintf(s, "%.2f", mz_max_d);
	main_GUI->txt_maxMZ->value(s);

	sprintf(s, "%.2f", rt_min_d);
	main_GUI->txt_minRT->value(s);

	sprintf(s, "%.2f", rt_max_d);
	main_GUI->txt_maxRT->value(s);

	sprintf(s, "%.1f", 0.0);
	main_GUI->txt_cMap_min->value(s);

	sprintf(s, "%.1f", 1.0);
	main_GUI->txt_cMap_max->value(s);

	draw_clrBar();

	main_GUI->resizable(main_GUI);

	main_GUI->redraw_overlay();

	Fl::visual(FL_RGB);         
	Fl::lock();

	Fl::add_timeout(0.1,timer_cb); // start timer after 0.1 sec delay
	main_GUI->show();	

	return(Fl::run());
}

#endif