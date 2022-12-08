#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(ui->statusLabel);
    ui->statusLabel->setText("Image Information will be here!");
    ui->fileToolBar->addAction(ui->actionOpen);
    ui->viewToolBar->addAction(ui->actionZoom_in);
    ui->viewToolBar->addAction(ui->actionZoom_out);
    ui->viewToolBar->addAction(ui->actionPrevious_Image);
    ui->viewToolBar->addAction(ui->actionNext_Image);

    imageScene = new QGraphicsScene(this);
    ui->graphicsView->setScene(imageScene);

    connect(ui->actionExit, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(openImage()));
    connect(ui->actionZoom_in, SIGNAL(triggered(bool)), this, SLOT(zoomIn()));
    connect(ui->actionZoom_out, SIGNAL(triggered(bool)), this, SLOT(zoomOut()));
    connect(ui->actionSave_as, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openImage()
{
    QFileDialog dialog(this);
    dialog.setWindowTitle("Open Image");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QStringList filePaths;
    if (dialog.exec()) {
        filePaths = dialog.selectedFiles();
        showImage(filePaths.at(0));
    }
}

void MainWindow::showImage(QString path)
{
    QPixmap image(path);
    imageScene->clear();
    currentImage = imageScene->addPixmap(image);
    imageScene->update();

    ui->graphicsView->resetTransform();
    ui->graphicsView->setSceneRect(image.rect());

    QString status = QString("%1, %2x%3, %4Bytes")
        .arg(path)
        .arg(image.width())
        .arg(image.height())
        .arg(QFile(path).size());
    ui->statusLabel->setText(status);
}

void MainWindow::zoomIn()
{
    ui->graphicsView->scale(1.2, 1.2);
}

void MainWindow::zoomOut()
{
    ui->graphicsView->scale(1 / 1.2, 1 / 1.2);
}

void MainWindow::saveAs()
{
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "Nothing to save.");
        return;
    }

    QFileDialog dialog(this);
    dialog.setWindowTitle("Save Image As ...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));

    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        if(QRegExp(".+\\.(png|bmp|jpg)").exactMatch(fileNames.at(0))) {
            currentImage->pixmap().save(fileNames.at(0));
        } else {
            QMessageBox::information(this, "Information", "Save error: bad format or filename.");
        }
    }
}
