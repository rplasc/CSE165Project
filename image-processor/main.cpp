#include <QtWidgets>
#include <QFileDialog>
#include <QImageReader>

class CropDialog : public QDialog
{
public:
    CropDialog(QWidget *parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("Crop Image");

        QLabel *label = new QLabel("Select crop area:", this);
        label->setAlignment(Qt::AlignCenter);

        xSpinBox = new QSpinBox(this);
        xSpinBox->setMinimum(0);

        ySpinBox = new QSpinBox(this);
        ySpinBox->setMinimum(0);

        widthSpinBox = new QSpinBox(this);
        widthSpinBox->setMinimum(1);

        heightSpinBox = new QSpinBox(this);
        heightSpinBox->setMinimum(1);

        QPushButton *okButton = new QPushButton("OK", this);
        connect(okButton, &QPushButton::clicked, this, &CropDialog::accept);

        QGridLayout *layout = new QGridLayout(this);
        layout->addWidget(label, 0, 0, 1, 2);
        layout->addWidget(new QLabel("X:"), 1, 0);
        layout->addWidget(xSpinBox, 1, 1);
        layout->addWidget(new QLabel("Y:"), 2, 0);
        layout->addWidget(ySpinBox, 2, 1);
        layout->addWidget(new QLabel("Width:"), 3, 0);
        layout->addWidget(widthSpinBox, 3, 1);
        layout->addWidget(new QLabel("Height:"), 4, 0);
        layout->addWidget(heightSpinBox, 4, 1);
        layout->addWidget(okButton, 5, 0, 1, 2);

        setLayout(layout);
    }

    QRect getCropRect() const
    {
        int x = xSpinBox->value();
        int y = ySpinBox->value();
        int width = widthSpinBox->value();
        int height = heightSpinBox->value();

        return QRect(x, y, width, height);
    }

private:
    QSpinBox *xSpinBox;
    QSpinBox *ySpinBox;
    QSpinBox *widthSpinBox;
    QSpinBox *heightSpinBox;
};

class Widget : public QLabel
{
public:
    Widget(QWidget *parent = nullptr)
        : QLabel(parent)
    {
        setAlignment(Qt::AlignCenter);
        setScaledContents(true);
    }

    void loadImage(const QString &fileName)
    {
        image.load(fileName);
        if (image.isNull()) {
            setText("Failed to load image");
            return;
        }

        setPixmap(QPixmap::fromImage(image));
    }

    void cropImage(const QRect &rect)
    {
        QPixmap cropped = pixmap().copy(rect);
        setPixmap(cropped.scaled(size(), Qt::KeepAspectRatio));
        image = cropped.toImage();
    }

private:
    QImage image;
};

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        widget = new Widget(this);
        setCentralWidget(widget);

        QAction *openAction = new QAction(tr("&Open..."), this);
        connect(openAction, &QAction::triggered, this, &MainWindow::open);
        menuBar()->addAction(openAction);

        QAction *cropAction = new QAction(tr("&Crop"), this);
        connect(cropAction, &QAction::triggered, this, &MainWindow::crop);
        menuBar()->addAction(cropAction);
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

    void crop()
    {
        CropDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            QRect cropRect = dialog.getCropRect();
            widget->cropImage(cropRect);
        }
    }

private:
    Widget *widget;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.resize(800, 600);
    window.show();

    return app.exec();
}
