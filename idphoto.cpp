#include <opencv2/opencv.hpp>
#include <QColor>
#include <vector>
#include <QDebug>

#include "idphoto.h"

IDPhoto::IDPhoto()
{
    maxHistotryNumber = 4;
    history = new Mat[this->maxHistotryNumber + 1];
    currentHistoryPhoto = 0;
    lastHistorPhoto = 0;
    dpi = DEFAULT_DPI;
//    size = ONE_INCH;
//    photoSize = {ONE_INCH_WIDTH_PIXEL, ONE_INCH_HEIGHT_PIXEL};
    scaleFactor = 1.0;
    isFirstRecall = true;
    isFull = false;
}

IDPhoto::~IDPhoto()
{
    delete [] history;
}

void IDPhoto::setInitialPhoto(Mat &photo)
{
    photo.copyTo(this->photo);
    photo.copyTo(history[0]);
    currentHistoryPhoto = 0;
    lastHistorPhoto = 0;
    scaleFactor = 1.0;
    isFirstRecall = true;
    isFull = false;
}

void IDPhoto::setPhoto(Mat &photo)
{
    photo.copyTo(this->photo);
//    photoSize.width = this->photo.cols;
//    photoSize.height = this->photo.rows;
    setHistoryPhoto(photo);
}

Mat& IDPhoto::getPhoto()
{
    return photo;
//    return history[currentHistoryPhoto];
}

int IDPhoto::setDpi(int newDpi)
{
    if(newDpi < 0 || newDpi > 600)
    {
        return -1;
    }
    this->dpi = newDpi;
//    if(this->size == ONE_INCH)
//    {
//        this->photoSize.width = (int)((ONE_INCH_WIDTH_CM / CM_PER_INCH ) * this->dpi);
//        this->photoSize.height = (int)((ONE_INCH_HEIGHT_CM / CM_PER_INCH ) * this->dpi);
//    } else if(this->size == TWO_INCH)
//    {
//        this->photoSize.width = (int)((TWO_INCH_WIDTH_CM / CM_PER_INCH) * this->dpi);
//        this->photoSize.height = (int)((TWO_INCH_HEIGHT_CM / CM_PER_INCH) * this->dpi);
//    } else
//    {
//        return -1;
//    }
    return 0;
}

PhotoSize IDPhoto::getPhotoSize()
{
    PhotoSize size;
    size.width = photo.cols;
    size.height = photo.rows;
    return size;
}

void IDPhoto::setHistorPhotoIndex()
{
    if(isFirstRecall)
    {
        currentHistoryPhoto--;
        /** 如果在没有任何修改的情况下进行单步撤回，
         *  既currentHistoryPhoto = 0，则忽略
         */
        if(currentHistoryPhoto < 0)
        {
            currentHistoryPhoto = 0;
            lastHistorPhoto = 0;
            scaleFactor = 1.0;
            return;
        }
        /** 如果当前历史位置是1，且历史数组记录满标识位置位
         *  则表示需要回到maxHistorNumber处，否则回到原图
         */
        if(currentHistoryPhoto == 0)
        {
            if(isFull)
            {
                currentHistoryPhoto = maxHistotryNumber;
                isFull = false;
            } else
            {
                lastHistorPhoto = 0;
                scaleFactor = 1.0;
            }
        }
        isFirstRecall = false;
    } else
    // 这里表示取消上一次的单步撤回命令
    {
        currentHistoryPhoto++;
        if(currentHistoryPhoto > maxHistotryNumber)
        {
            currentHistoryPhoto = 1;
            isFull = true;
        }
        isFirstRecall = true;
    }
}

void IDPhoto::setHistoryPhoto(Mat &oldPhoto)
{
    currentHistoryPhoto++;
    lastHistorPhoto++;
    if(currentHistoryPhoto > maxHistotryNumber)
    {
        currentHistoryPhoto = 1;
        lastHistorPhoto = 1;
        isFull = true;
    }
    oldPhoto.copyTo(history[currentHistoryPhoto]);
    isFirstRecall = true;
}

void IDPhoto::recallOneStep()
{
    setHistorPhotoIndex();
    history[currentHistoryPhoto].copyTo(photo);
}

void IDPhoto::recallContinous()
{
    // 连续撤回其实就是相当于每一次撤回都是单步撤回的初始状态
    isFirstRecall = true;
    setHistorPhotoIndex();
    if(currentHistoryPhoto == lastHistorPhoto)
    {
        currentHistoryPhoto = 0;
        lastHistorPhoto = 0;
        scaleFactor = 1.0;
    }
    history[currentHistoryPhoto].copyTo(photo);
}

void IDPhoto::zoomIn()
{
    scaleFactor += 0.1;
    cv::resize(history[currentHistoryPhoto], photo, Size(), scaleFactor, scaleFactor);
    // setHistoryPhoto(photo);
}

void IDPhoto::zoomOut()
{
    scaleFactor -= 0.1;
    cv::resize(history[currentHistoryPhoto], photo, Size(), scaleFactor, scaleFactor);
    // setHistoryPhoto(photo);
}

double IDPhoto::getScaleFactor()
{
    return scaleFactor;
}

bool IDPhoto::savePhoto(const QString &filename)
{
    if(cv::imwrite(filename.toStdString(), photo))
    {
        return true;
    }
    return false;
}

void IDPhoto::bgColorConvert(const QColor &color)
{
    static class removeBackground rb;
    Mat currentImg = history[currentHistoryPhoto];
    rb.setPhoto(currentImg);
    rb.initial();
    Mat result = rb.changeColorToBGR(color);
    result.convertTo(result, CV_8UC3, 255);
    result.copyTo(photo);
    setHistoryPhoto(photo);
}
