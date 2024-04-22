#include <QtWidgets>
#include <QFileDialog>
#include <QImageReader>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QPushButton>

class Display : public QWidget
{
public:
    Display(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);

        imageLabel = new QLabel(this);
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setScaledContents(true);
        layout->addWidget(imageLabel);

        brightnessSlider = createSlider(-100, 100, 0, tr("Brightness"), this);
        connect(brightnessSlider, &QSlider::valueChanged, this, &Display::updateImage);

        saturationSlider = createSlider(-100, 100, 0, tr("Saturation"), this);
        connect(saturationSlider, &QSlider::valueChanged, this, &Display::updateImage);

        warmthSlider = createSlider(-100, 100, 0, tr("Warmth"), this);
        connect(warmthSlider, &QSlider::valueChanged, this, &Display::updateImage);

        saveButton = new QPushButton(tr("Save"), this);
        connect(saveButton, &QPushButton::clicked, this, &Display::saveImage);

        layout->addWidget(brightnessSlider);
        layout->addWidget(saturationSlider);
        layout->addWidget(warmthSlider);
        layout->addWidget(saveButton);
    }

    void loadImage(const QString &fileName)
    {
        originalImage.load(fileName);
        if (originalImage.isNull()) {
            imageLabel->setText("Failed to load image");
            return;
        }

        updateImage();
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QWidget::resizeEvent(event);
        updateImage();
    }

private:
    QLabel *imageLabel;
    QSlider *brightnessSlider;
    QSlider *saturationSlider;
    QSlider *warmthSlider;
    QPushButton *saveButton;
    QImage originalImage;
    QImage adjustedImage;

    QSlider *createSlider(int minValue, int maxValue, int defaultValue, const QString &label, QWidget *parent)
    {
        QSlider *slider = new QSlider(Qt::Horizontal, parent);
        slider->setRange(minValue, maxValue);
        slider->setValue(defaultValue);

        QLabel *sliderLabel = new QLabel(label, parent);

        QHBoxLayout *layout = new QHBoxLayout();
        layout->addWidget(sliderLabel);
        layout->addWidget(slider);

        parent->layout()->addItem(layout);

        return slider;
    }

    void updateImage()
    {
        adjustedImage = originalImage.convertToFormat(QImage::Format_ARGB32);

        qreal brightness = qBound(-1.0, brightnessSlider->value() / 100.0, 1.0);
        qreal saturation = qBound(-1.0, saturationSlider->value() / 100.0, 1.0);
        qreal warmth = warmthSlider->value() / 100.0; // Use the full range of the slider

        for (int y = 0; y < adjustedImage.height(); ++y) {
            QRgb *scanLine = reinterpret_cast<QRgb *>(adjustedImage.scanLine(y));
            for (int x = 0; x < adjustedImage.width(); ++x) {
                QColor color = QColor::fromRgb(scanLine[x]);
                int h, s, v;
                color.getHsv(&h, &s, &v);

                // Adjust brightness
                v = qMax(0, qMin(v + static_cast<int>(255 * brightness), 255));

                // Adjust saturation
                s = qMax(0, qMin(s + static_cast<int>(255 * saturation), 255));

                // Adjust warmth
                qreal hueShift = warmth * 60.0; // Map warmth to a hue shift in degrees
                h = normalizeHue(h + static_cast<int>(hueShift));

                color.setHsv(h, s, v);
                scanLine[x] = color.rgb();
            }
        }

        imageLabel->setPixmap(QPixmap::fromImage(adjustedImage));
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

    void saveImage()
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), QString(), tr("Images (*.png *.jpg *.bmp)"));
        if (!fileName.isEmpty()) {
            adjustedImage.save(fileName);
        }
    }
};

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        widget = new Display(this);
        setCentralWidget(widget);

        QAction *openAction = new QAction(tr("&Open..."), this);
        connect(openAction, &QAction::triggered, this, &MainWindow::open);
        menuBar()->addAction(openAction);
    }

private slots:
    void open()
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), QString(), tr("Images (*.png *.jpg *.bmp)"));
        if (!fileName.isEmpty()) {
            widget->loadImage(fileName);
            setWindowTitle(fileName);
        }
    }

private:
    Display *widget;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.resize(800, 600);
    window.show();

    return app.exec();
}
