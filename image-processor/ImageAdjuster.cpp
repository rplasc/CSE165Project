#include "ImageAdjuster.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QPainter>
#include <QResizeEvent>
#include <QMessageBox>

ImageAdjuster::ImageAdjuster(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    messageLabel = new QLabel(tr("Please select an image."), this);
    messageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(messageLabel);

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setScaledContents(true);
    layout->addWidget(imageLabel, 1);

    brightnessLabel = new QLabel(tr("Brightness"), this);
    brightnessSlider = createSlider(-100, 100, 0, this);
    brightnessLabel->hide();
    brightnessSlider->hide();
    connect(brightnessSlider, &QSlider::valueChanged, this, &ImageAdjuster::adjustBrightness);

    saturationLabel = new QLabel(tr("Saturation"), this);
    saturationSlider = createSlider(-100, 100, 0, this);
    saturationLabel->hide();
    saturationSlider->hide();
    connect(saturationSlider, &QSlider::valueChanged, this, &ImageAdjuster::adjustSaturation);

    hueLabel = new QLabel(tr("Hue"), this);
    hueSlider = createSlider(-100, 100, 0, this);
    hueLabel->hide();
    hueSlider->hide();
    connect(hueSlider, &QSlider::valueChanged, this, &ImageAdjuster::adjustHue);

    applyButton = new QPushButton(tr("Apply"), this);
    applyButton->hide();
    connect(applyButton, &QPushButton::clicked, this, &ImageAdjuster::applyAdjustments);


    layout->addWidget(brightnessLabel);
    layout->addWidget(brightnessSlider);
    layout->addWidget(saturationLabel);
    layout->addWidget(saturationSlider);
    layout->addWidget(hueLabel);
    layout->addWidget(hueSlider);
    layout->addWidget(applyButton);
}

void ImageAdjuster::loadImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName)) {
        // Display error message
        QMessageBox::critical(this, tr("Error"), tr("Failed to load image"));
        return;
    }

    originalImage = loadedImage;
    adjustedImage = originalImage.copy(); // Make a copy of the original image
    tempImage = adjustedImage;
    messageLabel->hide();
    imageLabel->show();
    currentState = EditState::None;
    updateImage();
}

void ImageAdjuster::closeImage()
{
    originalImage = QImage();
    adjustedImage = QImage();
    imageLabel->clear();
    brightnessSlider->setValue(0);
    saturationSlider->setValue(0);
    hueSlider->setValue(0);
    messageLabel->show();
    imageLabel->hide();
    brightnessLabel->hide();
    brightnessSlider->hide();
    saturationLabel->hide();
    saturationSlider->hide();
    hueLabel->hide();
    hueSlider->hide();

    currentState = EditState::None;
    toggleApplyButton();
}

void ImageAdjuster::adjustBrightness(int value)
{
    tempImage = adjustedImage.copy(); // Make a copy of the original image

    qreal brightnessFactor = value / 100.0; // Normalize to range [-1, 1]

    for (int y = 0; y < tempImage.height(); ++y) {
        QRgb *scanLine = reinterpret_cast<QRgb *>(tempImage.scanLine(y));
        for (int x = 0; x < tempImage.width(); ++x) {
            QColor color = QColor::fromRgb(scanLine[x]);
            int h, s, v;
            color.getHsv(&h, &s, &v);

            // Adjust brightness
            v = qBound(0, v + static_cast<int>(255 * brightnessFactor), 255);

            color.setHsv(h, s, v);
            scanLine[x] = color.rgb();
        }
    }

    updateImage();
}

void ImageAdjuster::adjustSaturation(int value)
{
    tempImage = adjustedImage.copy(); // Make a copy of the original image

    qreal saturationFactor = value / 100.0; // Normalize to range [-1, 1]

    for (int y = 0; y < tempImage.height(); ++y) {
        QRgb *scanLine = reinterpret_cast<QRgb *>(tempImage.scanLine(y));
        for (int x = 0; x < tempImage.width(); ++x) {
            QColor color = QColor::fromRgb(scanLine[x]);
            int h, s, v;
            color.getHsv(&h, &s, &v);

            // Adjust saturation
            s = qBound(0, s + static_cast<int>(255 * saturationFactor), 255);

            color.setHsv(h, s, v);
            scanLine[x] = color.rgb();
        }
    }

    updateImage();
}

void ImageAdjuster::adjustHue(int value)
{
    tempImage = adjustedImage.copy(); // Make a copy of the original image

    qreal hueFactor = value / 100.0; // Normalize to range [-1, 1]

    for (int y = 0; y < tempImage.height(); ++y) {
        QRgb *scanLine = reinterpret_cast<QRgb *>(tempImage.scanLine(y));
        for (int x = 0; x < tempImage.width(); ++x) {
            QColor color = QColor::fromRgb(scanLine[x]);
            int h, s, v;
            color.getHsv(&h, &s, &v);

            // Adjust hue
            qreal hueShift = hueFactor * 60.0; // Map warmth to a hue shift in degrees
            h = normalizeHue(h + static_cast<int>(hueShift));

            color.setHsv(h, s, v);
            scanLine[x] = color.rgb();
        }
    }

    updateImage();
}

void ImageAdjuster::saveImage(const QString &fileName)
{
    adjustedImage.save(fileName);
}

void ImageAdjuster::resetState()
{
    brightnessSlider->setValue(0);
    saturationSlider->setValue(0);
    hueSlider->setValue(0);
    adjustBrightness(0);
    adjustSaturation(0);
    adjustHue(0);
    adjustedImage = originalImage.copy();
    tempImage = adjustedImage;
    updateImage();

    brightnessLabel->hide();
    brightnessSlider->hide();
    saturationLabel->hide();
    saturationSlider->hide();
    hueLabel->hide();
    hueSlider->hide();
    currentState = EditState::None;
    toggleApplyButton();
}

void ImageAdjuster::toggleSlider(EditState state)
{
    // Hide all sliders and labels if they are already visible
    if (brightnessLabel->isVisible())
    {
        brightnessLabel->hide();
        brightnessSlider->hide();
    }
    if (saturationLabel->isVisible())
    {
        saturationLabel->hide();
        saturationSlider->hide();
    }
    if (hueLabel->isVisible())
    {
        hueLabel->hide();
        hueSlider->hide();
    }

    if(currentState != state)
    {
        switch (state)
        {
        case EditState::Brightness:
            brightnessLabel->show();
            brightnessSlider->show();
            break;
        case EditState::Saturation:
            saturationLabel->show();
            saturationSlider->show();
            break;
        case EditState::Hue:
            hueLabel->show();
            hueSlider->show();
            break;
        case EditState::None:
            break;
        }

        currentState = state;
    }
    else currentState = EditState::None;

    toggleApplyButton();
    tempImage = adjustedImage;
    updateImage();
}

void ImageAdjuster::toggleApplyButton()
{
    if(currentState !=EditState::None)
        applyButton->show();
    else
        applyButton->hide();
}

void ImageAdjuster::applyAdjustments()
{
    adjustedImage = tempImage;
    updateImage();

    brightnessSlider->setValue(0);
    saturationSlider->setValue(0);
    hueSlider->setValue(0);
    adjustBrightness(0);
    adjustSaturation(0);
    adjustHue(0);
}

void ImageAdjuster::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateImage();
}

int ImageAdjuster::normalizeHue(int hue)
{
    // Ensure hue value is within the range [0, 359]
    if (hue < 0)
        hue += 360;
    else if (hue >= 360)
        hue -= 360;
    return hue;
}

QSlider *ImageAdjuster::createSlider(int minValue, int maxValue, int defaultValue, QWidget *parent)
{
    QSlider *slider = new QSlider(Qt::Horizontal, parent);
    slider->setRange(minValue, maxValue);
    slider->setValue(defaultValue);

    return slider;
}

void ImageAdjuster::updateImage()
{
    if (tempImage.isNull())
        return;

    // Get the size of the available space for the image
    QSize availableSize = imageLabel->size();

    // Calculate the scaled size while maintaining the aspect ratio
    QSize scaledSize = tempImage.size().scaled(availableSize, Qt::KeepAspectRatio);

    // Calculate the position to center the image in the available space
    QPoint position = QPoint((availableSize.width() - scaledSize.width()) / 2,
                             (availableSize.height() - scaledSize.height()) / 2);

    // Create a QPixmap from the adjustedImage and scale it to the calculated size
    QPixmap pixmap = QPixmap::fromImage(tempImage).scaled(scaledSize, Qt::KeepAspectRatio);

    // Create a new pixmap with the same size as the available space and fill it with black
    QPixmap blackPixmap(availableSize);
    blackPixmap.fill(Qt::black);

    // Create a painter to draw on the black pixmap
    QPainter painter(&blackPixmap);

    // Draw the scaled image onto the black pixmap at the calculated position
    painter.drawPixmap(position, pixmap);

    // Set the black pixmap as the pixmap for the imageLabel
    imageLabel->setPixmap(blackPixmap);

    // Center the imageLabel in the available space
    imageLabel->setAlignment(Qt::AlignCenter);
}
