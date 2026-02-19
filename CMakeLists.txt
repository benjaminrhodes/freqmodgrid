cmake_minimum_required(VERSION 3.20)
project(FreqmodGrid VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED IPLUG2_DIR)
  set(IPLUG2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/iPlug2" CACHE PATH "iPlug2 root directory")
endif()

include(${IPLUG2_DIR}/iPlug2.cmake)
find_package(iPlug2 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

iplug_add_plugin(${PROJECT_NAME}
  SOURCES
    src/iPlug/FreqmodGrid.cpp
    src/iPlug/FreqmodGrid.h
    src/iPlug/FreqmodGrid_DSP.h
    src/iPlug/PresetManager.cpp
    src/iPlug/PresetManager.h
    src/DSP/FMEngine.h
    src/DSP/FMEngine.cpp
    src/DSP/Operator.h
    src/DSP/Envelope.h
    src/DSP/Filter.h
    src/DSP/LFO.h
    src/DSP/Effects.h
    src/DSP/Constants.h
    src/DSP/StereoChorus.h
    src/DSP/StereoDelay.h
    src/DSP/Oversampler.h
    resources/config.h
  LINK
    iPlug2::Extras::Synth
)
