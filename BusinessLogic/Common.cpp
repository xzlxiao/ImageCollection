#include "Common.h"
using namespace std;
using namespace cv;

Common::Common()
{
    
}
std::vector<std::string> Common::MODE_NAME = { "Startup", "Detection", "Collect Faces", "Training", "Recognition", "Delete All", "ERROR!" };

string Common::getTime()
{
    MyDebug;
    time_t tt = time(NULL);//这句返回的只是一个时间cuo
    tm* t= localtime(&tt);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d_%H-%M-%S", t);
    string time = tmp;
    return time;
}

QString Common::qGetTime()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd_hh-mm-ss");
    return current_date;
}

QString Common::qGetTime2()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("hh-mm-ss");
    return current_date;
}
// Compare two images by getting the L2 error (square-root of sum of squared error).
///
/// \brief Common::getSimilarity 比较两图片的差异
/// \param A
/// \param B
/// \return 数值越大，差异越大
///
double Common::getSimilarity(const Mat &A, const Mat &B)
{
    if (A.rows > 0 && A.rows == B.rows && A.cols > 0 && A.cols == B.cols)
    {
        // Calculate the L2 relative error between the 2 images.
        double errorL2 = norm(A, B, NORM_L2);
        // Convert to a reasonable scale, since L2 error is summed across all pixels of the image.
        double similarity = errorL2 / (double)(A.rows * A.cols);
        return similarity;
    }
    else
    {
        //cout << "WARNING: Images have a different size in 'getSimilarity()'." << endl;
        return 100000000.0;  // Return a bad value
    }
}

void Common::Mat2QImage(const Mat &cvImg, QImage &qImage_out)
{
    //MyDebug;
    Mat invert_img;
    if(cvImg.channels()==3)                             //3 channels color image
    {

        cv::cvtColor(cvImg,invert_img,COLOR_BGR2RGB);
        qImage_out =QImage((const unsigned char*)(invert_img.data),
                           invert_img.cols, invert_img.rows,
                           invert_img.cols*invert_img.channels(),
                           QImage::Format_RGB888);
    }
    else if(cvImg.channels()==1)                    //grayscale image
    {
        //        qImage_out =QImage((const unsigned char*)(cvImg.data),
        //                           cvImg.cols,cvImg.rows,
        //                           cvImg.cols*cvImg.channels(),
        //                           QImage::Format_Indexed8);
        qImage_out = QImage(cvImg.cols, cvImg.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        qImage_out.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            qImage_out.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = cvImg.data;
        for(int row = 0; row < cvImg.rows; row ++)
        {
            uchar *pDest = qImage_out.scanLine(row);
            memcpy(pDest, pSrc, cvImg.cols);
            pSrc += cvImg.step;
        }


    }
    else
    {
        qImage_out =QImage((const unsigned char*)(cvImg.data),
                           cvImg.cols,cvImg.rows,
                           cvImg.cols*cvImg.channels(),
                           QImage::Format_RGB888);
    }
}

QImage CortexBot::Mat2QImage(cv::Mat cvImg)
{
    QImage qImg;
    if(cvImg.channels()==3)                             //3 channels color image
    {

        cv::cvtColor(cvImg,cvImg,COLOR_BGR2RGB);
        qImg =QImage((const unsigned char*)(cvImg.data),
                     cvImg.cols, cvImg.rows,
                     cvImg.cols*cvImg.channels(),
                     QImage::Format_RGB888);
    }
    else if(cvImg.channels()==1)                    //grayscale image
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                     cvImg.cols,cvImg.rows,
                     cvImg.cols*cvImg.channels(),
                     QImage::Format_Indexed8);
    }
    else
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                     cvImg.cols,cvImg.rows,
                     cvImg.cols*cvImg.channels(),
                     QImage::Format_RGB888);
    }

    return qImg;
}

///
/// \brief ToolCoordinate::operator = 赋值
/// \param robotStates 从机械臂获得的参数，double数组
///
ToolCoordinate::ToolCoordinate(const ToolCoordinate &other_)
{
    this->x = other_.x;
    this->y = other_.y;
    this->z = other_.z;
    this->Rx = other_.Rx;
    this->Ry = other_.Ry;
    this->Rz = other_.Rz;

    this->x_speed = other_.x_speed;
    this->y_speed = other_.y_speed;
    this->z_speed = other_.z_speed;
    this->Rx_speed = other_.Rx_speed;
    this->Ry_speed = other_.Ry_speed;
    this->Rz_speed = other_.Rz_speed;
}

void ToolCoordinate::operator=(const ToolCoordinate &other_)
{
    this->x = other_.x;
    this->y = other_.y;
    this->z = other_.z;
    this->Rx = other_.Rx;
    this->Ry = other_.Ry;
    this->Rz = other_.Rz;

    this->x_speed = other_.x_speed;
    this->y_speed = other_.y_speed;
    this->z_speed = other_.z_speed;
    this->Rx_speed = other_.Rx_speed;
    this->Ry_speed = other_.Ry_speed;
    this->Rz_speed = other_.Rz_speed;
}

void ToolCoordinate::operator=(const double *robotState)
{
    if (robotState != nullptr)
    {
        x = robotState[12];
        y = robotState[13];
        z = robotState[14];
        Rx = robotState[15];
        Ry = robotState[16];
        Rz = robotState[17];

        x_speed = robotState[18];
        y_speed = robotState[19];
        z_speed = robotState[20];
        Rx_speed = robotState[21];
        Ry_speed = robotState[22];
        Rz_speed = robotState[23];
    }

}

void ToolCoordinate::operator=(const std::vector<double> &robotStates)
{
    x = robotStates.at(12);
    y = robotStates.at(13);
    z = robotStates.at(14);
    Rx = robotStates.at(15);
    Ry = robotStates.at(16);
    Rz = robotStates.at(17);

    x_speed = robotStates.at(18);
    y_speed = robotStates.at(19);
    z_speed = robotStates.at(20);
    Rx_speed = robotStates.at(21);
    Ry_speed = robotStates.at(22);
    Rz_speed = robotStates.at(23);
}

void ToolCoordinate::operator=(const initializer_list<double> &robotState)
{
    x = robotState.begin()[12];
    y = robotState.begin()[13];
    z = robotState.begin()[14];
    Rx = robotState.begin()[15];
    Ry = robotState.begin()[16];
    Rz = robotState.begin()[17];

    x_speed = robotState.begin()[18];
    y_speed = robotState.begin()[19];
    z_speed = robotState.begin()[20];
    Rx_speed = robotState.begin()[21];
    Ry_speed = robotState.begin()[22];
    Rz_speed = robotState.begin()[23];
}

double ToolCoordinate::getX() const
{
    return this->x;
}

double ToolCoordinate::getY() const
{
    return this->y;
}

double ToolCoordinate::getZ() const
{
    return this->z;
}

double ToolCoordinate::getRx() const
{
    return this->Rx;
}

double ToolCoordinate::getRy() const
{
    return this->Ry;
}

double ToolCoordinate::getRz() const
{
    return this->Rz;
}

double ToolCoordinate::getXSpeed() const
{
    return this->x_speed;
}

double ToolCoordinate::getYSpeed() const
{
    return this->y_speed;
}

double ToolCoordinate::getZSpeed() const
{
    return this->z_speed;
}

double ToolCoordinate::getRxSpeed() const
{
    return this->Rx_speed;
}

double ToolCoordinate::getRySpeed() const
{
    return this->Ry_speed;
}

double ToolCoordinate::getRzSpeed() const
{
    return this->Rz_speed;
}

void ToolCoordinate::Print()
{
    qDebug() << x << endl;
    qDebug() << y << endl;
    qDebug() << z << endl;
    qDebug() << Rx << endl;
    qDebug() << Ry << endl;
    qDebug() << Rz << endl;

    qDebug() << x_speed << endl;
    qDebug() << y_speed << endl;
    qDebug() << z_speed << endl;
    qDebug() << Rx_speed << endl;
    qDebug() << Ry_speed << endl;
    qDebug() << Rz_speed << endl;
}

void ToolCoordinate::clear()
{
    x = 0;
    y = 0;
    z = 0;
    Rx = 0;
    Ry = 0;
    Rz = 0;

    x_speed = 0;
    y_speed = 0;
    z_speed = 0;
    Rx_speed = 0;
    Ry_speed = 0;
    Rz_speed = 0;
}

robotJoint::robotJoint(const robotJoint &other_)
{
    this->joint1 = other_.joint1;
    this->joint2 = other_.joint2;
    this->joint3 = other_.joint3;
    this->joint4 = other_.joint4;
    this->joint5 = other_.joint5;
    this->joint6 = other_.joint6;

    this->joint1_speed = other_.joint1_speed;
    this->joint2_speed = other_.joint2_speed;
    this->joint3_speed = other_.joint3_speed;
    this->joint4_speed = other_.joint4_speed;
    this->joint5_speed = other_.joint5_speed;
    this->joint6_speed = other_.joint6_speed;
}

void robotJoint::operator=(const robotJoint &other_)
{
    this->joint1 = other_.joint1;
    this->joint2 = other_.joint2;
    this->joint3 = other_.joint3;
    this->joint4 = other_.joint4;
    this->joint5 = other_.joint5;
    this->joint6 = other_.joint6;

    this->joint1_speed = other_.joint1_speed;
    this->joint2_speed = other_.joint2_speed;
    this->joint3_speed = other_.joint3_speed;
    this->joint4_speed = other_.joint4_speed;
    this->joint5_speed = other_.joint5_speed;
    this->joint6_speed = other_.joint6_speed;
}

void robotJoint::operator=(const double *robotStates)
{
    if (robotStates != nullptr)
    {
        joint1 = robotStates[0];
        joint2 = robotStates[1];
        joint3 = robotStates[2];
        joint4 = robotStates[3];
        joint5 = robotStates[4];
        joint6 = robotStates[5];

        joint1_speed = robotStates[6];
        joint2_speed = robotStates[7];
        joint3_speed = robotStates[8];
        joint4_speed = robotStates[9];
        joint5_speed = robotStates[10];
        joint6_speed = robotStates[11];
    }

}

void robotJoint::operator=(const std::vector<double> &robotStates)
{
    joint1 = robotStates.at(0);
    joint2 = robotStates.at(1);
    joint3 = robotStates.at(2);
    joint4 = robotStates.at(3);
    joint5 = robotStates.at(4);
    joint6 = robotStates.at(5);

    joint1_speed = robotStates.at(6);
    joint2_speed = robotStates.at(7);
    joint3_speed = robotStates.at(8);
    joint4_speed = robotStates.at(9);
    joint5_speed = robotStates.at(10);
    joint6_speed = robotStates.at(11);
}

void robotJoint::operator=(const initializer_list<double> &robotState)
{
    joint1 = robotState.begin()[0];
    joint2 = robotState.begin()[1];
    joint3 = robotState.begin()[2];
    joint4 = robotState.begin()[3];
    joint5 = robotState.begin()[4];
    joint6 = robotState.begin()[5];

    joint1_speed = robotState.begin()[6];
    joint2_speed = robotState.begin()[7];
    joint3_speed = robotState.begin()[8];
    joint4_speed = robotState.begin()[9];
    joint5_speed = robotState.begin()[10];
    joint6_speed = robotState.begin()[11];

}

double robotJoint::getJoint1() const
{
    return this->joint1;
}

double robotJoint::getJoint2() const
{
    return joint2;
}

double robotJoint::getJoint3() const
{
    return joint3;
}

double robotJoint::getJoint4() const
{
    return joint4;
}

double robotJoint::getJoint5() const
{
    return joint5;
}

double robotJoint::getJoint6() const
{
    return joint6;
}

double robotJoint::getJoint1Speed() const
{
    return joint1_speed;
}

double robotJoint::getJoint2Speed() const
{
    return joint2_speed;
}

double robotJoint::getJoint3Speed() const
{
    return joint3_speed;
}

double robotJoint::getJoint4Speed() const
{
    return joint4_speed;
}

double robotJoint::getJoint5Speed() const
{
    return joint5_speed;
}

double robotJoint::getJoint6Speed() const
{
    return joint6_speed;
}

void robotJoint::clear()
{
    joint1=0;
    joint2=0;
    joint3=0;
    joint4=0;
    joint5=0;
    joint6=0;

    joint1_speed=0;
    joint2_speed=0;
    joint3_speed=0;
    joint4_speed=0;
    joint5_speed=0;
    joint6_speed=0;
}
