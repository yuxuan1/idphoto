#ifndef PHOTOSIZE_H
#define PHOTOSIZE_H

#define DEFAULT_DPI           300
#define ONE_INCH_WIDTH_PIXEL  295
#define ONE_INCH_HEIGHT_PIXEL 413
#define TWO_INCH_WIDTH_PIXEL  413
#define TWO_INCH_HEIGHT_PIXEL 626
#define ONE_INCH_WIDTH_CM     2.5
#define ONE_INCH_HEIGHT_CM    3.5
#define TWO_INCH_WIDTH_CM     3.5
#define TWO_INCH_HEIGHT_CM    5.3
#define CM_PER_INCH           2.54

typedef struct photoSize
{
    int width;
    int height;
}PhotoSize;

typedef enum sizeType
{
    ONE_INCH,
    TWO_INCH,
    OTHERS
}SizeType;

#endif // PHOTOSIZE_H
