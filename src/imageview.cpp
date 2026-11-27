#include "imageview.h"

#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QUrl>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>

namespace
{
constexpr qreal ZoomStep = 1.2;
constexpr qreal MinimumZoomFactor = 0.01;
constexpr qreal MaximumZoomFactor = 1000.0;
constexpr qreal NavigationAreaWidth = 48.0;
}

ImageView::ImageView(
    const QImage &image,
    const QString &imagePath,
    QWidget *parent
)
    : QWidget(parent),
      m_image(image),
      m_imagePath(imagePath)
{
}

void ImageView::setImage(const QImage &image, const QString &imagePath)
{
    m_image = image;
    m_imagePath = imagePath;
    fitToWindow();
}

void ImageView::fitToWindow()
{
    m_fitToWindow = true;
    m_panOffset = {};
    m_isPanning = false;
    m_isDragCandidate = false;
    m_pressedEdgeNavigation = EdgeNavigation::None;
    updateCursor();
    update();
    emit zoomChanged(zoomPercentage());
}

void ImageView::showOriginalSize()
{
    m_fitToWindow = false;
    m_zoomFactor = 1.0;
    m_panOffset = {};
    m_isPanning = false;
    m_isDragCandidate = false;
    m_pressedEdgeNavigation = EdgeNavigation::None;
    updateCursor();
    update();
    emit zoomChanged(zoomPercentage());
}

qreal ImageView::zoomPercentage() const
{
    return scaleFactor() * 100.0;
}

qreal ImageView::fitScaleFactor() const
{
    if (m_image.isNull()) {
        return 1.0;
    }

    const qreal widthScale =
        static_cast<qreal>(width()) / m_image.width();
    const qreal heightScale =
        static_cast<qreal>(height()) / m_image.height();
    return std::min(widthScale, heightScale);
}

qreal ImageView::scaleFactor() const
{
    return m_fitToWindow ? fitScaleFactor() : m_zoomFactor;
}

QRectF ImageView::imageRect() const
{
    const QSizeF scaledSize = QSizeF(m_image.size()) * scaleFactor();
    QRectF targetRect(QPointF(0, 0), scaledSize);
    targetRect.moveCenter(QRectF(rect()).center());
    targetRect.translate(m_panOffset);
    return targetRect;
}

bool ImageView::canPan() const
{
    if (m_image.isNull()) {
        return false;
    }

    const QSizeF scaledSize = QSizeF(m_image.size()) * scaleFactor();
    return scaledSize.width() > width() || scaledSize.height() > height();
}

ImageView::EdgeNavigation ImageView::edgeNavigationAt(qreal x) const
{
    if (x < NavigationAreaWidth) {
        return EdgeNavigation::Previous;
    }
    if (x >= width() - NavigationAreaWidth) {
        return EdgeNavigation::Next;
    }
    return EdgeNavigation::None;
}

QPointF ImageView::constrainedPanOffset(const QPointF &offset) const
{
    const QSizeF scaledSize = QSizeF(m_image.size()) * scaleFactor();
    const qreal horizontalLimit =
        std::max<qreal>(0.0, (scaledSize.width() - width()) / 2.0);
    const qreal verticalLimit =
        std::max<qreal>(0.0, (scaledSize.height() - height()) / 2.0);

    return {
        std::clamp(offset.x(), -horizontalLimit, horizontalLimit),
        std::clamp(offset.y(), -verticalLimit, verticalLimit)
    };
}

void ImageView::updateCursor()
{
    if (m_isPanning) {
        setCursor(Qt::ClosedHandCursor);
    } else if (canPan()) {
        setCursor(Qt::OpenHandCursor);
    } else {
        unsetCursor();
    }
}

void ImageView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.fillRect(rect(), palette().brush(QPalette::Window));

    if (m_image.isNull()) {
        return;
    }

    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawImage(imageRect(), m_image);
}

void ImageView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        const EdgeNavigation edge = edgeNavigationAt(event->position().x());
        if (edge == EdgeNavigation::Previous) {
            emit previousImageRequested();
        } else if (edge == EdgeNavigation::Next) {
            emit nextImageRequested();
        } else if (m_fitToWindow) {
            showOriginalSize();
        } else {
            fitToWindow();
        }
        event->accept();
        return;
    }

    QWidget::mouseDoubleClickEvent(event);
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragCandidate
        && !canPan()
        && (event->buttons() & Qt::LeftButton)
        && (event->position() - m_dragStartPosition).manhattanLength()
            >= QApplication::startDragDistance()) {
        m_isDragCandidate = false;
        m_pressedEdgeNavigation = EdgeNavigation::None;

        QDrag drag(this);
        auto *mimeData = new QMimeData;
        mimeData->setUrls({QUrl::fromLocalFile(m_imagePath)});
        drag.setMimeData(mimeData);
        drag.exec(Qt::CopyAction);

        event->accept();
        return;
    }

    if (m_pressedEdgeNavigation != EdgeNavigation::None) {
        event->accept();
        return;
    }

    if (!m_isPanning) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    const QPointF delta = event->position() - m_lastPanPosition;
    m_panOffset = constrainedPanOffset(m_panOffset + delta);
    m_lastPanPosition = event->position();
    update();
    event->accept();
}

void ImageView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_pressedEdgeNavigation = edgeNavigationAt(event->position().x());
        m_dragStartPosition = event->position();
        m_isDragCandidate = !canPan() && imageRect().contains(event->position());
    }

    if (event->button() == Qt::LeftButton
        && m_pressedEdgeNavigation != EdgeNavigation::None) {
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton && canPan()) {
        m_isPanning = true;
        m_lastPanPosition = event->position();
        updateCursor();
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton
        && m_pressedEdgeNavigation != EdgeNavigation::None) {
        const EdgeNavigation pressedEdge = m_pressedEdgeNavigation;
        m_pressedEdgeNavigation = EdgeNavigation::None;
        m_isDragCandidate = false;
        if (edgeNavigationAt(event->position().x()) == pressedEdge) {
            if (pressedEdge == EdgeNavigation::Previous) {
                emit previousImageRequested();
            } else {
                emit nextImageRequested();
            }
        }
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton && m_isPanning) {
        m_isPanning = false;
        updateCursor();
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton && m_isDragCandidate) {
        m_isDragCandidate = false;
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}

void ImageView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!m_fitToWindow) {
        m_panOffset = constrainedPanOffset(m_panOffset);
    }
    updateCursor();
    emit zoomChanged(zoomPercentage());
}

void ImageView::wheelEvent(QWheelEvent *event)
{
    const int angleDelta = event->angleDelta().y();
    const int pixelDelta = event->pixelDelta().y();
    const qreal currentScale = scaleFactor();
    if (m_image.isNull() || (angleDelta == 0 && pixelDelta == 0)
        || currentScale <= 0.0) {
        QWidget::wheelEvent(event);
        return;
    }

    const qreal zoomSteps = angleDelta != 0
        ? static_cast<qreal>(angleDelta) / 120.0
        : static_cast<qreal>(pixelDelta) / 40.0;
    const qreal requestedScale =
        currentScale * std::pow(ZoomStep, zoomSteps);
    const qreal fitScale = fitScaleFactor();
    const qreal minimumScale =
        std::min(MinimumZoomFactor, fitScale / 100.0);
    const qreal maximumScale =
        std::max(MaximumZoomFactor, fitScale * MaximumZoomFactor);
    const qreal newScale = std::clamp(
        requestedScale,
        minimumScale,
        maximumScale
    );

    m_zoomFactor = newScale;
    m_fitToWindow = false;

    updateCursor();
    update();
    emit zoomChanged(zoomPercentage());
    event->accept();
}
