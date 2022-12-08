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

    connect(ui->actionExit, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(openImage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openImage()
{
    qDebug() << "slot openImage is called.";
}
