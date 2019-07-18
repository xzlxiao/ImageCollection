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
// 功能：深度摄像头
// 模块：class DeepCameraInterface
// (c) 肖镇龙 2018
// 2018-05-09 肖镇龙 init
// 2018-08-09 肖镇龙 realsense控制代码
// 2018-08-17 肖镇龙 realsense基本接口，多线程等
// 2018-08-17 肖镇龙 深度图像和彩色图像对齐
***************************************/
#ifndef DEEPCAMERAINTERFACE_H
#define DEEPCAMERAINTERFACE_H


#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <iostream>
#include <exception>
#include <QObject>
#include <QMutex>
#include <QImage>
#include <QThread>
#include "BusinessLogic/Common.h"
#include "BusinessLogic/debugprint.h"
#include <time.h>

struct DeepCamData
{
    rs2::pipeline *mPipe;
    rs2::frameset *mData;
    rs2::frame *mFrameDepth;
    rs2::frame *mFrameRgb;
    cv::Mat *mCvRgb;
    cv::Mat *mCvDepth;
    cv::Mat *mCvDepthColor;
    rs2::pipeline_profile *mProfile;
    cv::Mat *mCvAlignRgb;
    cv::Mat *mCvAlignDepth;
    cv::Mat *mCvAlignDepthColor;
    cv::Mat *mCvEclipse;
};

class DeepCameraInterface : public QObject
{
    Q_OBJECT
public slots:
    void slotOpenCamera();
public:
    explicit DeepCameraInterface(QObject *parent = nullptr);
    ~DeepCameraInterface();
    void __init__();

    void testOpenCamera();

    void static frameToMat(const rs2::frame &frame_in, cv::Mat &cv_image_out);
    void static depthFrameToMeters(const rs2::pipeline &pipe, const rs2::depth_frame &frame_in, cv::Mat &cv_image_out);
    void static depthFrameToColor(const rs2::depth_frame &frame_in, cv::Mat &cv_image_out);
    void waitForFrame();
    void collectFrameFromStream();
    void alignColorWithDepth();
    float getDepthScale(rs2::device dev);
    void removeBackground(rs2::video_frame& color_frame, const rs2::depth_frame& depth_frame, float depth_scale, float clipping_dist);
    void (*display)(const DeepCamData &data);
    QMutex mLocker;
    DeepCamData data;
    rs2::config mConfig;
private: //data
    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline_profile mProfile;
    rs2::pipeline mPipe;
    rs2::frameset mData;
    rs2::frame mFrameDepth;
    rs2::frame mFrameRgb;
    cv::Mat mCvRgb;
    cv::Mat mCvDepth;   // 以米为单位的深度数据
    cv::Mat mCvDepthColor;  // 以颜色表示的深度数据
    cv::Mat mCvAlignRgb;
    cv::Mat mCvAlignDepth;
    cv::Mat mCvAlignDepthColor;
    cv::Mat mCvEclipse;

private: //属性
    float mDepthScale;
    rs2::align mAlign;
    // Define a variable for controlling the distance to clip
public:
    static float mDepthClippingDistance;
    static float getDepthClippingDistance();
    static void setDepthClippingDistance(const float &distance);

public:
    static bool isCameraNotUsed;
signals:
    // 注：传递非标准库或非QT的的数据类型，需要在connect前进行注册
    void signalCameraImageData(const DeepCamData &image_data);
    void signalCameraImageData2(const cv::Mat &image_data);

private:
};



#endif // DEEPCAMERAINTERFACE_H
