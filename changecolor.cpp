#include <vector>
#include "changecolor.h"

removeBackground::removeBackground():
photo(new Mat()), mask(new Mat())
{
    keepHair = true;
}

removeBackground::~removeBackground()
{
    delete photo;
    delete mask;
}

void removeBackground::setPhoto(const Mat &newPhoto)
{
    newPhoto.copyTo(*photo);
}

Mat &removeBackground::getPhoto()
{
    return *photo;
}

Mat removeBackground::changeColorToBGR(unsigned blue, unsigned green, unsigned red)
{    
    Mat bg(photo->rows, photo->cols, CV_32FC3, Scalar(blue/255.0, green/255.0, red/255.0));
    Mat fg;
    photo->convertTo(fg, CV_32FC3, 1/255.0);
    Mat alpha;
    mask->convertTo(alpha, CV_32FC3, -1/255.0, 1);
    cvtColor(alpha, alpha, COLOR_GRAY2BGR);
    multiply(fg, alpha, fg);
    multiply(bg, Scalar::all(1.0) - alpha, bg);
    Mat result;
    add(fg, bg, result);
    return result;
}

Mat removeBackground::changeColorToBGR(const QColor &color)
{
    return changeColorToBGR(color.blue(), color.green(), color.red());
}


void removeBackground::maskFillHole()
{
    Mat rMask = ~(*mask);
//    imshow("RMASK", rMask);
    Size size = mask->size();
    Mat temp = Mat::zeros(size.height + 2, size.width + 2, mask->type());
    rMask.copyTo(temp(Range(1, size.height + 1), Range(1, size.width + 1)));
    floodFill(temp, Point(0, 0), Scalar(255));
    Mat cutImg;
    temp(Range(1, size.height + 1), Range(1, size.width + 1)).copyTo(cutImg);
    bitwise_not(cutImg, cutImg);
    bitwise_or(rMask, cutImg, rMask);
    *mask = 255 - rMask;
}

void removeBackground::maskFindCloth()
{
    Mat cloth;
    Mat prePhoto;
    cvtColor(*photo, prePhoto, COLOR_BGR2GRAY);
    GaussianBlur(prePhoto, prePhoto, Size(3, 3), 0);
    Mat cannyEdge;
    Canny(prePhoto, cannyEdge, 50, 120);
    cannyEdge.copyTo(cloth);
    int photoWidth  = prePhoto.cols;
    int photoHeight = prePhoto.rows;
    int neckLocation = int(2 * photoHeight / 3);
    vector<Point> dots;
    dots.push_back(Point(0, 0));
    vector<int> lineNot0;
    for(int i = neckLocation; i < photoHeight; i++)
    {
        lineNot0.clear();
        for(int j = 0; j < photoWidth; j++)
        {
            if(cannyEdge.at<unsigned char>(i, j))
            {
                lineNot0.push_back(j);
            }
        }
        if(lineNot0.size() < 2 || lineNot0.empty())
        {
            break;
        }
        if(lineNot0.front() == 0)
        {
            break;
        }
        dots.push_back(Point(lineNot0.front(), lineNot0.back()));
    }
    for(int i = neckLocation, j = 0; i < int(dots.size() + neckLocation); i++, j++)
    {
        cloth.row(i).colRange(dots.at(j).x, dots.at(j).y).setTo(Scalar(255));
    }
    for(int i = int(dots.size()) + neckLocation; i < photoHeight; i++)
    {
        cloth.row(i).setTo(Scalar(255));
    }
    bitwise_and(~cloth, *mask, *mask);
    GaussianBlur(*mask, *mask, Size(5, 5), 0);
}

void removeBackground::initial()
{
    kmeansSegmentation();
    maskFillHole();
    if(keepHair)
    {
        maskFindCloth();
    } else
    {
        Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
        dilate(*mask, *mask, kernel, Point(-1, -1), 2);
    }
}

void removeBackground::kmeansSegmentation()
{
    int photoWidth = photo->cols;
    int photoHeight = photo->rows;
    
    int sampleCount = photoWidth * photoHeight;
    int clusterCount = 4;
    Mat labels;
    
    Mat points;
    photo->convertTo(points, CV_32F);
    points = points.reshape(0, sampleCount);

    TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 0.1);
    // kmeans(points, clusterCount, labels, criteria, 3, KMEANS_PP_CENTERS);
    kmeans(points, clusterCount, labels, criteria, 3, KMEANS_RANDOM_CENTERS);
    Mat result = labels.reshape(0, photoHeight);
    unsigned char roi = result.at<unsigned char>(0, 0);
    inRange(result, Scalar(roi), Scalar(roi), *mask);
//    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
//    dilate(*mask, *mask, kernel, Point(-1, -1), 2);
//    imshow("mask", *mask);
//    waitKey();
}

void removeBackground::setKeepHair(bool isKeepHair)
{
    if(isKeepHair)
    {
        keepHair = true;
    } else
    {
        keepHair = false;
    }
}
