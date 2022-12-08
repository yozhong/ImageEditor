#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void showImage(QString path);
    void setupShortcuts();

    QGraphicsScene *imageScene;
    QGraphicsPixmapItem *currentImage;
    QString currentImagePath;

private slots:
    void openImage();
    void zoomIn();
    void zoomOut();
    void saveAs();
    void prevImage();
    void nextImage();
};
#endif // MAINWINDOW_H
