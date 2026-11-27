#pragma once

#include <QWidget>
#include <QImage>
#include <QPointF>
#include <QRectF>
#include <QString>

class QPaintEvent;
class QMouseEvent;
class QResizeEvent;
class QWheelEvent;

class ImageView : public QWidget
{
    Q_OBJECT

public:
    explicit ImageView(
        const QImage &image,
        const QString &imagePath,
        QWidget *parent = nullptr
    );

    void setImage(const QImage &image, const QString &imagePath);
    void clearImage();
    void fitToWindow();
    void showOriginalSize();
    void copyToClipboard() const;
    qreal zoomPercentage() const;

signals:
    void zoomChanged(qreal percentage);
    void previousImageRequested();
    void nextImageRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    enum class EdgeNavigation
    {
        None,
        Previous,
        Next
    };

    qreal fitScaleFactor() const;
    qreal scaleFactor() const;
    QRectF imageRect() const;
    bool canPan() const;
    EdgeNavigation edgeNavigationAt(qreal x) const;
    QPointF constrainedPanOffset(const QPointF &offset) const;
    void updateCursor();

    QImage m_image;
    QString m_imagePath;
    qreal m_zoomFactor = 1.0;
    QPointF m_panOffset;
    bool m_fitToWindow = true;
    bool m_isPanning = false;
    QPointF m_lastPanPosition;
    bool m_isDragCandidate = false;
    QPointF m_dragStartPosition;
    EdgeNavigation m_pressedEdgeNavigation = EdgeNavigation::None;
};
