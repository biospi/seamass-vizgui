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


#ifndef vizGUI_h
#define vizGUI_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Overlay_Window.H>
#include <FL/fl_draw.H>
#include <iostream>

#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

#include <Eigen/Core>

using namespace Eigen;

extern int img_w; // width of the raw image
extern int img_h; // heigh of the raw image
extern float rt_min; // the minimum rt in the dataset
extern float rt_max;  // the maximum rt in the dataset
extern float mz_min;  // the minimum mz in the dataset
extern float mz_max;  // the maximum mz in the dataset
extern float rt_min_d; // the minimum rt displayed in box_main
extern float rt_max_d;  // the maximum rt displayed in box_main
extern float mz_min_d;  // the minimum mz displayed in box_main
extern float mz_max_d;  // the maximum mz displayed in box_mainopen

class cmpBox : public Fl_Box
{
protected:
	void resize(int X, int Y, int W, int H) {
		Fl_Image *temp, *old_img;
		if (old_img = image())
		{
			temp = old_img ->copy(W, H);
			image(temp);
		}
		delete old_img;

        Fl_Box::resize(X,Y,W,H);

       redraw();
    }

public:
    cmpBox(int X, int Y, int W, int H, const char *L=0) : Fl_Box(X,Y,W,H,L)	{		
    }
};

class navBox : public Fl_Box 
{
protected:
    int handle(int e) {
        int ret = Fl_Box::handle(e);
        switch ( e ) {
            case FL_PUSH:
				do_callback();
                return(1);    
        }
        return(ret);
    }

public:
    navBox(int X, int Y, int W, int H, const char *L=0) : Fl_Box(X,Y,W,H,L)	{		
    }

};


class mzBox : public Fl_Box 
{
protected:
	void erase_box() 
	{
		window()->make_current();
		fl_overlay_clear();
	}
	
    int handle(int e) {
        static int offset[2] = { 0, 0 };
        int ret = Fl_Box::handle(e);
        switch ( e ) {
            case FL_PUSH:
				erase_box();
				dFlag = 0;
				x1 = Fl::event_x();
				y1 = Fl::event_y();
                return(1);

            case FL_DRAG:
				erase_box();
				dFlag = 1;
				x2 = Fl::event_x();
				y2 = Fl::event_y();
				
				r_x1 = x2>x1 ? x1 : x2;
				r_x2 = x2>x1 ? x2 : x1;
				r_y1 = y2>y1 ? y1 : y2;
				r_y2 = y2>y1 ? y2 : y1;
					
				window()->make_current();
				fl_overlay_rect(r_x1, r_y1, r_x2 - r_x1, r_y2 - r_y1);
                return(1);

            case FL_RELEASE:
				erase_box();
				do_callback();
                return(1);
        }
        return(ret);
    }

public:
	int r_x1, r_x2, r_y1, r_y2;
	int x1, y1, x2, y2;
	
	int dFlag;  // flag to present if it is a drag or simple click

    mzBox(int X, int Y, int W, int H, const char *L=0) : Fl_Box(X,Y,W,H,L) {
		dFlag = 0;
    }
};


class spectroBox : public Fl_Box
{
protected:
	void draw() {
		draw_box();
		do_callback();
	}

public:
    spectroBox(int X, int Y, int W, int H, const char *L=0) : Fl_Box(X,Y,W,H,L)	{	
    }
};


class chromaBox : public  Fl_Box
{
protected:
	void draw() {
		draw_box();
		do_callback();
	}

public:
    chromaBox(int X, int Y, int W, int H, const char *L=0) : Fl_Box(X,Y,W,H,L)	{		
    }
};


class myOverlay : public Fl_Overlay_Window {
public:
  void msVis_Win();
  Fl_Menu_Bar *main_menu;
  static Fl_Menu_Item menu_main_menu[];
  static Fl_Menu_Item *m_file;
  static Fl_Menu_Item *m_open;
  static Fl_Menu_Item *m_close;
  static Fl_Menu_Item *m_exit;
  static Fl_Menu_Item *m_clrmap;
  static Fl_Menu_Item *m_s_jet;
  static Fl_Menu_Item *m_jet;
  static Fl_Menu_Item *m_cubh;
  static Fl_Menu_Item *m_about;
  Fl_Tabs *main_tab;
  Fl_Group *tab1_group;
  spectroBox *box_spectro;
  chromaBox *box_chroma;
  navBox *box_nav;
  Fl_Button *btn_close;
  Fl_Return_Button *btn_exit;
  Fl_Button *btn_open;
  Fl_Output *opt_cross;
  Fl_Group *grp_null;
  Fl_Slider *sld_rt;
  Fl_Slider *sld_mz;
  mzBox *box_main;
  Fl_Output *txt_curTime;
  Fl_Output *txt_curMZ;
  Fl_Output *txt_minRT;
  Fl_Output *txt_maxRT;
  Fl_Output *txt_minMZ;
  Fl_Output *txt_maxMZ;
  Fl_Group *pan_group;
  Fl_Button *pan_left;
  Fl_Button *pan_down;
  Fl_Button *pan_right;
  Fl_Button *pan_up;
  Fl_Group *zoom_group;
  Fl_Button *zoom_in;
  Fl_Button *zoom_out;
  Fl_Output *txt_coef;
  Fl_Output *txt_RT2;
  Fl_Output *txt_RT3;
  Fl_Output *txt_RT4;
  Fl_Output *txt_RT5;
  Fl_Output *txt_MZ2;
  Fl_Output *txt_MZ3;
  Fl_Output *txt_MZ4;
  Fl_Output *txt_MZ5;

  cmpBox *box_cMap;
  Fl_Output *txt_cMap_min;
  Fl_Output *txt_cMap_max;
  
  Matrix<float, Dynamic, Dynamic> imgData;
  double maxCounts;
  double frmMaxCounts;
  
  Fl_RGB_Image  *  omics_RGB_img; 
  unsigned char *  RGB_data;
  Fl_Image * omics_img_disp;
  Fl_Image * omics_nail_img;

  void draw_overlay();
  void resize(int X, int Y, int W, int H);
  myOverlay(int W, int H, const char *L ="MZ Data Viewer"):Fl_Overlay_Window(W,H,L) {msVis_Win();}
};
#endif
