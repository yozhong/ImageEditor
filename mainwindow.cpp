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
    imageScene->addPixmap(image);
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
