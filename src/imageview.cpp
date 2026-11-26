#include "imageview.h"

#include <QPainter>
#include <QPaintEvent>
#include <QRectF>

#include <algorithm>

ImageView::ImageView(const QImage &image, QWidget *parent)
    : QWidget(parent),
      m_image(image)
{
}

void ImageView::setImage(const QImage &image)
{
    m_image = image;
    fitToWindow();
}

void ImageView::fitToWindow()
{
    m_fitToWindow = true;
    update();
}

void ImageView::showOriginalSize()
{
    m_fitToWindow = false;
    m_zoomFactor = 1.0;
    update();
}

void ImageView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.fillRect(rect(), palette().brush(QPalette::Window));

    if (m_image.isNull()) {
        return;
    }

    qreal scaleFactor = m_zoomFactor;
    if (m_fitToWindow) {
        const qreal widthScale =
            static_cast<qreal>(width()) / m_image.width();
        const qreal heightScale =
            static_cast<qreal>(height()) / m_image.height();
        scaleFactor = std::min(widthScale, heightScale);
    }

    const QSizeF scaledSize = QSizeF(m_image.size()) * scaleFactor;
    QRectF targetRect(QPointF(0, 0), scaledSize);
    targetRect.moveCenter(QRectF(rect()).center());

    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawImage(targetRect, m_image);
}
