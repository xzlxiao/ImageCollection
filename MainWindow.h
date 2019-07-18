/***************************************
//        .==.        .==.
//       //`^\\      //^`\\
//      // ^ ^\(\__/)/^ ^^\\
//     //^ ^^ ^/+  0\ ^^ ^ \\
//    //^ ^^ ^/( >< )\^ ^ ^ \\
//   // ^^ ^/\| v''v |/\^ ^ ^\\
//  // ^^/\/ /  `~~`  \ \/\^ ^\\
//  ----------------------------
// BE CAREFULL! THERE IS A DRAGONPIG.
//
// 功能：主窗口
// 模块：class MainWindow
// (c) 肖镇龙 2018
// 2018-05-09 肖镇龙 init
// 2018-08-09 肖镇龙 realsense控制代码
// 2018-08-17 肖镇龙 realsense基本接口，多线程等
// 2018-08-17 肖镇龙 深度图像和彩色图像对齐
***************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "DeepCameraInterface.h"
#include <QSettings>
#include <QPaintEvent>
#include <QLabel>
#include <QPainter>
#include <QFrame>
#include <QMutex>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QStringList>
#include <QDir>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadWindow();

    DeepCameraInterface *mDeepCam;  // realsense代理
    QThread mThreadDeepCam;         // mDeepCam的管理线程
    cv::Mat *mRgb;
    cv::Mat *mDepthColor;
    cv::Mat *mEclipse;
    cv::Mat *mAlighDepthColor;
    cv::Mat *mAlignRgb;
    cv::Mat *mAlignDepth;
    cv::Mat *mDepth;
    QMutex locker;
    QTimer *autoTakePicture;
    QSettings *mSetting;
    int autoTakePictureCount;

    cv::Rect getShowRect(const cv::Mat &src_image, const int &width_crop, const int &height_crop);
    void resizeImage(cv::Mat &image_, const int &width_crop, const int &height_crop);
    void displayImageInQLabel(const cv::Mat &image, QFrame &label);
    void displayDepthInQLabel(const cv::Mat &image, QFrame &label, QPainter &painter);
    void loadThread();
    void paintEvent(QPaintEvent *evet);
    void loadSettings();
    void saveSettings();

    QStringList mLight = {
        "default",
        "bright",
        "normal",
        "dark"
    };

    QStringList mGlasses = {
        "default",
        "yes",
        "no"
    };

    QStringList mPosture = {
        "default",
        "yes",
        "no"
    };

    QStringList mExpression = {
        "default",
        "yes",
        "no"
    };

private: //控件
    QComboBox *mComboBoxLight;
    QComboBox *mComboBoxGlasses;
    QComboBox *mComboBoxPosture;
    QComboBox *mComboBoxExpression;
    QLabel *mLbCurrentId;
    QLineEdit *mLineEditSubjectID;
    QLineEdit *mLineEditSaveDir;
    QMenuBar *mMenuBar;
    QMenu *mFileMenu;
    QAction *mActionSetSaveDir;

public: //set get
    void setSaveDir(const QString &dir);

    QString getSaveImageName();

private:
    int mDataId;
    QString mSaveDir;

private:
    Ui::MainWindow *ui;
signals:
    void signalOpenCamera();
public slots:
    void updateCameraCvImageSlot(const DeepCamData &image_data);
private slots:
    void on_btnTakePhoto_clicked();
    void slotSetSaveDir();
    void on_btnAutoTakePhoto_clicked();
    void on_btnStop_clicked();
    void autoTakePictures();
};

#endif // MAINWINDOW_H
