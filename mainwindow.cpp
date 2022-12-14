#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPluginLoader>

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
    ui->viewToolBar->addAction(ui->actionRotate_left);
    ui->viewToolBar->addAction(ui->actionRotate_right);

    imageScene = new QGraphicsScene(this);
    ui->graphicsView->setScene(imageScene);
    currentImage = nullptr;

    // connect the signals and slots
    connect(ui->actionExit, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(openImage()));
    connect(ui->actionZoom_in, SIGNAL(triggered(bool)), this, SLOT(zoomIn()));
    connect(ui->actionZoom_out, SIGNAL(triggered(bool)), this, SLOT(zoomOut()));
    connect(ui->actionSave_as, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
    connect(ui->actionPrevious_Image, SIGNAL(triggered(bool)), this, SLOT(prevImage()));
    connect(ui->actionNext_Image, SIGNAL(triggered(bool)), this, SLOT(nextImage()));
    connect(ui->actionRotate_left, SIGNAL(triggered(bool)), this, SLOT(rotateLeft()));
    connect(ui->actionRotate_right, SIGNAL(triggered(bool)), this, SLOT(rotateRight()));

    setupShortcuts();
    loadPlugins();
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
    currentImagePath = path;

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

void MainWindow::rotateRight()
{
    ui->graphicsView->rotate(90);
}

void MainWindow::rotateLeft()
{
    ui->graphicsView->rotate(-90);
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
        static QRegularExpression re(".+\\.(png|bmp|jpg)");

        if(re.match(fileNames.at(0)).hasMatch()) {
            currentImage->pixmap().save(fileNames.at(0));
        } else {
            QMessageBox::information(this, "Information", "Save error: bad format or filename.");
        }
    }
}

void MainWindow::prevImage()
{
    QFileInfo current(currentImagePath);
    QDir dir = current.absoluteDir();

    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg";

    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int idx = fileNames.indexOf(QRegularExpression(QRegularExpression::escape(current.fileName())));
    if(idx > 0) {
        showImage(dir.absoluteFilePath(fileNames.at(idx - 1)));
    } else {
        QMessageBox::information(this, "Information", "Current image is the first one.");
    }
}

void MainWindow::nextImage()
{
    QFileInfo current(currentImagePath);
    QDir dir = current.absoluteDir();

    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg";

    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int idx = fileNames.indexOf(QRegularExpression(QRegularExpression::escape(current.fileName())));
    if(idx < fileNames.size() - 1) {
        showImage(dir.absoluteFilePath(fileNames.at(idx + 1)));
    } else {
        QMessageBox::information(this, "Information", "Current image is the last one.");
    }
}

void MainWindow::setupShortcuts()
{
    QList<QKeySequence> shortcuts;
    shortcuts << Qt::Key_Plus << Qt::Key_Equal;
    ui->actionZoom_in->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Minus << Qt::Key_Underscore;
    ui->actionZoom_out->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Up << Qt::Key_Left;
    ui->actionPrevious_Image->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Down << Qt::Key_Right;
    ui->actionNext_Image->setShortcuts(shortcuts);
}

void MainWindow::loadPlugins()
{
    QDir pluginsDir(QApplication::instance()->applicationDirPath() + "/plugins");
    QStringList nameFilters;
    nameFilters << "*.so" << "*.dylib" << "*.dll";

    QFileInfoList plugins = pluginsDir.entryInfoList(
        nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);

    foreach(QFileInfo plugin, plugins) {
        QPluginLoader pluginLoader(plugin.absoluteFilePath(), this);
        EditorPluginInterface *plugin_ptr = dynamic_cast<EditorPluginInterface*>(pluginLoader.instance());
        if(plugin_ptr) {
            QAction *action = new QAction(plugin_ptr->name());
            ui->menuEdit->addAction(action);
            ui->editToolBar->addAction(action);
            editPlugins[plugin_ptr->name()] = plugin_ptr;
            connect(action, SIGNAL(triggered(bool)), this, SLOT(pluginPerform()));
        // pluginLoader.unload();
        } else {
            qDebug() << "bad plugin: " << plugin.absoluteFilePath();
        }
    }
}

void MainWindow::pluginPerform()
{
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "No image to edit.");
        return;
    }

    QAction *active_action = qobject_cast<QAction*>(sender());
    EditorPluginInterface *plugin_ptr = editPlugins[active_action->text()];
    if(!plugin_ptr) {
        QMessageBox::information(this, "Information", "No plugin is found.");
        return;
    }

    QImage image = currentImage->pixmap().toImage();
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat mat = cv::Mat(
        image.height(),
        image.width(),
        CV_8UC3,
        image.bits(),
        image.bytesPerLine());

    plugin_ptr->edit(mat, mat);
    QImage image_edited(
        mat.data,
        mat.cols,
        mat.rows,
        mat.step,
        QImage::Format_RGB888);

    QPixmap pixmap = QPixmap::fromImage(image_edited);
    imageScene->clear();
    currentImage = imageScene->addPixmap(pixmap);
    imageScene->update();

    ui->graphicsView->resetTransform();
    ui->graphicsView->setSceneRect(pixmap.rect());

    QString status = QString("(editted image), %1x%2")
        .arg(pixmap.width()).arg(pixmap.height());
    ui->statusLabel->setText(status);
}
