#include "imageview.h"

#include <QPainter>
#include <QPaintEvent>

ImageView::ImageView(const QImage &image, QWidget *parent)
    : QWidget(parent),
      m_image(image)
{
}

void ImageView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // Clear the background.
    painter.fillRect(rect(), palette().brush(QPalette::Window));

    // Calculate the image size while preserving its aspect ratio.
    QSize scaledSize = m_image.size();
    scaledSize.scale(size(), Qt::KeepAspectRatio);

    // Center the image inside the available area.
    QRect targetRect(QPoint(0, 0), scaledSize);
    targetRect.moveCenter(rect().center());

    // Render the image.
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawImage(targetRect, m_image);
}
