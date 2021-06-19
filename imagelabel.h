#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>
#include <QRect>

class ImageLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ImageLabel(QWidget *parent = nullptr);
    ~ImageLabel();
//    QRect getRect();
    QPoint getFirstPoint();
    QPoint getCurrentPoint();
    QPoint getLastPoint();
//    void setCurrentPoint(const QPoint &point);
//    void setCurrentPoint(int x, int y);
//    bool setRectWidth(int wid);
//    bool setRectHeight(int hei);
//    bool setRect(int wid, int hei);
    bool isSmallThan1Inch();

signals:
    void sigMouseMove();
    void sigMouseRelease();
    void sigMousePress();

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);

private:
//    QRect *rect;
    QPoint currentPosition;
    QPoint lastPosition;
    QPoint firstPosition;
};

#endif // IMAGELABEL_H
