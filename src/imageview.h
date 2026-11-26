#pragma once

#include <QWidget>
#include <QImage>

class QPaintEvent;

class ImageView : public QWidget
{
public:
    explicit ImageView(const QImage &image, QWidget *parent = nullptr);

    void fitToWindow();
    void showOriginalSize();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage m_image;
    qreal m_zoomFactor = 1.0;
    bool m_fitToWindow = true;
};
