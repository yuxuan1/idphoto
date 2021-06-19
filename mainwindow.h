#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include "idphoto.h"
#include "imagelabel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QLabel;
class QImage;
class QScrollArea;
class QWidget;
class QPixmap;
class QPainter;
class QColor;
QT_END_NAMESPACE

using namespace cv;

#define MENUBARHEIGHT 24
#define IMGMARGINY    10
#define IMGMARGINX    30

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool loadImage(const QString &);

private slots:
    void openImage();
    void saveImage();
//    void fixedSizeCutMode();
    void mousePress();
    void mouseMove();
    void movePhoto();
    void endCutMode();
    void on_actionOpen_triggered();
    void on_actionCut_triggered();
    void on_actionAbout_this_application_triggered();
    void on_actionzoom_in_triggered();
    void on_actionzoom_out_triggered();
    void on_actionrecall_triggered();
    void on_actioncreacall_triggered();
    void on_actionsaveAs_triggered();
    void on_actionchangeColor_triggered();
    void on_bgColorBlue_clicked();
    void on_bgColorWhite_clicked();
    void on_bgColorRed_clicked();
    void on_bgColorFree_clicked();

    void on_keepHair_clicked();

    void on_notKeepHair_clicked();

private:
    Ui::MainWindow *ui;
    QColor backgroundColor;
    void connectSlots();
    void initialWindow();
    void setupImage(QLabel *, const Mat &);
    void drawSelectedRec();
    void drawFixedRec(const QPoint &);
    void drawRec(const QPoint &, const QPoint &);
    constexpr inline QPoint calculateFinalPoint(const QPoint &);

    bool isCutMode;
    int mainWindowWidth  = 800;
    int mainWindowHeight = 500;
    IDPhoto idphoto;
};
#endif // MAINWINDOW_H
