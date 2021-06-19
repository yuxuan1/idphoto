#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "matqt.h"
#include "imagelabel.h"
#include "photosize.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QColorSpace>
#include <QMessageBox>
#include <QScreen>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QMenu>
#include <QPainter>
#include <QSize>
#include <QColor>
#include <QMouseEvent>
#include <QPen>
#include <QFile>
#include <QIcon>
#include <QScrollBar>
#include <QPoint>
#include <QColor>
#include <QPalette>

using namespace cv;

// 初始化选择图片的对话框
static void initializeImageFileDiag(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool isFirstDialog = true;
    if(isFirstDialog)
    {
        isFirstDialog = false;
        dialog.setDirectory("/Users/jiangkexuan/Desktop/2020321上海植物园");
    }
    QStringList mimeTypeFilters;
    const QByteArrayList supportMimeTypes = acceptMode == QFileDialog::AcceptOpen ?
                QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    for(const QByteArray &mimeType : supportMimeTypes)
    {
        mimeTypeFilters.append(QString(mimeType));
    }
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/png");
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    backgroundColor(60, 160, 230),
    isCutMode(false)
{
    ui->setupUi(this);
    connectSlots();
    initialWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectSlots()
{
    connect(ui->imgLabel,&ImageLabel::sigMouseMove, this, &MainWindow::mouseMove);
    connect(ui->imgLabel, &ImageLabel::sigMouseRelease, this, &MainWindow::endCutMode);
    connect(ui->imgLabel, &ImageLabel::sigMousePress, this, &MainWindow::mousePress);
}

void MainWindow::initialWindow()
{
    setWindowTitle("ID photo helper");
    QIcon icon(":/icon/photo.png");
    setWindowIcon(icon);
    QFile file(":/images/red.jpg");
    Mat img;
    if(file.open(QFile::ReadOnly))
    {
        QByteArray ba = file.readAll();
        img = imdecode(std::vector<char>(ba.begin(), ba.end()), 1);
    }
    idphoto.setInitialPhoto(img);
    setupImage(ui->imgLabel, img);

    ui->bgColorBlue->setChecked(true);
    ui->cutModeFree->setChecked(true);
    ui->keepHair->setChecked(true);

    ui->cutModeFree->setDisabled(true);
    ui->cutModeOneInch->setDisabled(true);
    ui->cutModeTwoInch->setDisabled(true);

    ui->colorLabel->setBackgroundColor(QColor(60, 160, 230));
}

void MainWindow::setupImage(QLabel *label, const Mat &img)
{
    QImage temp = CVS::cvMatToQImage(img);
    if(temp.colorSpace().isValid())
    {
        temp.convertToColorSpace(QColorSpace::SRgb);
    }
    label->setPixmap(QPixmap::fromImage(temp));
    label->setFixedSize(img.cols, img.rows);
    statusBar()->showMessage(QString("ID photo size width: %1   height: %2").arg(img.cols).arg(img.rows));
    label->update();
}

bool MainWindow::loadImage(const QString &imgName)
{
    Mat photo = cv::imread(imgName.toStdString());
    this->idphoto.setInitialPhoto(photo);
//    Mat &image = this->idphoto.getPhoto();
    if(photo.rows < ONE_INCH_HEIGHT_PIXEL || photo.cols < ONE_INCH_WIDTH_PIXEL)
    {
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),
                             "The opened photo's size is smaller than 1 inch,"
                             " please selected a new one or resize it.");
    }
    if(photo.empty())
    {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 QString("cant load %1").arg(QDir::toNativeSeparators(imgName)));
        return false;
    }
    setWindowFilePath(imgName);
    const QString message = QString("Opened \"%1\", %2x%3, Depth:%4")
                                    .arg(QDir::toNativeSeparators(imgName))
                                    .arg(photo.cols)
                                    .arg(photo.rows)
                                    .arg(photo.elemSize()*8);
    statusBar()->showMessage(message);
    return true;
}

void MainWindow::openImage()
{
    QFileDialog dialog(this, "Open image");
    initializeImageFileDiag(dialog, QFileDialog::AcceptOpen);
    if(dialog.exec() == QDialog::Accepted)
    {
        QStringList imageList = dialog.selectedFiles();
        if(loadImage(imageList.last()))
        {
            Mat photo = idphoto.getPhoto();
            setupImage(ui->imgLabel, photo);
        }
    }
}

void MainWindow::saveImage()
{
    QFileDialog dialog(this, "Save image");
    initializeImageFileDiag(dialog, QFileDialog::AcceptSave);
    QString imageName = dialog.getSaveFileName(this, "save file", "", "jpg");
    if(imageName.isNull())
    {
        return;
    }
    if(!idphoto.savePhoto(imageName))
    {
        QMessageBox::warning(this, "waring", "save image failed, please retry latter!");
    }
}

constexpr inline QPoint MainWindow::calculateFinalPoint(const QPoint &firstPoint)
{
    QPoint finalPoint;
    if(ui->cutModeOneInch->isChecked())
    {
        finalPoint.setX(firstPoint.x() + ONE_INCH_WIDTH_PIXEL - 1);
        finalPoint.setY(firstPoint.y() + ONE_INCH_HEIGHT_PIXEL - 1);
    } else if(ui->cutModeTwoInch->isChecked())
    {
        finalPoint.setX(firstPoint.x() + TWO_INCH_WIDTH_PIXEL - 1);
        finalPoint.setY(firstPoint.y() + TWO_INCH_HEIGHT_PIXEL - 1);
    } else
    {
        QMessageBox::warning(this, "warning", "the cut mode is wrong!");
        return QPoint();
    }
    return finalPoint;
}

void MainWindow::drawSelectedRec()
{
    QPoint firstPosition = ui->imgLabel->getFirstPoint();
    QPoint currentPosition = ui->imgLabel->getCurrentPoint();
    drawRec(firstPosition, currentPosition);
}

void MainWindow::drawFixedRec(const QPoint &firstPosition)
{
    QPoint finalPoint = calculateFinalPoint(firstPosition);
    if(finalPoint.isNull())
    {
        return;
    }
    drawRec(firstPosition, finalPoint);
}

void MainWindow::drawRec(const QPoint &topLeft, const QPoint &bottomRight)
{
    cv::Mat rgbImg;
    idphoto.getPhoto().copyTo(rgbImg);
    if(topLeft.x() < 0 ||
       topLeft.y() < 0 ||
       bottomRight.x() > rgbImg.cols ||
       bottomRight.y() > rgbImg.rows)
    {
        QMessageBox::warning(this, "warning", "the photo's size is tool small for this cut mode,"
                                              "you can zoom in this photo to satisify the requirement!");
        return;
    }
    cv::Rect roiRect(topLeft.x(),
                     topLeft.y(),
                     abs(bottomRight.x() - topLeft.x() + 1),
                     abs(bottomRight.y() - topLeft.y() + 1));
    // statusBar()->showMessage(QString("width: %1   height: %2").arg(roiRect.width).arg(roiRect.height));
    cv::rectangle(rgbImg,roiRect, Scalar(0, 0, 0));
    setupImage(ui->imgLabel, rgbImg);
}

void MainWindow::mousePress()
{
    if(isCutMode && (ui->cutModeOneInch->isChecked() ||
            ui->cutModeTwoInch->isChecked()))
    {
//        fixedSizeCutMode();
        QPoint firstPosition = ui->imgLabel->getFirstPoint();
        drawFixedRec(firstPosition);
    }
}

void MainWindow::mouseMove()
{
    if(isCutMode)
    {
        if(ui->cutModeFree->isChecked())
        {
            drawSelectedRec();
        } else
        {
            QPoint lastPosition = ui->imgLabel->getLastPoint();
            QPoint currentPosition = ui->imgLabel->getCurrentPoint();
            int dx = currentPosition.x() - lastPosition.x();
            int dy = currentPosition.y() - lastPosition.y();
            QPoint currentPoint(currentPosition.x() + dx, currentPosition.y() + dy);
            drawFixedRec(QPoint(currentPosition.x() + dx, currentPosition.y() + dy));
        }
    } else
    {
        movePhoto();
    }
}

void MainWindow::movePhoto()
{
    QPoint lastPosition = ui->imgLabel->getLastPoint();
    QPoint currentPosition = ui->imgLabel->getCurrentPoint();
    int dx = currentPosition.x() - lastPosition.x();
    int dy = currentPosition.y() - lastPosition.y();

    QScrollBar *vScrollBar = ui->imageScrollArea->verticalScrollBar();
    QScrollBar *hScrollBar = ui->imageScrollArea->horizontalScrollBar();
    vScrollBar->setValue(vScrollBar->value() - dy);
    hScrollBar->setValue(hScrollBar->value() - dx);
}


void MainWindow::endCutMode()
{
    if(isCutMode)
    {
        isCutMode = false;
        QPoint firstPosition ;
        QPoint finalPoint;
        if(ui->cutModeFree->isChecked())
        {
            firstPosition = ui->imgLabel->getFirstPoint();
            finalPoint = ui->imgLabel->getCurrentPoint();
        } else
        {
            firstPosition = ui->imgLabel->getCurrentPoint();
            finalPoint = calculateFinalPoint(firstPosition);
            if(finalPoint.isNull())
            {
                return;
            }
        }

        PhotoSize size = idphoto.getPhotoSize();
        if(finalPoint.x() > size.width) {finalPoint.setX(size.width - 1);}
        if(finalPoint.y() > size.height) {finalPoint.setY(size.height - 1);}
        Mat photo = idphoto.getPhoto();
        cv::Rect roiRect(firstPosition.x(),
                         firstPosition.y(),
                         abs(finalPoint.x() - firstPosition.x() + 1),
                         abs(finalPoint.y() - firstPosition.y() + 1));
        Mat roi(idphoto.getPhoto(), roiRect);
        if(roi.cols < ONE_INCH_WIDTH_PIXEL || roi.rows < ONE_INCH_HEIGHT_PIXEL)
        {
            QMessageBox::warning(this, "Warning",
                                 "The photo size is smaller than 1 inch, Please check it.");
        }
        idphoto.setPhoto(roi);
        setupImage(ui->imgLabel, idphoto.getPhoto());

        ui->cutModeFree->setDisabled(true);
        ui->cutModeOneInch->setDisabled(true);
        ui->cutModeTwoInch->setDisabled(true);
    }
}

void MainWindow::on_actionOpen_triggered()
{
    openImage();
}

void MainWindow::on_actionCut_triggered()
{
    isCutMode = !isCutMode;
    if(isCutMode)
    {
        ui->cutModeFree->setDisabled(false);
        ui->cutModeOneInch->setDisabled(false);
        ui->cutModeTwoInch->setDisabled(false);
    } else
    {
        ui->cutModeFree->setDisabled(true);
        ui->cutModeOneInch->setDisabled(true);
        ui->cutModeTwoInch->setDisabled(true);
    }
}

void MainWindow::on_actionAbout_this_application_triggered()
{
    QMessageBox::information(this, "About this application", "Author: Jiang Kexuan\n\r"
                                    "Email: jiangkexuan@gmail.com\n\r"
                                    "This application is just for test temporarily!");
}

void MainWindow::on_actionzoom_in_triggered()
{
    idphoto.zoomIn();
    Mat photo = idphoto.getPhoto();
    setupImage(ui->imgLabel, photo);
    ui->actionzoom_in->setEnabled(idphoto.getScaleFactor() < 3.0);
    ui->actionzoom_out->setEnabled(idphoto.getScaleFactor() > 0.3);
}

void MainWindow::on_actionzoom_out_triggered()
{
    idphoto.zoomOut();
    Mat photo = idphoto.getPhoto();
    setupImage(ui->imgLabel, photo);
    ui->actionzoom_in->setEnabled(idphoto.getScaleFactor() < 3.0);
    ui->actionzoom_out->setEnabled(idphoto.getScaleFactor() > 0.3);
}

void MainWindow::on_actionrecall_triggered()
{
    idphoto.recallOneStep();
    Mat photo = idphoto.getPhoto();
    setupImage(ui->imgLabel, photo);
}

void MainWindow::on_actioncreacall_triggered()
{
    idphoto.recallContinous();
    Mat photo = idphoto.getPhoto();
    setupImage(ui->imgLabel, photo);
}

void MainWindow::on_actionsaveAs_triggered()
{
    saveImage();
}

void MainWindow::on_actionchangeColor_triggered()
{
    QColor color = ui->colorLabel->getColor();
    idphoto.bgColorConvert(color);
    setupImage(ui->imgLabel, idphoto.getPhoto());
}

void MainWindow::on_bgColorBlue_clicked()
{
    ui->colorLabel->setBackgroundColor(QColor(60, 160, 230));
    ui->colorLabel->setEnabled(false);
}

void MainWindow::on_bgColorWhite_clicked()
{
    ui->colorLabel->setBackgroundColor(QColor(255, 255, 255));
    ui->colorLabel->setEnabled(false);
}

void MainWindow::on_bgColorRed_clicked()
{
    ui->colorLabel->setBackgroundColor(QColor(255, 0, 0));
    ui->colorLabel->setEnabled(false);
}

void MainWindow::on_bgColorFree_clicked()
{
    ui->colorLabel->setEnabled(true);
}

void MainWindow::on_keepHair_clicked()
{

}

void MainWindow::on_notKeepHair_clicked()
{

}
