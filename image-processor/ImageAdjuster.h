#ifndef IMAGEADJUSTER_H
#define IMAGEADJUSTER_H

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QPushButton>
#include "ImageProcessor.h"
#include "EditState.h"

class ImageAdjuster : public QWidget, public ImageProcessor
{
    Q_OBJECT

public:
    ImageAdjuster(QWidget *parent = nullptr);

    void loadImage(const QString &fileName) override;
    void closeImage() override;
    void adjustBrightness(int value) override;
    void adjustSaturation(int value) override;
    void adjustHue(int value) override;
    void saveImage(const QString &fileName) override;

    void resetState();
    void toggleSlider(EditState state);
    void toggleApplyButton();

private:
    QLabel *messageLabel;
    QLabel *imageLabel;
    QLabel *brightnessLabel;
    QLabel *saturationLabel;
    QLabel *hueLabel;
    QSlider *brightnessSlider;
    QSlider *saturationSlider;
    QSlider *hueSlider;
    QImage originalImage;
    QImage adjustedImage;
    QImage tempImage;
    EditState currentState;
    QPushButton *applyButton;

    void updateImage();
    int normalizeHue(int hue);
    QSlider *createSlider(int minValue, int maxValue, int defaultValue, QWidget *parent);
    void applyAdjustments();

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // IMAGEADJUSTER_H
