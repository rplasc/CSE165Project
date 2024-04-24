#include "MainWindow.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    adjuster = new ImageAdjuster(this);
    setCentralWidget(adjuster);
    setWindowTitle("Image Processor");

    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(Qt::darkGray));
    this->setPalette(palette);

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAction = new QAction(tr("&Open..."), this);
    connect(openAction, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAction);

    closeAction = new QAction(tr("Close Image"), this);
    connect(closeAction, &QAction::triggered, this, &MainWindow::closeImage);
    closeAction->setDisabled(true);
    fileMenu->addAction(closeAction);

    saveAction = new QAction(tr("Save as..."), this);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveImage);
    saveAction->setDisabled(true);
    fileMenu->addAction(saveAction);

    editMenu = menuBar()->addMenu(tr("&Edit..."));
    editMenu->setEnabled(false);

    brightnessAction = new QAction(tr("&Brightness"), this);
    connect(brightnessAction, &QAction::triggered, this, &::MainWindow::toggleBrightness);
    editMenu->addAction(brightnessAction);

    saturationAction = new QAction(tr("&Saturation"), this);
    connect(saturationAction, &QAction::triggered, this, &::MainWindow::toggleSaturation);
    editMenu->addAction(saturationAction);

    hueAction = new QAction(tr("&Hue"), this);
    connect(hueAction, &QAction::triggered, this, &MainWindow::toggleHue);
    editMenu->addAction(hueAction);

    resetAction = new QAction(tr("&Reset"), this);
    connect(resetAction, &QAction::triggered, this, &MainWindow::reset);
    editMenu->addAction(resetAction);

}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), QString(), tr("Images (*.png *.jpg *.bmp)"));
    if (!fileName.isEmpty()) {
        adjuster->loadImage(fileName);
        closeAction->setEnabled(true);
        saveAction->setEnabled(true);
        editMenu->setEnabled(true);
    }
    edited = false;
}

void MainWindow::closeImage()
{
    QMessageBox::StandardButton reply;
    if(edited)
    {
        reply = QMessageBox::question(this,"Are you sure?", "Any changes applied will not be saved.");
        if(reply != QMessageBox::Yes) return;
    }
    adjuster->closeImage();
    closeAction->setDisabled(true);
    saveAction->setDisabled(true);
    editMenu->setEnabled(false);
    edited = false;
}

void MainWindow::saveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), QString(), tr("Images (*.png *.jpg *.bmp)"));
    if (!fileName.isEmpty()) {
        adjuster->saveImage(fileName);
    }
}

void MainWindow::reset()
{
    QMessageBox::StandardButton reply;
    if(edited)
    {
        reply = QMessageBox::question(this,"Are you sure?", "The image will be reverted to its original state.");
        if(reply != QMessageBox::Yes) return;
        else
        {
            adjuster->resetState();
            edited = false;
        }
    }
}

void MainWindow::toggleBrightness()
{
    if(!edited) edited = true;
    adjuster->toggleSlider(EditState::Brightness);
}

void MainWindow::toggleSaturation()
{
    if(!edited) edited = true;
    adjuster->toggleSlider(EditState::Saturation);
}

void MainWindow::toggleHue()
{
    if(!edited) edited = true;
    adjuster->toggleSlider(EditState::Hue);
}
