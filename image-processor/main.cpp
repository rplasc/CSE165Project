#include <QtWidgets>
#include <QFileDialog>
#include <QImageReader>

class GLWidget : public QLabel
{
public:
    GLWidget(QWidget *parent = nullptr)
        : QLabel(parent)
    {
        setAlignment(Qt::AlignCenter);
    }

    void loadImage(const QString &fileName)
    {
        QPixmap pixmap(fileName);
        if (pixmap.isNull()) {
            setText("Failed to load image");
            return;
        }

        setPixmap(pixmap.scaled(size(), Qt::KeepAspectRatio));
    }
};

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        glWidget = new GLWidget(this);
        setCentralWidget(glWidget);

        QAction *openAction = new QAction(tr("&Open..."), this);
        connect(openAction, &QAction::triggered, this, &MainWindow::open);
        menuBar()->addAction(openAction);
    }

private slots:
    void open()
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), QString(), tr("Images (*.png *.jpg *.bmp)"));
        if (!fileName.isEmpty()) {
            glWidget->loadImage(fileName);
            setWindowTitle(fileName);
        }
    }

private:
    GLWidget *glWidget;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.resize(800, 600);
    window.show();

    return app.exec();
}
