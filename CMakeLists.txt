#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)
add_compile_options(-std=c++11)
#add_definitions(-std=c++11)
#set(CMAKE_C_COMPILER /usr/bin/gcc)

project(ImageCollection)

# 各个库的cmake编译命令
# cmake .. -DCMAKE_INSTALL_PREFIX=.         安装到当前目录
# make
# make install

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(RESOURCE_DIR resource/resource.qrc)
IF (APPLE)
    set(CMAKE_C_COMPILER "/usr/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
    set(Qt5_DIR /Users/xzlxiao/Qt/5.11.2/clang_64/lib/cmake/Qt5)
    if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
            set(OpenMP_C_FLAG "-fopenmp=libomp -Wno-unused-command-line-argument")
        endif()
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(OpenMP_CXX_FLAG "-fopenmp=libomp -Wno-unused-command-line-argument")
        endif()
ELSE()

ENDIF()

find_package(Qt5 REQUIRED Widgets)



#opencv3.4
set(OpenCV_DIR /usr/local/cortexbot/opencv-4.0.0/build)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#librealsense2
find_package(realsense2 REQUIRED)
#INCLUDE_DIRECTORIES(${realsense_INCLUDE_DIR})

#源代码路径
include_directories(

    )
set(Src
    DeepCameraInterface
    MainWindow
    MainWindow.ui
    BusinessLogic/Common
    BusinessLogic/debugprint
    )


#添加所有源代码文件
add_executable(ImageCollection
    main.cpp
    ${Src}
    )
target_link_libraries(ImageCollection
    Qt5::Widgets
    ${OpenCV_LIBS}
    ${realsense2_LIBRARY} 
)
