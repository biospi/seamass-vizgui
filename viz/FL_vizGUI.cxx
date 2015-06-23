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


#include "FL_vizGUI.h"

Fl_Menu_Item myOverlay::menu_main_menu[] = {
 {"File", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Open...", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Close", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Exit", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Colormap", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"S_jet", 0,  0, 0, 8, FL_NORMAL_LABEL, 0, 14, 0},
 {"Jet", 0,  0, 0, 8, FL_NORMAL_LABEL, 0, 14, 0},
 {"CubeHelix", 0,  0, 0, 12, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"About", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};
Fl_Menu_Item* myOverlay::m_file = myOverlay::menu_main_menu + 0;
Fl_Menu_Item* myOverlay::m_open = myOverlay::menu_main_menu + 1;
Fl_Menu_Item* myOverlay::m_close = myOverlay::menu_main_menu + 2;
Fl_Menu_Item* myOverlay::m_exit = myOverlay::menu_main_menu + 3;
Fl_Menu_Item* myOverlay::m_clrmap = myOverlay::menu_main_menu + 5;
Fl_Menu_Item* myOverlay::m_s_jet = myOverlay::menu_main_menu + 6;
Fl_Menu_Item* myOverlay::m_jet = myOverlay::menu_main_menu + 7;
Fl_Menu_Item* myOverlay::m_cubh = myOverlay::menu_main_menu + 8;
Fl_Menu_Item* myOverlay::m_about = myOverlay::menu_main_menu + 10;

void myOverlay::resize(int X, int Y, int W, int H) 
{
	Fl_Overlay_Window::resize(X, Y, W, H);
}

void  myOverlay::msVis_Win() {
  { 
    { main_menu = new Fl_Menu_Bar(0, 0, 1650, 20);
      main_menu->menu(menu_main_menu);
    } // Fl_Menu_Bar* main_menu	
    { main_tab = new Fl_Tabs(0, 20, 1650, 1002);
      { tab1_group = new Fl_Group(0, 40, 1650, 975, "Tab 1");
        tab1_group->selection_color(FL_DARK3);
        tab1_group->labelsize(12);
        { box_spectro = new spectroBox(90, 52, 1350, 160);
          box_spectro->box(FL_EMBOSSED_BOX);
        } // Fl_Box* box_spectro
        { box_chroma = new chromaBox(1451, 225, 179, 680);
		  box_chroma->box(FL_EMBOSSED_BOX);
        } // Fl_Box* box_chroma
        { box_nav = new navBox(1450, 52, 180, 160);
          box_nav->box(FL_EMBOSSED_BOX);
        } // Fl_Box* box_nav
        { btn_close = new Fl_Button(1400, 972, 100, 30, "Close");
        } // Fl_Button* btn_close
        { btn_exit = new Fl_Return_Button(1517, 972, 115, 30, "Exit");
        } // Fl_Return_Button* btn_exit
        { btn_open = new Fl_Button(1278, 972, 100, 30, "Open...");
        } // Fl_Button* btn_open
        { opt_cross = new Fl_Output(110, 972, 240, 25, "Cross position: (rt, mz, counts)");
		  opt_cross->align(Fl_Align(132));
        } // Fl_Output* opt_cross
        { grp_null = new Fl_Group(0, 965, 1650, 4);
          grp_null->box(FL_DOWN_FRAME);
          grp_null->end();
        } // Fl_Group* grp_null
        { sld_rt = new Fl_Slider(68, 225, 20, 680);
          sld_rt->type(4);
        } // Fl_Slider* sld_rt
        { sld_mz = new Fl_Slider(90, 907, 1350, 19);
          sld_mz->type(5);
        } // Fl_Slider* sld_mz
        { { box_main = new mzBox(90, 225, 1350, 680);
            box_main->box(FL_DOWN_BOX);
			box_main->align(FL_ALIGN_IMAGE_BACKDROP);
            Fl_Group::current()->resizable(box_main);
          } // Fl_Box* box_main
        } // Fl_Scroll* scr_main
        { txt_curTime = new Fl_Output(20, 188, 50, 22, "Current RT");
          txt_curTime->labelsize(10);
          txt_curTime->textsize(10);
          txt_curTime->align(Fl_Align(FL_ALIGN_TOP_RIGHT));
        } // Fl_Output* txt_curTime
        { txt_curMZ = new Fl_Output(840, 940, 0, 22, "Mass to Charge (m/z)");
          txt_curMZ->labelsize(16);
          txt_curMZ->textsize(16);
        } // Fl_Output* txt_curMZ
        { txt_minRT = new Fl_Output(20, 220, 50, 22);
          txt_minRT->box(FL_NO_BOX);
          txt_minRT->labelsize(16);
          txt_minRT->textsize(16);
          txt_minRT->align(Fl_Align(FL_ALIGN_RIGHT));
        } // Fl_Output* txt_minRT
        { txt_maxRT = new Fl_Output(20, 890, 55, 22);
          txt_maxRT->box(FL_NO_BOX);
          txt_maxRT->labelsize(16);
          txt_maxRT->textsize(16);
          txt_maxRT->align(Fl_Align(FL_ALIGN_RIGHT));
        } // Fl_Output* txt_maxRT
        { txt_minMZ = new Fl_Output(92, 925, 55, 22);
          txt_minMZ->box(FL_NO_BOX);
          txt_minMZ->labelsize(16);
          txt_minMZ->textsize(16);
        } // Fl_Output* txt_minMZ
        { txt_maxMZ = new Fl_Output(1385, 925, 55, 22);
          txt_maxMZ->box(FL_NO_BOX);
          txt_maxMZ->labelsize(16);
          txt_maxMZ->textsize(16);
          txt_maxMZ->align(Fl_Align(FL_ALIGN_RIGHT));
        } // Fl_Output* txt_maxMZ
        { pan_group = new Fl_Group(868, 969, 170, 35, "Pan: ");
          pan_group->box(FL_EMBOSSED_FRAME);
          pan_group->align(Fl_Align(FL_ALIGN_LEFT));
          { pan_left = new Fl_Button(878, 971, 30, 30, "@<-");
          } // Fl_Button* pan_left
          { pan_down = new Fl_Button(958, 971, 30, 30, "@2->");
          } // Fl_Button* pan_down
          { pan_right = new Fl_Button(998, 971, 30, 30, "@->");
          } // Fl_Button* pan_right
          { pan_up = new Fl_Button(918, 971, 30, 30, "@8->");
          } // Fl_Button* pan_up
          pan_group->end();
        } // Fl_Group* pan_group
 		{ zoom_group = new Fl_Group(1148, 970, 90, 35, "Zoom: ");
          zoom_group->box(FL_EMBOSSED_FRAME);
          zoom_group->align(Fl_Align(FL_ALIGN_LEFT));
          { zoom_out = new Fl_Button(1158, 971, 30, 30, "@line");
          } // Fl_Button* pan_left
          { zoom_in  = new Fl_Button(1198, 971, 30, 30, "@+");
          } // Fl_Button* pan_down
          zoom_group->end();
        } // Fl_Group* zoom_group
        { txt_coef = new Fl_Output(1455, 930, 175, 29, "Coefficients used (kb):");
          txt_coef->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        } // Fl_Output* txt_coef
        { txt_RT2 = new Fl_Output(20, 354, 50, 22);
          txt_RT2->box(FL_NO_BOX);
          txt_RT2->labelsize(16);
          txt_RT2->textsize(16);
          txt_RT2->align(Fl_Align(FL_ALIGN_RIGHT));
        } // Fl_Output* txt_RT2
        { txt_RT3 = new Fl_Output(20, 488, 50, 22);
          txt_RT3->box(FL_NO_BOX);
          txt_RT3->labelsize(16);
          txt_RT3->textsize(16);
          txt_RT3->align(Fl_Align(FL_ALIGN_RIGHT));
        } // Fl_Output* txt_RT3
        { txt_RT4 = new Fl_Output(20, 622, 50, 22);
          txt_RT4->box(FL_NO_BOX);
          txt_RT4->labelsize(16);
          txt_RT4->textsize(16);
          txt_RT4->align(Fl_Align(FL_ALIGN_RIGHT));
        } // Fl_Output* txt_RT4
        { txt_RT5 = new Fl_Output(20, 756, 50, 22);
          txt_RT5->box(FL_NO_BOX);
          txt_RT5->labelsize(16);
          txt_RT5->textsize(16);
          txt_RT5->align(Fl_Align(FL_ALIGN_RIGHT));
        } // Fl_Output* txt_RT5
        { txt_MZ2 = new Fl_Output(334, 925, 55, 22);
          txt_MZ2->box(FL_NO_BOX);
          txt_MZ2->labelsize(16);
          txt_MZ2->textsize(16);
        } // Fl_Output* txt_MZ2
        { txt_MZ3 = new Fl_Output(598, 925, 55, 22);
          txt_MZ3->box(FL_NO_BOX);
          txt_MZ3->labelsize(16);
          txt_MZ3->textsize(16);
        } // Fl_Output* txt_MZ3
        { txt_MZ4 = new Fl_Output(862, 925, 55, 22);
          txt_MZ4->box(FL_NO_BOX);
          txt_MZ4->labelsize(16);
          txt_MZ4->textsize(16);
        } // Fl_Output* txt_MZ4
        { txt_MZ5 = new Fl_Output(1126, 925, 55, 22);
          txt_MZ5->box(FL_NO_BOX);
          txt_MZ5->labelsize(16);
          txt_MZ5->textsize(16);
        } // Fl_Output* txt_MZ5
		{ //box_cMap = new Fl_Box(414, 978, 311, 18);
		  box_cMap = new cmpBox(414, 978, 311, 18);
          box_cMap->box(FL_UP_BOX);
        } // Fl_Box* box_cMap
        { txt_cMap_min = new Fl_Output(378, 974, 35, 22);
          txt_cMap_min->box(FL_NO_BOX);
          txt_cMap_min->textsize(16);
          txt_cMap_min->align(Fl_Align(FL_ALIGN_RIGHT));
        } // Fl_Output* txt_cMap_min
        { txt_cMap_max = new Fl_Output(728, 974, 82, 22);
          txt_cMap_max->box(FL_NO_BOX);
          txt_cMap_max->textsize(16);
        } // Fl_Output* txt_cMap_max
        tab1_group->end();
      } // Fl_Group* tab1_group
      main_tab->end();
      Fl_Group::current()->resizable(main_tab);
    } // Fl_Tabs* main_tab
  } // msVis_Win
}


void myOverlay::draw_overlay() 
{
  // ===================================
  // === draw the cross in  box_main
  int lc_x1, lc_y1, lc_x2, lc_y2; // points for the lc_line
  int ms_x1, ms_y1, ms_x2, ms_y2; // points for the ms_line
  fl_color(FL_RED);
  
  // draw the lc_line
  lc_x1 = box_main->x();
  lc_x2 = lc_x1 + box_main->w();
  lc_y1 = (int)(sld_rt->value()*box_main->h()) + box_main->y();
  lc_y2 = lc_y1;
  fl_line(lc_x1, lc_y1, lc_x2, lc_y2);
  
  // draw the ms_line
  ms_x1 = (int)(sld_mz->value()*box_main->w()) + box_main->x();
  ms_x2 = ms_x1;
  ms_y1 = box_main->y();
  ms_y2 = ms_y1 + box_main->h();
  fl_line(ms_x1, ms_y1, ms_x2, ms_y2);
  
  // draw the viewport rectangle in navigation box
  fl_color(FL_RED);
  int n_x1, n_y1, n_x2, n_y2;
 
  n_x1 = box_nav->x() + box_nav->w() *(mz_min_d - mz_min)/(mz_max - mz_min);
  n_x2 = box_nav->x() + box_nav->w() *(mz_max_d - mz_min)/(mz_max - mz_min);
  n_y1 = box_nav->y() + box_nav->h() *(rt_min_d - rt_min)/(rt_max - rt_min);
  n_y2 = box_nav->y() + box_nav->h() *(rt_max_d - rt_min)/(rt_max - rt_min);
 
  fl_line(n_x1, n_y1, n_x2, n_y1);
  fl_line(n_x1, n_y1, n_x1, n_y2);
  fl_line(n_x2, n_y1, n_x2, n_y2);
  fl_line(n_x1, n_y2, n_x2, n_y2);
}

