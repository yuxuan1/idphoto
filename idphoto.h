#ifndef IDPHOTO_H
#define IDPHOTO_H

#include <opencv2/opencv.hpp>
#include <QString>
#include <QColor>
#include "photosize.h"
#include "changecolor.h"

using namespace cv;

class IDPhoto
{
public:
    IDPhoto();
    ~IDPhoto();
    void setPhoto(Mat &);
    void setInitialPhoto(Mat &);
    Mat &getPhoto();
    int setDpi(int newDpi);
    PhotoSize getPhotoSize();
    void setHistoryPhoto(Mat &);
    void recallOneStep();
    void recallContinous();
    void zoomIn();
    void zoomOut();
    double getScaleFactor();
    int currentHistoryPhoto;
    int lastHistorPhoto;
    bool savePhoto(const QString &);
    void bgColorConvert(const QColor &);

private:
    Mat photo;
    int maxHistotryNumber;
    Mat *history;
    int dpi;
//    SizeType size;
//    PhotoSize photoSize;
    bool isFirstRecall;
    bool isFull;
    double scaleFactor;
//    QColor bgColor;

    void setHistorPhotoIndex();
};

#endif // IDPHOTO_H
