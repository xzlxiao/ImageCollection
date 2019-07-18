#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMetaType>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>



using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mDeepCam(new DeepCameraInterface),
    mRgb(nullptr),
    mDepthColor(nullptr),
    mEclipse(nullptr),
    mAlighDepthColor(nullptr),
    mSetting(new QSettings( QCoreApplication::applicationDirPath()+"/settings.ini", QSettings::IniFormat)),
    ui(new Ui::MainWindow)
{
    autoTakePicture=new QTimer(this);
    autoTakePictureCount=0;
    qRegisterMetaType<DeepCamData>("DeepCamData"); //
    ui->setupUi(this);
    loadWindow();
    loadSettings();
    connect(mDeepCam, SIGNAL(signalCameraImageData(DeepCamData)), this, SLOT(updateCameraCvImageSlot(DeepCamData)));//
    connect(&mThreadDeepCam, &QThread::finished, mDeepCam, &QObject::deleteLater); //线程结束时销毁deep_camera
    connect(this, SIGNAL(signalOpenCamera()), mDeepCam, SLOT(slotOpenCamera()));//
    connect(mActionSetSaveDir, SIGNAL(triggered(bool)), this, SLOT(slotSetSaveDir()));
    connect(autoTakePicture,SIGNAL(timeout()),this,SLOT(autoTakePictures()));
    loadThread();   //
    mThreadDeepCam.start();//
    ui->btnStop->setEnabled(false);
    emit signalOpenCamera();
}

MainWindow::~MainWindow()
{
    DeepCameraInterface::isCameraNotUsed = true;
    mThreadDeepCam.quit();
    mThreadDeepCam.wait();
    saveSettings();
    delete ui;
}

void MainWindow::loadWindow()
{
    mComboBoxExpression = ui->comboBoxExpression;
    mComboBoxGlasses = ui->comboBoxGlasses;
    mComboBoxLight = ui->comboBoxLight;
    mComboBoxPosture = ui->comboBoxPosture;
    mLineEditSubjectID = ui->lineEditSubjectID;
    mLbCurrentId = ui->lbCurrentId;
    mLineEditSaveDir = ui->lineEditSaveDir;
    mMenuBar = ui->menubar;
    mFileMenu = new QMenu("文件", this);
    mActionSetSaveDir = new QAction("设置储存地址", this);
    mMenuBar->addMenu(mFileMenu);
    mFileMenu->addAction(mActionSetSaveDir);
    mMenuBar->show();
}

///
/// \brief 按width_crop/height_crop的比例裁剪图片最大范围
/// \param src_image
/// \param width_crop
/// \param height_crop
/// \return
///
cv::Rect MainWindow::getShowRect(const Mat &src_image, const int &width_crop, const int &height_crop)
{
    //MyDebug;
    double gui_wh_ratio, img_wh_ratio;
    cv::Rect ret;
    gui_wh_ratio = (double)width_crop / (double)height_crop;
    img_wh_ratio = (double)src_image.size().width / (double)src_image.size().height;
    if (gui_wh_ratio > img_wh_ratio)
    {
        ret.width = src_image.size().width;
        ret.height = (double)(((double)ret.width / (double)width_crop) * (double)height_crop);
        ret.x = 0;
        ret.y = (int)floor(((double)src_image.size().height - (double)ret.height) / 2.0);
    }
    else
    {
        ret.height = src_image.size().height;
        ret.width = (int)(((double)ret.height / (double)height_crop) * (double)width_crop);

        ret.x = (int)floor(((double)src_image.size().width - (double)ret.width) / 2.0);
        ret.y = 0;
    }
    return ret;
}

///
/// \brief 按width_crop/height_crop的比例裁剪图片最大范围，并缩放到对应大小
/// \param image_
/// \param width_crop
/// \param height_crop
///
void MainWindow::resizeImage(Mat &image_, const int &width_crop, const int &height_crop)
{
    //MyDebug;
    image_ = image_(getShowRect(image_, width_crop,height_crop));
    cv::resize(image_,image_, cv::Size(width_crop, height_crop));
}

///
/// \brief 在label控件范围内绘制图像image
/// \param image    待绘制图片
/// \param label    绘制图片控件位置，需保证label是MainWindow的子控件
///
void MainWindow::displayImageInQLabel(const Mat &image, QFrame &label)
{
    QPainter painter(this);
    cv::Mat image_tmp;
    QImage q_rgb;
    if (!image.empty())
    {
        image.copyTo(image_tmp);
        resizeImage(image_tmp, label.width()-2, label.height()-2);
        Common::Mat2QImage(image_tmp, q_rgb);
        painter.drawImage(label.pos().x()+1, label.pos().y()+1,q_rgb);
    }
}

///
/// \brief 显示深度图像，有严重bug
/// \param image
/// \param label
/// \param painter
///
void MainWindow::displayDepthInQLabel(const Mat &image, QFrame &label, QPainter &painter)
{
    locker.lock();
    cv::Mat image_tmp;
    QImage q_rgb;
    if (!image.empty())
    {
        image.convertTo(image_tmp, CV_8UC1);
        //resizeImage(image_tmp, label.width()-2, label.height()-2);
        Common::Mat2QImage(image_tmp, q_rgb);
        painter.drawImage(label.pos().x()+1, label.pos().y()+1,q_rgb);
    }
    locker.unlock();
}

void MainWindow::loadThread()
{
    mDeepCam->moveToThread(&mThreadDeepCam);
}

void MainWindow::paintEvent(QPaintEvent *evet)
{

    if (mRgb)
    {

        displayImageInQLabel(*mRgb, *ui->frameRgb);
        //displayDepthInQLabel(*mDepth, *ui->frameDepth, painter);
        displayImageInQLabel(*mDepthColor, *ui->frameDepthColor);
        displayImageInQLabel(*mAlighDepthColor, *ui->frameAlignDepthColor);
        displayImageInQLabel(*mEclipse, *ui->frameDepthEclipse);
    }
}

void MainWindow::loadSettings()
{
    if(mSetting->contains("Config/DataID"))
    {
        mDataId = mSetting->value("Config/DataID").toInt();
        mSaveDir = mSetting->value("Config/SaveDir").toString();
    }
    else
    {
        mSetting->beginGroup("Config");
        mSetting->setValue("DataID", "1");
        mDataId = 1;
        mSetting->setValue("SaveDir", QCoreApplication::applicationDirPath()+"/image/");
        mSaveDir = QCoreApplication::applicationDirPath()+"/image/";
        mSetting->endGroup();
    }
    mLbCurrentId->setText(QString("%1").arg(mDataId, 4, 10, QChar('0')));
    mComboBoxExpression->addItems(mExpression);
    mComboBoxGlasses->addItems(mGlasses);
    mComboBoxLight->addItems(mLight);
    mComboBoxPosture->addItems(mPosture);
    mLineEditSaveDir->setText(mSaveDir);

}

void MainWindow::saveSettings()
{
    mSetting->setValue("Config/DataID", mDataId);
    mSetting->setValue("Config/SaveDir", mSaveDir);
}

void MainWindow::setSaveDir(const QString &dir)
{
    mSaveDir = dir;
    mSetting->setValue("Config/SaveDir", mSaveDir);
    mLineEditSaveDir->setText(mSaveDir);
}

///
/// \brief 获得除主目录和文件后缀名外的子文件夹和文件名
/// \return 字符串（例）：CB0003/CB0003_exprdefault_glassdefault_lightdefault_postdefault_01-30-32
///
QString MainWindow::getSaveImageName()
{
    return QString("CB%1/CB%2_%3_%4_%5_%6_%7")
            .arg(mDataId, 4, 10, QChar('0'))
            .arg(mDataId, 4, 10, QChar('0'))
            .arg("expr"+mComboBoxExpression->currentText())
            .arg("glass"+mComboBoxGlasses->currentText())
            .arg("light"+mComboBoxLight->currentText())
            .arg("post"+mComboBoxPosture->currentText())
            .arg(Common::qGetTime2());
}

///
/// \brief 从mDeepCam接收信号，更新摄像头的数据
/// \param image_data
///
void MainWindow::updateCameraCvImageSlot(const DeepCamData &image_data)
{
//    namedWindow("test");
//    if (!(*image_data.mCvRgb).empty())
//    {
//        imshow("test", (*image_data.mCvRgb));
//        waitKey(0);
//    }
    mRgb = image_data.mCvRgb;
    mDepthColor = image_data.mCvDepthColor;
    mAlighDepthColor = image_data.mCvAlignDepthColor;
    mEclipse = image_data.mCvEclipse;
    mAlignRgb = image_data.mCvAlignRgb;
    mAlignDepth = image_data.mCvAlignDepth;
    mDepth = image_data.mCvDepth;
    this->update();

}

void MainWindow::on_btnTakePhoto_clicked()
{
    ui->btnAutoTakePhoto->setDisabled(true);
    ui->btnFinish->setDisabled(true);
    ui->btnStop->setDisabled(true);
    ui->btnTakePhoto->setDisabled(true);
    ui->btnTakeVideo->setDisabled(true);
    QString filepath;
    QString save_name = mSaveDir + getSaveImageName();
    filepath = save_name + "rgb.tif";
    QFileInfo fileinfo(filepath);
    if (!fileinfo.absoluteDir().exists())
    {
        if (!fileinfo.absoluteDir().mkpath(fileinfo.absolutePath()))
        {
            QMessageBox::about(this, "错误", "创建文件夹失败");
            return;
        }
    }
    if (!cv::imwrite(filepath.toStdString(), *mRgb))
    {
        QMessageBox::warning(this, "错误", "保存失败");
    }
    filepath = save_name + "Align.tif";
    if (!cv::imwrite(filepath.toStdString(), *mAlignRgb))
    {
        QMessageBox::warning(this, "错误", "保存失败");
    }
    filepath = save_name + "depth.xml";
    FileStorage fs(filepath.toStdString(),FileStorage::WRITE);
    fs << "Matrix" << *mDepth;
    fs.release();
    filepath = save_name + "Align.xml";
    FileStorage fs2(filepath.toStdString(),FileStorage::WRITE);
    fs2 << "Matrix" << *mAlignDepth;
    fs2.release();
    ui->btnAutoTakePhoto->setDisabled(false);
    ui->btnFinish->setDisabled(false);
    ui->btnStop->setDisabled(false);
    ui->btnTakePhoto->setDisabled(false);
    ui->btnTakeVideo->setDisabled(false);
}

void MainWindow::slotSetSaveDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "储存目录设置", "./");
    dir = dir + "/";
    setSaveDir(dir);
}

void MainWindow::on_btnAutoTakePhoto_clicked()
{
    autoTakePicture->start(250);
    ui->btnTakePhoto->setEnabled(false);
    ui->btnStop->setEnabled(true);
    ui->btnFinish->setEnabled(false);
    ui->btnTakeVideo->setEnabled(false);
}


void MainWindow::on_btnStop_clicked()
{
    autoTakePicture->stop();
    ui->btnTakePhoto->setEnabled(true);
    ui->btnStop->setEnabled(false);
    ui->btnFinish->setEnabled(true);
    ui->btnTakeVideo->setEnabled(true);
}

void MainWindow::autoTakePictures()
{
    QString filepath;
    QString save_name = mSaveDir + getSaveImageName();
    filepath = save_name + "rgb.tif";
    QFileInfo fileinfo(filepath);
    if (!fileinfo.absoluteDir().exists())
    {
        if (!fileinfo.absoluteDir().mkpath(fileinfo.absolutePath()))
        {
            QMessageBox::about(this, "错误", "创建文件夹失败");
            return;
        }
    }
    if (!cv::imwrite(filepath.toStdString(), *mRgb))
    {
        QMessageBox::warning(this, "错误", "保存失败");
    }
    filepath = save_name + "Align.tif";
    if (!cv::imwrite(filepath.toStdString(), *mAlignRgb))
    {
        QMessageBox::warning(this, "错误", "保存失败");
    }
    filepath = save_name + "depth.xml";
    FileStorage fs(filepath.toStdString(),FileStorage::WRITE);
    fs << "Matrix" << *mDepth;
    fs.release();
    filepath = save_name + "Align.xml";
    FileStorage fs2(filepath.toStdString(),FileStorage::WRITE);
    fs2 << "Matrix" << *mAlignDepth;
    fs2.release();

}
