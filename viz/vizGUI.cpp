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

#define NOMINMAX
#include <Windows.h> //using WinMain

#include <iostream>
#include <boost/filesystem.hpp>

#include "SMVStreamer.hpp"
#include "FL_viewer.h"

using namespace std;

// Use standard main to have console background:
int main()

// Use WinMain if you don't want the console in the background:
/*int __stdcall WinMain(
    __in HINSTANCE hInstance,
    __in_opt HINSTANCE hPrevInstance,
    __in LPSTR lpCmdLine,
    __in int nShowCmd
    )*/
{
	cout << ">>> seaMass Visualisation GUI..." << endl;
	
	InitializeCriticalSection(&g_cs); 
	
	fl_viewer();
	
	DeleteCriticalSection(&g_cs); 

	return 0;
}

