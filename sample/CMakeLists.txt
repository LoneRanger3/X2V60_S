cmake_minimum_required(VERSION 3.10)
project(car_demo)

set(SAMPLE_SOURCE_ROOT ${CMAKE_SOURCE_DIR})

if(CMAKE_TOOLCHAIN_FILE)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Installation Directory")
endif()

#platform
#ARCH pc/board8521d/board85xx/board53/board55/board65

### Compiler ###
if(${ARCH} STREQUAL "board8521d")
	set(CMAKE_C_COMPILER arm-xmv2-linux-gcc)
	set(CMAKE_CXX_COMPILER arm-xmv2-linux-g++)
	set(CHIP 8521D)
	set(Lib_ROOT lib/${CHIP})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARCHBOARD -DBOARD8521")
elseif(${ARCH} STREQUAL "board8520dv200")
	set(CMAKE_C_COMPILER arm-xm-linux-gcc)
	set(CMAKE_CXX_COMPILER arm-xm-linux-g++)
	set(CHIP 8520DV200)
	set(Lib_ROOT lib/${CHIP})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARCHBOARD -DBOARD8520DV200")
elseif(${ARCH} STREQUAL "board650v200")
	set(CMAKE_C_COMPILER arm-xm-linux-gcc)
	set(CMAKE_CXX_COMPILER arm-xm-linux-g++)
	set(CHIP 650V200)
	set(Lib_ROOT lib/${CHIP})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARCHBOARD -DBOARD650V200")
elseif(${ARCH} STREQUAL "pc")
	set(Lib_ROOT lib_linux)
endif()

### debug log ###
message("=== source root=${SAMPLE_SOURCE_ROOT}")
message("=== c compiler=${CMAKE_C_COMPILER}")
message("=== cxx compiler=${CMAKE_CXX_COMPILER}")
message("=== arch=${ARCH}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=neon -mno-unaligned-access -fno-aggressive-loop-optimizations -flax-vector-conversions -fsigned-char -fopenmp -lstdc++ -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

set(CMAKE_C_FLAGS_RELEASE "-s")
set(CMAKE_CXX_FLAGS_RELEASE "-s")

include_directories(${SAMPLE_SOURCE_ROOT}/GUI/porting)
include_directories(${SAMPLE_SOURCE_ROOT}/GUI)
include_directories(${SAMPLE_SOURCE_ROOT}/network)
include_directories(${SAMPLE_SOURCE_ROOT}/cJson)
include_directories(${SAMPLE_SOURCE_ROOT})
include_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/include)
include_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/lvgl-8.2.0_spi)
include_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/lvgl-8.2.0_spi/src)
include_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/include/${CHIP}/mpp)
include_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/include/${CHIP}/smart)
include_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/include/${CHIP}/usb)
include_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/include/${CHIP}/fvideo)
include_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/include/${CHIP}/isp)
include_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/include/${CHIP}/comm)

file(GLOB SAMPLE_SOURCE_FILES "*.cpp" "GUI/*.cpp" "cJson/*.cpp" "GUI/porting/*.cpp" "network/*.cpp" "network/tinyxml/*.cpp" "periphery/*.cpp" "log/*.cpp" "usbdev/*.cpp" "mpp/*.cpp" )

message("=== sample files=${SAMPLE_SOURCE_FILES}")

link_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/${Lib_ROOT})
link_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/${Lib_ROOT}/ia)
link_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/${Lib_ROOT}/isp)

if(USE_TS)
    link_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/${Lib_ROOT}/ffmpeg_ts_no_playback)
    set(Lib_NAME XM_MiddleWare_api)
else()
    link_directories(${SAMPLE_SOURCE_ROOT}/../middlewaresdk/${Lib_ROOT}/ffmpeg_mp4_no_playback)
    set(Lib_NAME XM_MiddleWare_api_mp4)
endif()

add_executable (car_demo ${SAMPLE_SOURCE_FILES})

if(CT317)
	add_definitions(-DCT317)
endif(CT317)

if(${ARCH} STREQUAL "board8521d")
target_link_libraries (car_demo 
    ${Lib_NAME} 
    xmia 
    rtc 
    xmia_kwr 
    xmia_vad 
    fftw3f 
    fdr
    fr
    facnn
    fpcnn
    xmia_fd_cnn
    xmia_adas 
    xmia_td 
    xmia_spc
    xmia_comm
    npu
    ive
    mpi 
    venc
    vdec 
    h265e 
    h265d 
    audio 
    fvideo 
    dvr 
    xmsdk
	avformat 
	avcodec 
	swscale 
	swresample 
	avutil
    json 
    pthread 
    m 
	z
    lvgl)
elseif(${ARCH} STREQUAL "board8520dv200")
target_link_libraries (car_demo 
    ${Lib_NAME}  
	xmaudio3a
    xmia 
    rtc 
    xmia_kwr 
    xmia_vad 
    fftw3f 
    fdr
    fr
    facnn
    fpcnn
    xmia_fd_cnn
    xmia_adas 
    xmia_td 
    xmia_spc
    xmia_comm
    npu
    dpu
    smpi
    ive
    mpi 
    venc
    vdec 
    h265e 
    h265d 
    audio 
    fvideo 
    dvr 
    xmsdk
    xmjpege
	avformat 
	avcodec 
	swscale 
	swresample 
	avutil
    json 
    pthread 
    m 
	z
    lvgl)
elseif(${ARCH} STREQUAL "board650v200")
target_link_libraries (car_demo 
    ${Lib_NAME} 
    xmipprotocol
    xmia 
    xmia_kwr
    fftw3f 
    fr
    xmia_comm
    xmrtsp
    npu
    dpu
    ad_m320
    ispapp
    sns_XAx
    phyvi
    autoscene_fv
    fvideo
    minIni
    sdnc
    isp
    ae 
    awb
    mpi_all 
    rtc_16K 
    venc
    vdec
    h265e 
    h265d 
    x264_weak
    jpegenc
    jpegdec
    xmaudio3a
    sda
    xmsdk
	zzip
    avformat 
	avcodec 
	swscale 
	swresample 
	avutil
    json 
    ive
    iconv
    jpeg
    pthread 
    m 
	z
	cr
    lvgl_CT317
    SunAlgorithm)
endif()