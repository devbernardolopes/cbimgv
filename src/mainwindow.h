#pragma once

#include "imagenavigator.h"

#include <QMainWindow>
#include <QImage>
#include <QString>

class QKeyEvent;
class ImageView;
class QLabel;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(
        const QImage &image,
        const QString &imagePath,
        QWidget *parent = nullptr
    );

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void navigateTo(const QString &imagePath);
    void updateImageStatus(const QImage &image, const QString &imagePath);
    void updateZoomStatus(qreal percentage);

    ImageNavigator m_navigator;
    ImageView *m_imageView = nullptr;
    QLabel *m_fileNameLabel = nullptr;
    QLabel *m_dimensionsLabel = nullptr;
    QLabel *m_zoomLabel = nullptr;
};