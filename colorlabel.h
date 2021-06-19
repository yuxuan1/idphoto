#ifndef COLORLABEL_H
#define COLORLABEL_H

#include <QLabel>
#include <QColor>

class ColorLabel: public QLabel
{
    Q_OBJECT
public:
    explicit ColorLabel(QWidget *parent = nullptr);
    ~ColorLabel();
    void setBackgroundColor(const QColor &);
    QColor getColor();

protected:
    void mousePressEvent(QMouseEvent *ev);

private:
    QColor color;
};

#endif // COLORLABEL_H
