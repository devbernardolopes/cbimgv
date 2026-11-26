#pragma once

#include <QWidget>
#include <QImage>

class QPaintEvent;

class ImageView : public QWidget
{
public:
    explicit ImageView(const QImage &image, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage m_image;
};
