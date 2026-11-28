#include "thumbnailribbon.h"

#include <QAbstractItemDelegate>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QIcon>
#include <QImageReader>
#include <QListWidgetItem>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollBar>
#include <QSet>
#include <QShowEvent>
#include <QTimer>

namespace
{
constexpr int BonusHeight = 32;
constexpr int RibbonHeight = 72 + BonusHeight;
constexpr int ThumbnailWidth = 80;
constexpr int ThumbnailHeight = 56 + BonusHeight;
constexpr int ItemWidth = 88;
constexpr int ItemHeight = 64 + BonusHeight;

class ThumbnailDelegate : public QAbstractItemDelegate
{
public:
    void paint(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
    ) const override
    {
        painter->save();
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->fillRect(option.rect, option.palette.base());

        const QIcon icon =
            qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        if (!icon.isNull()) {
            const QPixmap pixmap = icon.pixmap(
                QSize(ThumbnailWidth, ThumbnailHeight)
            );
            const QRect target = pixmap.rect();
            QPoint position(
                option.rect.left() + (option.rect.width() - target.width()) / 2,
                option.rect.top() + (option.rect.height() - target.height()) / 2
            );
            painter->drawPixmap(position, pixmap);
        }

        if (option.state & QStyle::State_Selected) {
            painter->setPen(QPen(option.palette.highlight(), 2));
            painter->drawRect(option.rect.adjusted(1, 1, -2, -2));
        }

        painter->restore();
    }

    QSize sizeHint(
        const QStyleOptionViewItem &,
        const QModelIndex &
    ) const override
    {
        return {ItemWidth, ItemHeight};
    }
};
}

ThumbnailRibbon::ThumbnailRibbon(QWidget *parent)
    : QWidget(parent),
      m_thumbnailCache(32 * 1024 * 1024)
{
    setFixedHeight(RibbonHeight);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    m_previousButton = new QPushButton(QStringLiteral("<"), this);
    m_nextButton = new QPushButton(QStringLiteral(">"), this);
    m_previousButton->setFixedSize(28, 56 + BonusHeight);
    m_nextButton->setFixedSize(28, 56 + BonusHeight);
    m_previousButton->setFocusPolicy(Qt::NoFocus);
    m_nextButton->setFocusPolicy(Qt::NoFocus);

    m_list = new QListWidget(this);
    m_list->setFlow(QListView::LeftToRight);
    m_list->setWrapping(false);
    m_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_list->setIconSize(QSize(ThumbnailWidth, ThumbnailHeight));
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    m_list->setFocusPolicy(Qt::NoFocus);
    m_list->setItemDelegate(new ThumbnailDelegate);
    m_list->setUniformItemSizes(true);
    m_list->setFixedHeight(64 + BonusHeight);

    layout->addWidget(m_previousButton);
    layout->addWidget(m_list, 1);
    layout->addWidget(m_nextButton);

    connect(m_previousButton, &QPushButton::clicked, this, &ThumbnailRibbon::previousImageRequested);
    connect(m_nextButton, &QPushButton::clicked, this, &ThumbnailRibbon::nextImageRequested);
    connect(m_list, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        const qsizetype index = m_list->row(item);
        if (index >= 0 && index < m_paths.size()) {
            m_selectedIndex = index;
            emit imageSelected(m_paths.at(index));
        }
    });
    connect(
        m_list->horizontalScrollBar(),
        &QScrollBar::valueChanged,
        this,
        &ThumbnailRibbon::updateVisibleThumbnails
    );

}

void ThumbnailRibbon::setImages(
    const QStringList &paths,
    qsizetype currentIndex
)
{
    invalidateChangedCache(paths);
    m_paths = paths;
    m_selectedIndex = currentIndex;
    m_list->clear();

    for (const QString &path : m_paths) {
        auto *item = new QListWidgetItem;
        item->setSizeHint(QSize(ItemWidth, ItemHeight));
        if (const QImage *thumbnail = m_thumbnailCache.object(path)) {
            item->setIcon(QPixmap::fromImage(*thumbnail));
        }
        m_list->addItem(item);
    }

    setSelectedIndex(currentIndex);
}

void ThumbnailRibbon::setSelectedIndex(qsizetype index)
{
    if (index < 0 || index >= m_paths.size()) {
        return;
    }

    m_selectedIndex = index;
    m_list->setCurrentRow(static_cast<int>(index));
    centerSelectedThumbnail();
    updateVisibleThumbnails();
}

void ThumbnailRibbon::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    scheduleVisibleThumbnailUpdate();
}

void ThumbnailRibbon::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    scheduleVisibleThumbnailUpdate();
}

void ThumbnailRibbon::scheduleVisibleThumbnailUpdate()
{
    QTimer::singleShot(0, this, [this] {
        centerSelectedThumbnail();
        updateVisibleThumbnails();
    });
}

void ThumbnailRibbon::thumbnailLoaded(
    const QString &path,
    const QImage &thumbnail
)
{
    if (thumbnail.isNull()) {
        return;
    }

    m_thumbnailCache.insert(
        path,
        new QImage(thumbnail),
        thumbnail.sizeInBytes()
    );
    const QFileInfo info(path);
    m_thumbnailStamps.insert(
        path,
        QString("%1:%2")
            .arg(info.size())
            .arg(info.lastModified().toMSecsSinceEpoch())
    );

    const qsizetype index = m_paths.indexOf(path);
    if (index >= 0) {
        QListWidgetItem *item = m_list->item(static_cast<int>(index));
        if (item) {
            item->setIcon(QPixmap::fromImage(thumbnail));
        }
    }
}

void ThumbnailRibbon::loadThumbnail(const QString &path)
{
    if (m_thumbnailCache.contains(path)) {
        return;
    }

    QImageReader reader(path);
    reader.setAutoTransform(true);

    const QSize scaledSize = reader.size().scaled(
        ThumbnailWidth,
        ThumbnailHeight,
        Qt::KeepAspectRatio
    );
    if (scaledSize.isValid() && !scaledSize.isEmpty()) {
        reader.setScaledSize(scaledSize);
    }

    thumbnailLoaded(path, reader.read());
}

void ThumbnailRibbon::invalidateChangedCache(const QStringList &paths)
{
    QSet<QString> validPaths(paths.begin(), paths.end());

    for (auto it = m_thumbnailStamps.begin();
         it != m_thumbnailStamps.end();) {
        if (!validPaths.contains(it.key())) {
            m_thumbnailCache.remove(it.key());
            it = m_thumbnailStamps.erase(it);
            continue;
        }

        const QFileInfo info(it.key());
        const QString stamp =
            QString("%1:%2")
                .arg(info.size())
                .arg(info.lastModified().toMSecsSinceEpoch());
        if (stamp != it.value()) {
            m_thumbnailCache.remove(it.key());
            it = m_thumbnailStamps.erase(it);
            continue;
        }
        ++it;
    }

    for (const QString &path : paths) {
        if (!m_thumbnailStamps.contains(path)
            && m_thumbnailCache.contains(path)) {
            m_thumbnailCache.remove(path);
        }
    }
}

void ThumbnailRibbon::updateVisibleThumbnails()
{
    if (m_paths.isEmpty()
        || m_selectedIndex < 0
        || m_selectedIndex >= m_paths.size()) {
        return;
    }

    loadThumbnail(m_paths.at(m_selectedIndex));

    const QRect viewportRect = m_list->viewport()->rect();
    qsizetype nearestPrevious = m_selectedIndex;
    qsizetype nearestNext = m_selectedIndex;

    for (int index = 0; index < m_list->count(); ++index) {
        const QRect itemRect =
            m_list->visualItemRect(m_list->item(index));
        if (itemRect.intersects(viewportRect)) {
            loadThumbnail(m_paths.at(index));
        }
    }

    if (m_paths.size() > 1) {
        for (qsizetype distance = 1; distance <= 3; ++distance) {
            nearestPrevious =
                (m_selectedIndex - distance) % m_paths.size();
            if (nearestPrevious < 0) {
                nearestPrevious += m_paths.size();
            }
            nearestNext =
                (m_selectedIndex + distance) % m_paths.size();
            loadThumbnail(m_paths.at(nearestPrevious));
            loadThumbnail(m_paths.at(nearestNext));
        }
    }
}

void ThumbnailRibbon::centerSelectedThumbnail()
{
    if (m_selectedIndex < 0 || m_selectedIndex >= m_list->count()) {
        return;
    }

    m_list->scrollToItem(
        m_list->item(static_cast<int>(m_selectedIndex)),
        QAbstractItemView::PositionAtCenter
    );
}
