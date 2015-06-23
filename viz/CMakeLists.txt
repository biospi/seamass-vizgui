set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(FLTK_DIR "D:/Kitware/Kitware_FLTK/fltk-1.3.2-x64")

find_package(SpatialIndex 1.8.5 REQUIRED)

find_package(PNG 1.5 REQUIRED)

find_package(Boost 1.54 REQUIRED COMPONENTS
	system filesystem program_options chrono timer
)
find_package(Eigen3 3.2.0 REQUIRED)

include_directories(${FLTK_DIR})
include_directories(${FLTK_DIR}/ide/VisualC2010)

link_directories(${FLTK_DIR}/lib)
 
add_executable(vizGUI
	vizGUI.cpp
	aux_Eigen.hpp
	aux_Eigen.cpp
	SMVStreamer.hpp
	SMVStreamer.cpp
	Reconstructer.hpp
	Reconstructer.cpp
	FL_viewer.h
	FL_vizGUI.h
	FL_vizGUI.cxx
)

target_include_directories(vizGUI PUBLIC
  ${SPATIALINDEX_INCLUDE_DIR}
  ${PNG_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIR}
)

target_link_libraries (vizGUI LINK_PUBLIC
  ${SPATIALINDEX_LIBRARY}
  ${PNG_LIBRARIES}
  ${Boost_LIBRARIES}
  debug fltkd.lib
  debug fltkformsd.lib
  debug fltkgld.lib
  debug fltkimagesd.lib
  debug fltkjpegd.lib
  debug fltkzlibd.lib
  optimized fltk.lib
  optimized fltkforms.lib
  optimized fltkgl.lib
  optimized fltkimages.lib
  optimized fltkjpeg.lib
  optimized fltkzlib.lib
)

add_definitions(
  ${PNG_DEFINITIONS}
)
