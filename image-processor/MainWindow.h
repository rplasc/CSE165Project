#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ImageAdjuster.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void open();
    void closeImage();
    void saveImage();
    void reset();
    void toggleBrightness();
    void toggleSaturation();
    void toggleHue();

private:
    ImageAdjuster *adjuster;
    QAction *closeAction;
    QAction *saveAction;
    QMenu *editMenu;
    QAction *brightnessAction;
    QAction *saturationAction;
    QAction *hueAction;
    QAction *resetAction;
};

#endif // MAINWINDOW_H
