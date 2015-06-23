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

#pragma once

#define NOMINMAX
#include <windows.h>

#include <string>
#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>
//#include <boost/gil/gil_all.hpp>
//#include <Eigen/Core>
#include <boost/timer/timer.hpp>
//#include "PNGWriter.hpp"
#include "FL_vizGUI.h"
#include "aux_Eigen.hpp"

using namespace std;
using namespace boost;
//using namespace Eigen;

struct Coef
{
	int lx, ly, x, y;
	fp v;
	//double v;
	
	bool operator()(Coef& a, Coef& b);
};


class Reconstructer
{
protected:
	filesystem::path out_path;

	// accumulated image
	//Matrix<double,Dynamic,Dynamic> img; // accumulated image
	Matrix<fp,Dynamic,Dynamic> img;
	double mz_min;
	double mz_max;
	double rt_min;
	double rt_max;
	double max_counts;

	filesystem::path stream_path;
	ofstream         stream_ofs;
	int              stream_index;
	int              chunk_index;

    timer::cpu_timer read_time;
    timer::cpu_timer viz_time;
	//double read_start;
	//double read_time;
	//double viz_start;
	//double viz_time;
	size_t chunk_count;
	PNGWriter<fp> writer; //PNGWriter writer;

	int nx,ny,nxy;

public:
	Reconstructer(const string& out_dir, int width, int height);

	void next_stream(double mz_min, double mz_max, double rt_min, double rt_max); 
	void next_chunk(const vector<Coef>& cs);

	void fl_next_chunk(const vector<Coef>& cs, myOverlay* m_GUI, int ovwFlag);
	void fl_close_stream_ofs();
};