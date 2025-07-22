#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <QStatusBar>
#include <QLabel> // 添加 QLabel 头文件

// OpenCV 头文件
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

// 新增 OpenCV 模块头文件（根据功能需求）
#include <opencv2/stitching.hpp> // 图像拼接
#include <opencv2/photo/photo.hpp> // seamlessClone 等函数所需
// 如果人脸美颜功能需要更高级的人脸检测（例如Dlib或OpenCV的face模块），需要额外添加
// #include <opencv2/face.hpp> // 如果使用 face 模块进行地标检测等
// #include <opencv2/objdetect.hpp> // 如果使用 Haar/LBP 级联分类器进行人脸检测

// 前向声明 UI 类
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openImage();
    void sharpenImage();
    void convertToGrayscale();
    void performCannyEdgeDetection();
    void applyGammaCorrection();

    void openSecondImage();
    void swapImages(); // 交换主/第二图像的槽函数
    void stitchImages();
    void blendImages();
    void applyTextureTransfer();
    void applyFaceBeautify();

private:
    Ui::MainWindow *ui;
    cv::Mat originalImage;  // 第一张（主）图像
    cv::Mat secondImage;    // 第二张图像（用于拼接/融合等）
    cv::Mat processedImage; // 用于存储处理后的图像结果，避免覆盖 originalImage，除非是最终结果

    // 辅助函数
    QImage MatToQImage(const cv::Mat& mat);
    void displayImage(const QImage& image, QLabel* label);
    void updateButtonStates(); // 统一更新按钮状态的函数
};

#endif // MAINWINDOW_H
