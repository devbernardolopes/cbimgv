#include "mainwindow.h"
#include "imageview.h"

#include <QFileInfo>

MainWindow::MainWindow(
    const QImage &image,
    const QString &imagePath,
    QWidget *parent
)
    : QMainWindow(parent)
{
    setWindowTitle(QFileInfo(imagePath).fileName() + " - cbimgv");
    resize(800, 600);

    auto *imageView = new ImageView(image);

    setCentralWidget(imageView);
}