#pragma once

#include <QCache>
#include <QImage>
#include <QListWidget>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QWidget>

class QHBoxLayout;
class QPushButton;

class ThumbnailRibbon : public QWidget
{
    Q_OBJECT

public:
    explicit ThumbnailRibbon(QWidget *parent = nullptr);

    void setImages(const QStringList &paths, qsizetype currentIndex);
    void setSelectedIndex(qsizetype index);

signals:
    void imageSelected(const QString &imagePath);
    void previousImageRequested();
    void nextImageRequested();

public slots:
    void thumbnailLoaded(const QString &path, const QImage &thumbnail);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateVisibleThumbnails();

private:
    void loadThumbnail(const QString &path);
    void centerSelectedThumbnail();
    void invalidateChangedCache(const QStringList &paths);

    QListWidget *m_list = nullptr;
    QPushButton *m_previousButton = nullptr;
    QPushButton *m_nextButton = nullptr;
    QStringList m_paths;
    qsizetype m_selectedIndex = -1;
    QCache<QString, QImage> m_thumbnailCache;
    QHash<QString, QString> m_thumbnailStamps;
};
