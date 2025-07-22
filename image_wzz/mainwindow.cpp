#include "mainwindow.h"
#include "ui_mainwindow.h" // 确保包含 ui_mainwindow.h

#include <QDebug>
#include <cmath> // 用于 pow 函数，尽管通常在 C++ 中包含 cmath

// =====================================================================
// MatToQImage 函数：将 OpenCV 的 cv::Mat 转换为 Qt 的 QImage
// 确保内部的颜色转换和步长转换已修正
// =====================================================================
QImage MainWindow::MatToQImage(const cv::Mat& mat)
{
    if (mat.empty()) {
        return QImage();
    }

    if (mat.type() == CV_8UC1) { // 灰度图像
        // 使用 Format_Grayscale8 格式
        QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8);
        return image.copy(); // 返回副本，因为 mat.data 指向的内存可能会被 OpenCV 更改
    } else if (mat.type() == CV_8UC3) { // 彩色图像 (BGR)
        // OpenCV 默认是 BGR 顺序，QImage 默认是 RGB，需要转换
        cv::Mat rgbMat;
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB); // 使用 cv::COLOR_BGR2RGB 替代 CV_BGR2RGB
        QImage image(rgbMat.data, rgbMat.cols, rgbMat.rows, static_cast<int>(rgbMat.step), QImage::Format_RGB888);
        return image.copy();
    } else if (mat.type() == CV_8UC4) { // RGBA 图像
        // 如果是RGBA，直接转换
        QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_ARGB32);
        return image.copy();
    }

    return QImage(); // 不支持的图像类型
}

// =====================================================================
// displayImage 函数：在指定的 QLabel 上显示 QImage
// = =====================================================================
void MainWindow::displayImage(const QImage& image, QLabel* label)
{
    if (label && !image.isNull()) {
        // 缩放图片以适应 QLabel 大小，并保持宽高比，平滑缩放
        label->setPixmap(QPixmap::fromImage(image).scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        label->setAlignment(Qt::AlignCenter); // 居中显示
    } else if (label) {
        label->clear(); // 如果图片为空，则清除 QLabel 显示
        // 根据是哪个标签，显示不同的默认文本
        if (label == ui->imageLabel) {
            label->setText("请加载第一张图像");
        } else if (label == ui->secondImageLabel) {
            label->setText("请加载第二张图像");
        }
        label->setAlignment(Qt::AlignCenter); // 确保默认文本也居中
    }
}

// =====================================================================
// updateButtonStates 函数：统一管理所有按钮的启用/禁用状态
// =====================================================================
void MainWindow::updateButtonStates()
{
    // 单图操作按钮 (需要 originalImage 存在)
    bool canOperateSingleImage = !originalImage.empty();
    ui->sharpenButton->setEnabled(canOperateSingleImage);
    ui->grayButton->setEnabled(canOperateSingleImage);
    ui->cannyButton->setEnabled(canOperateSingleImage);
    ui->gammaButton->setEnabled(canOperateSingleImage);
    ui->faceBeautifyButton->setEnabled(canOperateSingleImage);

    // 多图操作按钮 (需要 originalImage 和 secondImage 都存在)
    bool canOperateDualImage = !originalImage.empty() && !secondImage.empty();
    ui->stitchButton->setEnabled(canOperateDualImage);
    ui->blendButton->setEnabled(canOperateDualImage);
    ui->textureTransferButton->setEnabled(canOperateDualImage);
    ui->swapImagesButton->setEnabled(canOperateDualImage); // 交换按钮只在两张图都存在时启用
}

// =====================================================================
// 构造函数
// =====================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 连接所有按钮的信号与槽
    connect(ui->openButton, SIGNAL(clicked()), this, SLOT(openImage()));
    connect(ui->sharpenButton, SIGNAL(clicked()), this, SLOT(sharpenImage()));
    connect(ui->grayButton, SIGNAL(clicked()), this, SLOT(convertToGrayscale()));
    connect(ui->cannyButton, SIGNAL(clicked()), this, SLOT(performCannyEdgeDetection()));
    connect(ui->gammaButton, SIGNAL(clicked()), this, SLOT(applyGammaCorrection()));

    connect(ui->openSecondImageButton, SIGNAL(clicked()), this, SLOT(openSecondImage()));
    connect(ui->swapImagesButton, SIGNAL(clicked()), this, SLOT(swapImages())); // 连接交换按钮
    connect(ui->stitchButton, SIGNAL(clicked()), this, SLOT(stitchImages()));
    connect(ui->blendButton, SIGNAL(clicked()), this, SLOT(blendImages()));
    connect(ui->textureTransferButton, SIGNAL(clicked()), this, SLOT(applyTextureTransfer()));
    connect(ui->faceBeautifyButton, SIGNAL(clicked()), this, SLOT(applyFaceBeautify()));

    // 初始化状态栏
    if (!statusBar()) {
        setStatusBar(new QStatusBar(this));
    }

    // 调用更新函数，设置所有按钮的初始状态 (除 openButton 和 openSecondImageButton 外都禁用)
    updateButtonStates();
}

// =====================================================================
// 析构函数
// =====================================================================
MainWindow::~MainWindow()
{
    delete ui;
}

// =====================================================================
// openImage 槽函数：打开并显示第一张图像
// =====================================================================
/*
 void MainWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("选择图像"), "", tr("图像文件 (*.png *.jpg *.jpeg *.bmp)"));
    if (!fileName.isEmpty()) {
        cv::Mat loadedImage = cv::imread(fileName.toStdString());
        if (loadedImage.empty()) {
            QMessageBox::critical(this, tr("错误"), tr("无法加载图像!"));
            statusBar()->showMessage(tr("无法加载图像"), 3000);
            originalImage.release(); // 确保释放内存
            displayImage(QImage(), ui->imageLabel); // 清除显示
        } else {
            originalImage = loadedImage.clone(); // 复制图像数据
            displayImage(MatToQImage(originalImage), ui->imageLabel);
            statusBar()->showMessage(tr("图像已加载"), 3000);
        }
        updateButtonStates(); // 更新按钮状态
    }
}

// =====================================================================
// openSecondImage 槽函数：打开并显示第二张图像
// =====================================================================
void MainWindow::openSecondImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("选择第二张图像"), "", tr("图像文件 (*.png *.jpg *.jpeg *.bmp)"));
    if (!fileName.isEmpty()) {
        cv::Mat loadedImage = cv::imread(fileName.toStdString());
        if (loadedImage.empty()) {
            QMessageBox::critical(this, tr("错误"), tr("无法加载第二张图像!"));
            statusBar()->showMessage(tr("无法加载第二张图像"), 3000);
            secondImage.release(); // 确保释放内存
            displayImage(QImage(), ui->secondImageLabel); // 清除显示
        } else {
            secondImage = loadedImage.clone(); // 复制图像数据
            displayImage(MatToQImage(secondImage), ui->secondImageLabel);
            statusBar()->showMessage(tr("第二张图像已加载"), 3000);
        }
        updateButtonStates(); // 更新按钮状态
    }
}
*/
// 在 mainwindow.cpp 中找到 openImage() 函数
void MainWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("选择图像"), "", tr("图像文件 (*.png *.jpg *.jpeg *.bmp)"));
    if (!fileName.isEmpty()) {
        // *** 关键更改：使用 toLocal8Bit() 确保转换为系统本地编码 ***
        cv::Mat loadedImage = cv::imread(fileName.toLocal8Bit().toStdString()); // 将 QString 转换为系统本地编码的 std::string
        if (loadedImage.empty()) {
            QMessageBox::critical(this, tr("错误"), tr("无法加载图像!"));
            statusBar()->showMessage(tr("无法加载图像"), 3000);
            originalImage.release();
            displayImage(QImage(), ui->imageLabel);
        } else {
            originalImage = loadedImage.clone();
            displayImage(MatToQImage(originalImage), ui->imageLabel);
            statusBar()->showMessage(tr("图像已加载"), 3000);
        }
        updateButtonStates();
    }
}

// 在 mainwindow.cpp 中找到 openSecondImage() 函数
void MainWindow::openSecondImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("选择第二张图像"), "", tr("图像文件 (*.png *.jpg *.jpeg *.bmp)"));
    if (!fileName.isEmpty()) {
        // *** 关键更改：使用 toLocal8Bit() 确保转换为系统本地编码 ***
        cv::Mat loadedImage = cv::imread(fileName.toLocal8Bit().toStdString()); // 将 QString 转换为系统本地编码的 std::string
        if (loadedImage.empty()) {
            QMessageBox::critical(this, tr("错误"), tr("无法加载第二张图像!"));
            statusBar()->showMessage(tr("无法加载第二张图像"), 3000);
            secondImage.release();
            displayImage(QImage(), ui->secondImageLabel);
        } else {
            secondImage = loadedImage.clone();
            displayImage(MatToQImage(secondImage), ui->secondImageLabel);
            statusBar()->showMessage(tr("第二张图像已加载"), 3000);
        }
        updateButtonStates();
    }
}
// =====================================================================
// swapImages 槽函数：交换主图像和第二图像
// =====================================================================
void MainWindow::swapImages()
{
    if (originalImage.empty() || secondImage.empty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载两张图像才能交换!"));
        statusBar()->showMessage(tr("请先加载两张图像才能交换!"), 3000);
        return;
    }

    cv::Mat tempImage = originalImage.clone(); // 临时存储 originalImage
    originalImage = secondImage.clone();      // secondImage 变为新的 originalImage
    secondImage = tempImage.clone();         // originalImage 变为新的 secondImage

    // 重新显示两张图片
    displayImage(MatToQImage(originalImage), ui->imageLabel);
    displayImage(MatToQImage(secondImage), ui->secondImageLabel);

    statusBar()->showMessage(tr("主图像与第二图像已交换"), 3000);
    // 交换操作不改变图片存在性，所以按钮状态通常不变，但为了安全起见可调用
    // updateButtonStates();
}


// =====================================================================
// sharpenImage 槽函数：锐化图像
// 结果显示在 ui->imageLabel 上
// =====================================================================
void MainWindow::sharpenImage()
{
    if (originalImage.empty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载图像!"));
        statusBar()->showMessage(tr("请先加载图像!"), 3000);
        return;
    }

    // 锐化逻辑
    cv::Mat kernel = (cv::Mat_<float>(3,3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
    cv::filter2D(originalImage, processedImage, originalImage.depth(), kernel);

    originalImage = processedImage.clone(); // 将处理结果更新到 originalImage
    displayImage(MatToQImage(originalImage), ui->imageLabel);
    statusBar()->showMessage(tr("图像已锐化"), 3000);
}

// =====================================================================
// convertToGrayscale 槽函数：灰度化图像
// 结果显示在 ui->imageLabel 上
// =====================================================================
void MainWindow::convertToGrayscale()
{
    if (originalImage.empty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载图像!"));
        statusBar()->showMessage(tr("请先加载图像!"), 3000);
        return;
    }

    cv::Mat grayImage;
    if (originalImage.channels() == 3) {
        cv::cvtColor(originalImage, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = originalImage.clone(); // 如果已经是灰度图，则直接复制
    }

    originalImage = grayImage.clone();
    displayImage(MatToQImage(originalImage), ui->imageLabel);
    statusBar()->showMessage(tr("图像已灰度化"), 3000);
}

// =====================================================================
// performCannyEdgeDetection 槽函数：Canny边缘检测
// 结果显示在 ui->imageLabel 上
// =====================================================================
void MainWindow::performCannyEdgeDetection()
{
    if (originalImage.empty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载图像!"));
        statusBar()->showMessage(tr("请先加载图像!"), 3000);
        return;
    }

    cv::Mat grayImage;
    if (originalImage.channels() == 3) {
        cv::cvtColor(originalImage, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = originalImage.clone();
    }

    cv::Mat edges;
    cv::Canny(grayImage, edges, 100, 200, 3, false); // 阈值可调

    originalImage = edges.clone();
    displayImage(MatToQImage(originalImage), ui->imageLabel);
    statusBar()->showMessage(tr("已进行Canny边缘检测"), 3000);
}

// =====================================================================
// applyGammaCorrection 槽函数：伽马变换
// 结果显示在 ui->imageLabel 上
// =====================================================================
void MainWindow::applyGammaCorrection()
{
    if (originalImage.empty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载图像!"));
        statusBar()->showMessage(tr("请先加载图像!"), 3000);
        return;
    }

    double gamma = 0.5; // 伽马值，可根据需要调整或从UI获取
    cv::Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    for (int i = 0; i < 256; ++i) {
        p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);
    }

    cv::Mat gammaCorrectedImage;
    cv::LUT(originalImage, lookUpTable, gammaCorrectedImage);

    originalImage = gammaCorrectedImage.clone();
    displayImage(MatToQImage(originalImage), ui->imageLabel);
    statusBar()->showMessage(tr("已应用伽马变换"), 3000);
}

// =====================================================================
// blendImages 槽函数：图像融合
// 结果显示在 ui->imageLabel 上，secondImage 区域清空
// =====================================================================
void MainWindow::blendImages()
{
    if (originalImage.empty() || secondImage.empty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载两张图像!"));
        statusBar()->showMessage(tr("请先加载两张图像!"), 3000);
        return;
    }

    // 确保两张图像大小和类型一致，否则尝试调整第二张图像大小
    if (originalImage.size() != secondImage.size() || originalImage.type() != secondImage.type()) {
        QMessageBox::warning(this, tr("错误"), tr("两张图像的大小或类型不匹配，无法融合！\n将尝试调整第二张图像大小。"));
        cv::resize(secondImage, secondImage, originalImage.size());
        if (originalImage.type() != secondImage.type()) {
            secondImage.convertTo(secondImage, originalImage.type());
        }
    }

    cv::Mat blendedImage;
    double alpha = 0.5; // 融合比例，可调整
    cv::addWeighted(originalImage, alpha, secondImage, 1.0 - alpha, 0.0, blendedImage);

    originalImage = blendedImage.clone(); // 将融合结果设为主图像
    displayImage(MatToQImage(originalImage), ui->imageLabel);
    secondImage.release(); // 释放第二张图像的内存
    displayImage(QImage(), ui->secondImageLabel); // 清空第二张图像显示区域
    statusBar()->showMessage(tr("图像已融合"), 3000);
    updateButtonStates(); // 更新按钮状态
}

// =====================================================================
// stitchImages 槽函数：图像拼接
// 结果显示在 ui->imageLabel 上，secondImage 区域清空
// =====================================================================
void MainWindow::stitchImages()
{
    if (originalImage.empty() || secondImage.empty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载两张图像!"));
        statusBar()->showMessage(tr("请先加载两张图像!"), 3000);
        return;
    }

    std::vector<cv::Mat> imgs;
    imgs.push_back(originalImage);
    imgs.push_back(secondImage);

    cv::Mat panorama;
    cv::Stitcher::Mode mode = cv::Stitcher::PANORAMA; // 可选 SCANS、PANORAMA
    cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(mode);

    cv::Stitcher::Status status = stitcher->stitch(imgs, panorama);

    if (status != cv::Stitcher::OK) {
        QString errorMsg = QString("图像拼接失败！错误代码: %1").arg(status);
        QMessageBox::critical(this, tr("错误"), errorMsg);
        statusBar()->showMessage(errorMsg, 5000);
        return;
    }

    originalImage = panorama.clone(); // 将拼接结果设为主图像
    displayImage(MatToQImage(originalImage), ui->imageLabel);
    secondImage.release(); // 释放第二张图像的内存
    displayImage(QImage(), ui->secondImageLabel); // 清空第二张图像显示区域
    statusBar()->showMessage(tr("图像已拼接"), 3000);
    updateButtonStates(); // 更新按钮状态
}

// =====================================================================
// applyTextureTransfer 槽函数：纹理迁移（使用泊松克隆）
// 结果显示在 ui->imageLabel 上，secondImage 区域清空
// =====================================================================
void MainWindow::applyTextureTransfer()
{
    if (originalImage.empty() || secondImage.empty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载两张图像！第一张是目标，第二张是纹理源。"));
        statusBar()->showMessage(tr("请先加载两张图像！"), 3000);
        return;
    }

    // 确保源图像大小与目标图像一致
    cv::Mat source_resized;
    cv::resize(secondImage, source_resized, originalImage.size());

    // 创建一个简单的掩码，例如一个矩形区域
    // 掩码的大小和位置可以根据需要调整，或者通过用户交互来确定
    cv::Mat mask = cv::Mat::zeros(originalImage.size(), CV_8U);
    int center_x = originalImage.cols / 2;
    int center_y = originalImage.rows / 2;
    int rect_width = originalImage.cols / 2;
    int rect_height = originalImage.rows / 2;
    cv::Rect roi(center_x - rect_width / 2, center_y - rect_height / 2, rect_width, rect_height);
    mask(roi).setTo(255); // 在ROI区域设置掩码为255（白色）

    cv::Point center = cv::Point(center_x, center_y); // 克隆中心点

    cv::Mat result_cloned;
    // seamlessClone 是 photo 模块的功能
    cv::seamlessClone(source_resized, originalImage, mask, center, result_cloned, cv::NORMAL_CLONE);

    originalImage = result_cloned.clone(); // 将结果更新到主图像
    displayImage(MatToQImage(originalImage), ui->imageLabel);
    secondImage.release(); // 释放第二张图像的内存
    displayImage(QImage(), ui->secondImageLabel); // 清空第二张图像显示区域
    statusBar()->showMessage(tr("已应用纹理迁移 (泊松克隆)"), 3000);
    updateButtonStates(); // 更新按钮状态
}

// =====================================================================
// applyFaceBeautify 槽函数：人脸美颜（使用双边滤波进行磨皮）
// 结果显示在 ui->imageLabel 上
// =====================================================================
void MainWindow::applyFaceBeautify()
{
    if (originalImage.empty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载图像!"));
        statusBar()->showMessage(tr("请先加载图像!"), 3000);
        return;
    }

    cv::Mat beautifiedImage;
    int d = 9; // 滤波器直径
    double sigmaColor = 75; // 颜色空间滤波器的 sigma 值
    double sigmaSpace = 75; // 坐标空间滤波器的 sigma 值

    cv::bilateralFilter(originalImage, beautifiedImage, d, sigmaColor, sigmaSpace);

    originalImage = beautifiedImage.clone(); // 将结果更新到主图像
    displayImage(MatToQImage(originalImage), ui->imageLabel);
    statusBar()->showMessage(tr("已应用人脸美颜 (磨皮)"), 3000);
}
//我勒个豆阿 怎么会这样呜呜呜，呀脉络