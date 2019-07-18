/***************************************
//        .==.        .==.
//       //`^\\      //^`\\
//      // ^ ^\(\__/)/^ ^^\\
//     //^ ^^ ^/+  0\ ^^ ^ \\
//    //^ ^^ ^/( >< )\^ ^ ^ \\
//   // ^^ ^/\| v''v |/\^ ^ ^\\
//  // ^^/\/ /  `~~`  \ \/\^ ^\\
//  ----------------------------
// BE CAREFULL! THERE IS A DRAGON.
//
// 功能：通用算法
// 模块：class Common
// (c) 肖镇龙 2018
// 2018-05-10 肖镇龙 init
// 2018-05-29 肖镇龙 迁移接口Mat2QImage
***************************************/
#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <time.h>
#include <string>
#include "debugprint.h"
#include <QString>
#include <QDateTime>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <initializer_list>


class Common
{
public:
    Common();
    static std::string getTime();
    static QString qGetTime();
    static QString qGetTime2();
    template <typename T> static std::string toString(T t);
    template <typename T> static T fromString(std::string t);
    static double getSimilarity(const cv::Mat &A, const cv::Mat &B);
    static std::vector<std::string> MODE_NAME;
    static void Mat2QImage(const cv::Mat &cvImg, QImage &qImage_out);
};

template <typename T> std::string Common::toString(T t)
{
    std::ostringstream out;
    out << t;
    return out.str();
}

template <typename T> T Common::fromString(std::string t)
{
    T out;
    std::istringstream in(t);
    in >> out;
    return out;
}

enum FISHERMODES
{
    MODE_STARTUP = 0,
    MODE_DETECTION,
    MODE_COLLECT_FACES,
    MODE_TRAINING,
    MODE_RECOGNITION,
    MODE_DELETE_ALL,
    MODE_END
};

enum FACE_OPTION
{
    FACE_FISHER_SINGLE = 0,     //只检测一张脸
    FACE_FISHER_MULTI           //检测多张脸
};

enum TCPSTATE
{
    TCP_NONE = 0,
    TCP_CONNECTING,
    TCP_CONNECTED,
    TCP_FAILED
};

namespace CortexBot
{
    QImage  Mat2QImage(cv::Mat cvImg);
}

class ToolCoordinate
{
public:
    double x;
    double y;
    double z;
    double Rx;
    double Ry;
    double Rz;

    double x_speed;
    double y_speed;
    double z_speed;
    double Rx_speed;
    double Ry_speed;
    double Rz_speed;
public:
    ToolCoordinate():x(0),y(0),z(0),Rx(0),Ry(0),Rz(0),x_speed(0),y_speed(0),z_speed(0),Rx_speed(0),Ry_speed(0),Rz_speed(0){}
    ToolCoordinate(const ToolCoordinate &other_);
    void operator= (const ToolCoordinate &other_);
    void operator= (const double *robotStates);
    void operator= (const std::vector<double> &robotStates);
    void operator= (const std::initializer_list<double> &robotState);
    double getX()const;
    double getY()const;
    double getZ()const;
    double getRx()const;
    double getRy()const;
    double getRz()const;
    double getXSpeed()const;
    double getYSpeed()const;
    double getZSpeed()const;
    double getRxSpeed()const;
    double getRySpeed()const;
    double getRzSpeed()const;
    void Print();
    void clear();
};

class robotJoint
{
public:
    double joint1;
    double joint2;
    double joint3;
    double joint4;
    double joint5;
    double joint6;

    double joint1_speed;
    double joint2_speed;
    double joint3_speed;
    double joint4_speed;
    double joint5_speed;
    double joint6_speed;
public:
    robotJoint():joint1(0),joint2(0),joint3(0),joint4(0),joint5(0),joint6(0),joint1_speed(0),joint2_speed(0),joint3_speed(0),joint4_speed(0),joint5_speed(0),joint6_speed(0){}
    robotJoint(const robotJoint &other_);
    void operator= (const robotJoint &other_);
    void operator= (const double *robotStates);
    void operator= (const std::vector<double> &robotStates);
    void operator= (const std::initializer_list<double> &robotState);
    double getJoint1()const;
    double getJoint2()const;
    double getJoint3()const;
    double getJoint4()const;
    double getJoint5()const;
    double getJoint6()const;
    double getJoint1Speed()const;
    double getJoint2Speed()const;
    double getJoint3Speed()const;
    double getJoint4Speed()const;
    double getJoint5Speed()const;
    double getJoint6Speed()const;
    void clear();
};

#endif // COMMON_H
