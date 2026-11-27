#pragma once

#include "imagenavigator.h"

#include <QMainWindow>
#include <QImage>
#include <QString>

class QKeyEvent;
class QEvent;
class ImageView;
class QLabel;
class ThumbnailRibbon;
class DirectoryWatcher;
class QWidget;

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
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void navigateTo(const QString &imagePath, bool forceReload = false);
    void refreshDirectory();
    void updateImageStatus(const QImage &image, const QString &imagePath);
    void updateZoomStatus(qreal percentage);

    ImageNavigator m_navigator;
    ImageView *m_imageView = nullptr;
    ThumbnailRibbon *m_thumbnailRibbon = nullptr;
    DirectoryWatcher *m_directoryWatcher = nullptr;
    QLabel *m_fileNameLabel = nullptr;
    QLabel *m_dimensionsLabel = nullptr;
    QLabel *m_zoomLabel = nullptr;
};