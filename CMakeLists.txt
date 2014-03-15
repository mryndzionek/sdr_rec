#################################################################
#   GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY    #
#################################################################

cmake_minimum_required (VERSION 2.8)

project (sdr_rec C)

set(SDR_REC_VERSION "0.0.0")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lm -lc")

set (SOURCES_files_Source_Files
    src/rtl_asgram.c
    src/normalizer.c
    src/timer.c
	external/rtl-sdr/src/convenience/convenience.c
)
source_group ("Source Files" FILES ${SOURCES_files_Source_Files})

set (SOURCES_Source_Files
  ${SOURCES_files_Source_Files}
)

set (SOURCES_files_Header_Files
    include/rtl_asgram.h
    include/normalizer.h
    include/debug.h
    include/timer.h
	external/rtl-sdr/src/convenience/convenience.h
)
source_group ("Header Files" FILES ${SOURCES_files_Header_Files})

set (SOURCES_Header_Files
  ${SOURCES_files_Header_Files}
)

set (SOURCES_
  ${SOURCES_Source_Files}
  ${SOURCES_Header_Files}
)

find_library(LIQUID NAMES libliquid.a liquid PATHS external/liquid-dsp/build/lib)
find_library(RTLSDR NAMES librtlsdr.a rtlsdr PATHS external/rtl-sdr/build/lib)

include_directories (
  include
  ${CMAKE_INSTALL_PREFIX}/include
  external/liquid-dsp/build/include
  external/liquid-dsp/include
  external/rtl-sdr/build/include
  external/rtl-sdr/src/convenience
  ${SOURCES_Platform_Files}
)

add_executable (
    rtl_asgram
    ${SOURCES_}
)
target_link_libraries(rtl_asgram ${LIQUID} ${RTLSDR} fftw3f usb-1.0 m)

install (
    FILES ${SOURCES_files_Header_Files} DESTINATION include
)

#################################################################
#   GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY    #
#################################################################
