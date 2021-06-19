#ifndef removeBackground_hpp
#define removeBackground_hpp

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <QColor>

using namespace std;
using namespace cv;

//typedef struct
//{
//    unsigned blue;
//    unsigned green;
//    unsigned red;
//}bgrColor;

class removeBackground
{
public:
    removeBackground();
    ~removeBackground();
    void setPhoto(const Mat &);
    Mat &getPhoto();
    Mat changeColorToBGR(unsigned, unsigned, unsigned);
    Mat changeColorToBGR(const QColor &);
    void initial();
    void setKeepHair(bool );
    
private:
    Mat *photo;
    Mat *mask;
    bool keepHair;
    void maskFillHole();
    void maskFindCloth();
    void kmeansSegmentation();
};

#endif /* removeBackground_hpp */
