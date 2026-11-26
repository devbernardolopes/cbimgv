#include <QApplication>
#include <QImageReader>
#include <QMessageBox>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("cbimgv");

    // Check whether an image filename was provided.
    if (app.arguments().size() < 2)
    {
        QMessageBox::critical(
            nullptr,
            "cbimgv",
            "No image file specified.\n\n"
            "Usage: cbimgv <image-file>"
        );

        return 1;
    }

    const QString imagePath = app.arguments().at(1);

    // Load the image.
    QImageReader reader(imagePath);

    // Apply image orientation metadata (EXIF).
    reader.setAutoTransform(true);

    const QImage image = reader.read();

    // Check whether loading succeeded.
    if (image.isNull())
    {
        QMessageBox::critical(
            nullptr,
            "cbimgv",
            QString("Could not open image:\n\n%1\n\n%2")
                .arg(imagePath, reader.errorString())
        );

        return 1;
    }

    // Create and display the main window.
    MainWindow window(image, imagePath);

    window.show();

    return app.exec();
}