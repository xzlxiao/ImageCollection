#include "DeepCameraInterface.h"
using namespace cv;
bool DeepCameraInterface::isCameraNotUsed = false;
float DeepCameraInterface::mDepthClippingDistance = 1.f;
DeepCameraInterface::DeepCameraInterface(QObject *parent) : QObject(parent),
    display(nullptr),
    mAlign(RS2_STREAM_COLOR)
{
    MyDebug;
    __init__();
}

DeepCameraInterface::~DeepCameraInterface()
{
    MyDebug;
    try
    {
        mPipe.stop();
    }
    catch(...)
    {}

}

void DeepCameraInterface::__init__()
{
    MyDebug;
    data.mProfile = &mProfile;
    data.mPipe = &mPipe;
    data.mData = &mData;
    data.mFrameDepth = &mFrameDepth;
    data.mFrameRgb = &mFrameRgb;
    data.mCvRgb = &mCvRgb;
    data.mCvDepth = &mCvDepth;
    data.mCvDepthColor = &mCvDepthColor;
    data.mCvAlignRgb = &mCvAlignRgb;
    data.mCvAlignDepth = &mCvAlignDepth;
    data.mCvAlignDepthColor = &mCvAlignDepthColor;
    data.mCvEclipse = &mCvEclipse;



}

void DeepCameraInterface::testOpenCamera()
{
    MyDebug;

    mProfile = mPipe.start();
    mDepthScale = getDepthScale(mProfile.get_device());
    while (!DeepCameraInterface::isCameraNotUsed)
    {
        clock_t start, end;
        start = clock();

        namedWindow("test");
        waitForFrame();
        collectFrameFromStream();
        alignColorWithDepth();
        //emit signalCameraImageData(data);
        __init__();
        //if (display != nullptr)
            //display(data);

        end = clock();
        double gaptime = (double)(end-start)/CLOCKS_PER_SEC;
        qDebug() << "耗时：" << gaptime << endl;
    }
}

void DeepCameraInterface::slotOpenCamera()
{
    MyDebug;
    mConfig.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 15);
    mConfig.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 15);
    while (!DeepCameraInterface::isCameraNotUsed)
    {
        try
        {
            mProfile = mPipe.start(mConfig);
            break;
        }
        catch(...)
        {
            qDebug() << "DeepCamera Init Failed";
        }
    }

    if (!DeepCameraInterface::isCameraNotUsed)
        mDepthScale = getDepthScale(mProfile.get_device());
    while (!DeepCameraInterface::isCameraNotUsed)
    {
        clock_t start, end;
        start = clock();

        waitForFrame();
        collectFrameFromStream();
        alignColorWithDepth();
        emit signalCameraImageData(data);
        //__init__();
        //if (display != nullptr)
            //display(data);
        end = clock();
        double gaptime = (double)(end-start)/CLOCKS_PER_SEC;
        qDebug() << "耗时：" << gaptime << endl;
    }
}


///
/// \brief DeepCameraInterface::frameToMat
/// \param frame_in
/// \param cv_image_out
/// \abstract Convert rs2::frame to cv::Mat
///
void DeepCameraInterface::frameToMat(const rs2::frame &frame_in, cv::Mat &cv_image_out)
{
    MyDebug;
    using namespace cv;
    using namespace rs2;

    auto vf = frame_in.as<video_frame>();
    const int w = vf.get_width();
    const int h = vf.get_height();

    if (frame_in.get_profile().format() == RS2_FORMAT_BGR8)
    {
        //cv_image_out = Mat(Size(w, h), CV_8UC3, (void*)frame_in.get_data(), Mat::AUTO_STEP);
        Mat(Size(w, h), CV_8UC3, (void*)frame_in.get_data(), Mat::AUTO_STEP)
                        .copyTo(cv_image_out);
    }
    else if (frame_in.get_profile().format() == RS2_FORMAT_RGB8)
    {
        auto r = Mat(Size(w, h), CV_8UC3, (void*)frame_in.get_data(), Mat::AUTO_STEP);

        //cv_image_out = r;
        r.copyTo(cv_image_out);
        cv::cvtColor(cv_image_out, cv_image_out, COLOR_BGR2RGB);
    }
    else if (frame_in.get_profile().format() == RS2_FORMAT_Z16)
    {
        //cv_image_out = Mat(Size(w, h), CV_16UC1, (void*)frame_in.get_data(), Mat::AUTO_STEP);
        Mat(Size(w, h), CV_16UC1, (void*)frame_in.get_data(), Mat::AUTO_STEP).copyTo(cv_image_out);
    }
    else if (frame_in.get_profile().format() == RS2_FORMAT_Y8)
    {
        //cv_image_out = Mat(Size(w, h), CV_8UC1, (void*)frame_in.get_data(), Mat::AUTO_STEP);
        Mat(Size(w, h), CV_8UC1, (void*)frame_in.get_data(), Mat::AUTO_STEP).copyTo(cv_image_out);
    }
    else
    {
        throw std::runtime_error("Frame format is not supported yet!");
    }


}

///
/// \brief DeepCameraInterface::depthFrameToMeters
/// \param pipe
/// \param frame_in
/// \param cv_image_out
/// \abstract Converts depth frame to a matrix of doubles with distances in meters
///
void DeepCameraInterface::depthFrameToMeters(const rs2::pipeline &pipe, const rs2::depth_frame &frame_in, cv::Mat &cv_image_out)
{
    //MyDebug;
    using namespace cv;
    using namespace rs2;

    frameToMat(frame_in, cv_image_out);
    cv_image_out.convertTo(cv_image_out, CV_64F);
    auto depth_scale = pipe.get_active_profile()
            .get_device()
            .first<depth_sensor>()
            .get_depth_scale();
    cv_image_out = cv_image_out * depth_scale;
}

void DeepCameraInterface::depthFrameToColor(const rs2::depth_frame &frame_in, cv::Mat &cv_image_out)
{
    //MyDebug;
    rs2::colorizer color_map;
    rs2::frame depth_color = color_map(frame_in);
    DeepCameraInterface::frameToMat(depth_color, cv_image_out);
}

void DeepCameraInterface::waitForFrame()
{
    MyDebug;
    this->mData = mPipe.wait_for_frames();  // Wait for next set of frames from the camera
}

void DeepCameraInterface::collectFrameFromStream()
{
    mLocker.lock();
    //MyDebug;
    this->mFrameDepth = mData.get_depth_frame();
    this->mFrameRgb = mData.get_color_frame();
    frameToMat(mFrameRgb, mCvRgb);
    depthFrameToMeters(mPipe, mFrameDepth, mCvDepth);
    depthFrameToColor(mFrameDepth, mCvDepthColor);
    mLocker.unlock();
}

void DeepCameraInterface::alignColorWithDepth()
{
    mLocker.lock();
    //Get processed aligned frame
    auto processed = mAlign.process(mData);

    // Trying to get both color and aligned depth frames
    rs2::video_frame color_frame = processed.first(RS2_STREAM_COLOR);
    rs2::depth_frame aligned_depth_frame = processed.get_depth_frame();
//    rs2::depth_frame aligned_depth_frame = processed.first(RS2_STREAM_DEPTH);
//    rs2::video_frame color_frame = processed.get_color_frame();
    //If one of them is unavailable, continue iteration
    if (!aligned_depth_frame || !color_frame)
    {
        return;
    }

    // Passing both frames to remove_background so it will "strip" the background
    // NOTE: in this example, we alter the buffer of the color frame, instead of copying it and altering the copy
    //       This behavior is not recommended in real application since the color frame could be used elsewhere
    frameToMat(color_frame, mCvAlignRgb);
    // 这一步是耗时且在实际应用中很可能没有必要的，所以如不是演示用，可以去除
    removeBackground(color_frame, aligned_depth_frame, mDepthScale, getDepthClippingDistance());
    frameToMat(color_frame, mCvEclipse);

    depthFrameToMeters(mPipe, aligned_depth_frame, mCvAlignDepth);
    depthFrameToColor(aligned_depth_frame, mCvAlignDepthColor);
    mLocker.unlock();
}

float DeepCameraInterface::getDepthScale(rs2::device dev)
{
    // Go over the device's sensors
    for (rs2::sensor& sensor : dev.query_sensors())
    {
        // Check if the sensor if a depth sensor
        if (rs2::depth_sensor dpt = sensor.as<rs2::depth_sensor>())
        {
            return dpt.get_depth_scale();
        }
    }
    throw std::runtime_error("Device does not have a depth sensor");
}

void DeepCameraInterface::removeBackground(rs2::video_frame &color_frame, const rs2::depth_frame &depth_frame, float depth_scale, float clipping_dist)
{
    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());
    uint8_t* p_color_frame = reinterpret_cast<uint8_t*>(const_cast<void*>(color_frame.get_data()));

    int width = color_frame.get_width();
    int height = color_frame.get_height();
    int color_bpp = color_frame.get_bytes_per_pixel();

    #pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 0; y < height; y++)
    {
        auto depth_pixel_index = y * width;
        for (int x = 0; x < width; x++, ++depth_pixel_index)
        {
            // Get the depth value of the current pixel
            auto pixels_distance = depth_scale * p_depth_frame[depth_pixel_index];

            // Check if the depth value is invalid (<=0) or greater than the threashold
            if (pixels_distance <= 0.f || pixels_distance > clipping_dist)
            {
                // Calculate the offset in color frame's buffer to current pixel
                auto offset = depth_pixel_index * color_bpp;

                // Set pixel to "background" color (0x999999)
                std::memset(&p_color_frame[offset], 0x99, color_bpp);
            }
        }
    }
}

float DeepCameraInterface::getDepthClippingDistance()
{
    return mDepthClippingDistance;
}

void DeepCameraInterface::setDepthClippingDistance(const float &distance)
{
    mDepthClippingDistance = distance;
}




