//
// Original authors: Andrew Dowsey <andrew.dowsey@manchester.ac.uk>
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

#include "aux_Eigen.hpp"

#include <iostream>
#include <sstream>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <math.h>


extern ClrMap COLOR_TABLE;

// http://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale

COLOUR GetColour(fp v,fp vmin,fp vmax)
{
   COLOUR c = {1.0f,1.0f,1.0f}; // white
   fp dv;

   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;

   dv = vmax - vmin;
	  
   if (COLOR_TABLE == simpleJet)  
   {
	   //dv = vmax - vmin;

	   if (v < (vmin + 0.25f * dv)) {
		  c.r = 0;
		  c.g = 4 * (v - vmin) / dv;
	   } else if (v < (vmin + 0.5f * dv)) {
		  c.r = 0;
		  c.b = 1 + 4 * (vmin + 0.25f * dv - v) / dv;
	   } else if (v < (vmin + 0.75f * dv)) {
		  c.r = 4 * (v - vmin - 0.5f * dv) / dv;
		  c.b = 0;
	   } else {
		  c.g = 1 + 4 * (vmin + 0.75f * dv - v) / dv;
		  c.b = 0;
	   }

// end http://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale

   }
   else if (COLOR_TABLE == matlabJet)  
   {
	   //dv = vmax - vmin;

	   if (v < (vmin + 0.125f * dv)) {
		   c.b = 0.5f + 4*(v -vmin)/dv ;
		   c.g = 0;
		   c.r = 0;
	   }
	   else if (v < (vmin + 0.375f * dv) ) {
		  c.r = 0;
		  c.g = 4 * (v - vmin - 0.125f * dv) / dv;
	   } 
	   else if (v < (vmin + 0.625f * dv)) {
		  c.r = 4 * (v - vmin - 0.375f * dv) / dv;
		  c.b = 1 + 4 * (vmin + 0.375f * dv - v) / dv;
	   } 
	   else if (v < (vmin + 0.875f * dv)) {
		  c.g = 1 + 4 * (vmin + 0.625f * dv - v) / dv;
		  c.b = 0;
	   } 
	   else {
		  c.r = 1 + 4 * (vmin + 0.875f * dv - v) / dv;
		  c.g = 0;
		  c.b = 0;
	   }
   }
   else if (COLOR_TABLE == cubeHelix)  
   {
		fp START, ROTS, HUE, GAMMA;
		START = 0.5f;
		ROTS = -1.5f;
		HUE = 1.0f;
		GAMMA = 1.0f;
		
		int NLEV = 256;
		
		fp fract, angle, amp;
		fp PI = 4.0f*atan(1.0f);
		fract = (v - vmin)/dv;
		angle = 2 * PI * (START/3.0f + 1.0f + ROTS*fract);
		fract = fract * GAMMA;
		amp = HUE*fract*(1-fract)/2.0;
		
		c.r = 1.0 - fract+amp*(-0.14861*cos(angle)+1.78277*sin(angle));
        c.g = 1.0 - fract+amp*(-0.29227*cos(angle)-0.90649*sin(angle));
        c.b = 1.0 - fract+amp*(+1.97294*cos(angle));
		
		if (c.r < 0.0f) c.r = 0.0;
		if (c.g < 0.0f) c.g = 0.0;
		if (c.b < 0.0f) c.b = 0.0;
		if (c.r > 1.0f) c.r = 1.0f;
		if (c.g > 1.0f) c.g = 1.0f;
		if (c.b > 1.0f) c.b = 1.0f;
   }

   return(c);
}

