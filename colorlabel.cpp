#include "colorlabel.h"
#include <QColorDialog>

ColorLabel::ColorLabel(QWidget *parent): QLabel(parent)
{
    color = QColor(60, 160, 230);
}

ColorLabel::~ColorLabel()
{

}

void ColorLabel::setBackgroundColor(const QColor &color)
{
    setStyleSheet(QString("QLabel { background-color: rgb(%1, %2, %3); }")
                  .arg(color.red())
                  .arg(color.green())
                  .arg(color.blue()));
    this->color = color;
}

void ColorLabel::mousePressEvent(QMouseEvent *ev)
{
    QColorDialog dialog;
    color = dialog.getColor(Qt::blue, this, "color select");
    setBackgroundColor(color);
}

QColor ColorLabel::getColor()
{
    return color;
}
