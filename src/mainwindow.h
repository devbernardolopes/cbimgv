#pragma once

#include "imagenavigator.h"

#include <QMainWindow>
#include <QImage>
#include <QString>

class QKeyEvent;
class ImageView;

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

    ImageNavigator m_navigator;
    ImageView *m_imageView = nullptr;
};