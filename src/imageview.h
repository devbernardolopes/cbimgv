#pragma once

#include <QWidget>
#include <QImage>
#include <QPointF>
#include <QRectF>

class QPaintEvent;
class QMouseEvent;
class QResizeEvent;
class QWheelEvent;

class ImageView : public QWidget
{
public:
    explicit ImageView(const QImage &image, QWidget *parent = nullptr);

    void setImage(const QImage &image);
    void fitToWindow();
    void showOriginalSize();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    qreal fitScaleFactor() const;
    qreal scaleFactor() const;
    QRectF imageRect() const;
    bool canPan() const;
    QPointF constrainedPanOffset(const QPointF &offset) const;
    void updateCursor();

    QImage m_image;
    qreal m_zoomFactor = 1.0;
    QPointF m_panOffset;
    bool m_fitToWindow = true;
    bool m_isPanning = false;
    QPointF m_lastPanPosition;
};
