#pragma once

#include <QMainWindow>
#include <QImage>
#include <QString>

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(
        const QImage &image,
        const QString &imagePath,
        QWidget *parent = nullptr
    );
};