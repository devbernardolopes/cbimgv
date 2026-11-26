#include "mainwindow.h"
#include "imageview.h"

#include <QFileInfo>
#include <QImageReader>
#include <QKeyEvent>
#include <QMessageBox>

MainWindow::MainWindow(
    const QImage &image,
    const QString &imagePath,
    QWidget *parent
)
    : QMainWindow(parent),
      m_navigator(imagePath)
{
    setWindowTitle(QFileInfo(imagePath).fileName() + " - cbimgv");
    resize(800, 600);

    m_imageView = new ImageView(image);
    setCentralWidget(m_imageView);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        close();
        event->accept();
        return;
    case Qt::Key_Left:
        navigateTo(m_navigator.previousPath());
        event->accept();
        return;
    case Qt::Key_Right:
        navigateTo(m_navigator.nextPath());
        event->accept();
        return;
    case Qt::Key_Home:
        navigateTo(m_navigator.firstPath());
        event->accept();
        return;
    case Qt::Key_End:
        navigateTo(m_navigator.lastPath());
        event->accept();
        return;
    case Qt::Key_F11:
        isFullScreen() ? showNormal() : showFullScreen();
        event->accept();
        return;
    case Qt::Key_F:
        m_imageView->fitToWindow();
        event->accept();
        return;
    case Qt::Key_1:
        m_imageView->showOriginalSize();
        event->accept();
        return;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::navigateTo(const QString &imagePath)
{
    if (imagePath == m_navigator.currentPath()) {
        return;
    }

    QImageReader reader(imagePath);
    reader.setAutoTransform(true);
    const QImage image = reader.read();
    if (image.isNull()) {
        QMessageBox::warning(
            this,
            "cbimgv",
            QString("Could not open image:\n\n%1\n\n%2")
                .arg(imagePath, reader.errorString())
        );
        return;
    }

    if (!m_navigator.setCurrentPath(imagePath)) {
        QMessageBox::warning(
            this,
            "cbimgv",
            QString("Could not navigate to image:\n\n%1").arg(imagePath)
        );
        return;
    }

    m_imageView->setImage(image);
    setWindowTitle(QFileInfo(imagePath).fileName() + " - cbimgv");
}