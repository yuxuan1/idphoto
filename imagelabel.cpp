#include <QMouseEvent>
#include <QPoint>
#include "imagelabel.h"
#include "photosize.h"

ImageLabel::ImageLabel(QWidget *parent) : QLabel(parent)
{
//    rect = new QRect(0, 0, 0, 0);
    currentPosition = QPoint(0, 0);
}
ImageLabel::~ImageLabel()
{
    ;
}

void ImageLabel::mousePressEvent(QMouseEvent *ev)
{
//    rect->setTopLeft(ev->pos());
    firstPosition = ev->pos();
    currentPosition = ev->pos();
    lastPosition = ev->pos();
    emit sigMousePress();
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *ev)
{
//    rect->setBottomRight(ev->pos());
    currentPosition = ev->pos();
    emit sigMouseRelease();
}

void ImageLabel::mouseMoveEvent(QMouseEvent *ev)
{
//    rect->setBottomRight(ev->pos());
    lastPosition = currentPosition;
    currentPosition = ev->pos();
    emit sigMouseMove();
}

//QRect ImageLabel::getRect()
//{
////    return *rect;
//    return QRect(firstPosition, currentPosition);
//}

QPoint ImageLabel::getFirstPoint()
{
    return firstPosition;
}

QPoint ImageLabel::getCurrentPoint()
{
    return currentPosition;
}

QPoint ImageLabel::getLastPoint()
{
    return lastPosition;
}

//void ImageLabel::setCurrentPoint(const QPoint &point)
//{
//    this->currentPosition = point;
//}

//void ImageLabel::setCurrentPoint(int x, int y)
//{
//    setCurrentPoint(QPoint(x, y));
//}

bool ImageLabel::isSmallThan1Inch()
{
//    if(rect->width() < ONE_INCH_WIDTH_PIXEL || rect->height() < ONE_INCH_HEIGHT_PIXEL)
    if(abs(currentPosition.x() - firstPosition.x()) < ONE_INCH_HEIGHT_PIXEL ||
       abs(currentPosition.y() - firstPosition.y()) < ONE_INCH_HEIGHT_PIXEL)
    {
        return true;
    }
    return false;
}

//bool ImageLabel::setRectWidth(int wid)
//{
//    if(wid < ONE_INCH_WIDTH_PIXEL)
//    {
//        return false;
//    }
//    rect->setWidth(wid);
//    return true;
//}

//bool ImageLabel::setRectHeight(int hei)
//{
//    if(hei < ONE_INCH_HEIGHT_PIXEL)
//    {
//        return false;
//    }
//    rect->setHeight(hei);
//    return true;
//}

//bool ImageLabel::setRect(int wid, int hei)
//{
//    if(setRectWidth(wid) && setRectHeight(hei))
//    {
//        return true;
//    }
//    return false;
//}
