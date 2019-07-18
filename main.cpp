/*************************************************************************
        .==.        .==.
       //`^\\      //^`\\
      // ^ ^\(\__/)/^ ^^\\
     //^ ^^ ^/+  0\ ^^ ^ \\
    //^ ^^ ^/( >< )\^ ^ ^ \\
   // ^^ ^/\| v''v |/\^ ^ ^\\
  // ^^/\/ /  `~~`  \ \/\^ ^\\
  ----------------------------
 BE CAREFULL! THERE IS A DRAGONPIG.

 File Name: main.cpp
 功能：
 模块：
 待实现功能：
 1 录像
 2 连续拍照
 3 结束录像或连续拍照，并保存
 4 多线程保存图像
 5 进度条提示保存进度
 (c) 肖镇龙
 Mail: xzl_xiao@163.com 
 Created Time: 三 10/17 21:33:06 2018
 ************************************************************************/

#include <QApplication>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <vector>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <iostream>
#include "MainWindow.h"

using namespace cv;



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

