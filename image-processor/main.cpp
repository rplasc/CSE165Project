#include <QtWidgets>
#include <QFileDialog>
#include <QImageReader>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QPushButton>

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

class ImageAdjuster : public QWidget, public ImageProcessor
{
public:
    ImageAdjuster(QWidget *parent = nullptr)
        : QWidget(parent)
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

        resetButton = new QPushButton(tr("Reset"), this);
        connect(resetButton, &QPushButton::clicked, this, &ImageAdjuster::resetSliders);
        layout->addWidget(resetButton);
        resetButton->hide();

        layout->addWidget(brightnessLabel);
        layout->addWidget(brightnessSlider);
        layout->addWidget(saturationLabel);
        layout->addWidget(saturationSlider);
        layout->addWidget(hueLabel);
        layout->addWidget(hueSlider);
    }

    void loadImage(const QString &fileName) override
    {
        QImage loadedImage;
        if (!loadedImage.load(fileName)) {
            // Display error message
            QMessageBox::critical(this, tr("Error"), tr("Failed to load image"));
            return;
        }

        originalImage = loadedImage;
        adjustedImage = originalImage.copy(); // Make a copy of the original image
        messageLabel->hide();
        imageLabel->show();
        brightnessLabel->show();
        brightnessSlider->show();
        saturationLabel->show();
        saturationSlider->show();
        hueLabel->show();
        hueSlider->show();
        resetButton->show();
        updateImage();
    }

    void closeImage() override
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
        resetButton->hide();
    }

    void adjustBrightness(int value) override
    {
        qreal brightnessFactor = value / 100.0; // Normalize to range [-1, 1]

        for (int y = 0; y < adjustedImage.height(); ++y) {
            QRgb *scanLine = reinterpret_cast<QRgb *>(adjustedImage.scanLine(y));
            for (int x = 0; x < adjustedImage.width(); ++x) {
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

    void adjustSaturation(int value) override
    {
        qreal saturationFactor = value / 100.0; // Normalize to range [-1, 1]

        for (int y = 0; y < adjustedImage.height(); ++y) {
            QRgb *scanLine = reinterpret_cast<QRgb *>(adjustedImage.scanLine(y));
            for (int x = 0; x < adjustedImage.width(); ++x) {
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

    void adjustHue(int value) override
    {
        qreal hueFactor = value / 100.0; // Normalize to range [-1, 1]

        for (int y = 0; y < adjustedImage.height(); ++y) {
            QRgb *scanLine = reinterpret_cast<QRgb *>(adjustedImage.scanLine(y));
            for (int x = 0; x < adjustedImage.width(); ++x) {
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

    void saveImage(const QString &fileName) override
    {
        adjustedImage.save(fileName);
    }

    void resetSliders()
    {
        brightnessSlider->setValue(0);
        saturationSlider->setValue(0);
        hueSlider->setValue(0);
        adjustBrightness(0);
        adjustSaturation(0);
        adjustHue(0);
        adjustedImage = originalImage.copy();
        updateImage();
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QWidget::resizeEvent(event);
        updateImage();
    }

    int normalizeHue(int hue)
    {
        // Ensure hue value is within the range [0, 359]
        if (hue < 0)
            hue += 360;
        else if (hue >= 360)
            hue -= 360;
        return hue;
    }

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
    QPushButton *resetButton;

    QSlider *createSlider(int minValue, int maxValue, int defaultValue, QWidget *parent)
    {
        QSlider *slider = new QSlider(Qt::Horizontal, parent);
        slider->setRange(minValue, maxValue);
        slider->setValue(defaultValue);

        return slider;
    }

    void updateImage()
    {
        if (adjustedImage.isNull())
            return;

        // Get the size of the available space for the image
        QSize availableSize = imageLabel->size();

        // Calculate the scaled size while maintaining the aspect ratio
        QSize scaledSize = adjustedImage.size().scaled(availableSize, Qt::KeepAspectRatio);

        // Calculate the position to center the image in the available space
        QPoint position = QPoint((availableSize.width() - scaledSize.width()) / 2,
                                 (availableSize.height() - scaledSize.height()) / 2);

        // Create a QPixmap from the adjustedImage and scale it to the calculated size
        QPixmap pixmap = QPixmap::fromImage(adjustedImage).scaled(scaledSize, Qt::KeepAspectRatio);

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
};

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        adjuster = new ImageAdjuster(this);
        setCentralWidget(adjuster);
        setWindowTitle("Image Processor");

        // Set the background color of the main window to dark gray
        QPalette palette = this->palette();
        palette.setColor(QPalette::Window, QColor(Qt::darkGray));
        this->setPalette(palette);

        QAction *openAction = new QAction(tr("&Open..."), this);
        connect(openAction, &QAction::triggered, this, &MainWindow::open);
        menuBar()->addAction(openAction);

        closeAction = new QAction(tr("Close Image"), this);
        connect(closeAction, &QAction::triggered, this, &MainWindow::closeImage);
        closeAction->setDisabled(true);
        menuBar()->addAction(closeAction);

        saveAction = new QAction(tr("Save Image"), this);
        connect(saveAction, &QAction::triggered, this, &MainWindow::saveImage);
        saveAction->setDisabled(true);
        menuBar()->addAction(saveAction);
    }

private slots:
    void open()
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), QString(), tr("Images (*.png *.jpg *.bmp)"));
        if (!fileName.isEmpty()) {
            adjuster->loadImage(fileName);
            closeAction->setEnabled(true);
            saveAction->setEnabled(true);
        }
    }

    void closeImage()
    {
        adjuster->closeImage();
        closeAction->setDisabled(true);
        saveAction->setDisabled(true);
    }

    void saveImage()
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), QString(), tr("Images (*.png *.jpg *.bmp)"));
        if (!fileName.isEmpty()) {
            adjuster->saveImage(fileName);
        }
    }

private:
    ImageAdjuster *adjuster;
    QAction *closeAction;
    QAction *saveAction;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
