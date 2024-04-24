#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QString>

class ImageProcessor
{
public:
    virtual ~ImageProcessor() {}

    virtual void loadImage(const QString &fileName) = 0;
    virtual void closeImage() = 0;
    virtual void adjustBrightness(int value) = 0;
    virtual void adjustSaturation(int value) = 0;
    virtual void adjustHue(int value) = 0;
    virtual void saveImage(const QString &fileName) = 0;
};

#endif // IMAGEPROCESSOR_H
