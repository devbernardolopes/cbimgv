#include "imagenavigator.h"

#include <QCollator>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QSet>

#include <algorithm>

namespace
{
void sortPaths(QStringList &paths)
{
    QCollator collator;
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    collator.setNumericMode(true);
    std::stable_sort(
        paths.begin(),
        paths.end(),
        [&collator](const QString &left, const QString &right) {
            const int result = collator.compare(
                QFileInfo(left).fileName(),
                QFileInfo(right).fileName()
            );
            if (result != 0) {
                return result < 0;
            }
            return QString::compare(left, right, Qt::CaseSensitive) < 0;
        }
    );
}

QStringList scanDirectory(const QString &directoryPath)
{
    QStringList paths;
    QSet<QString> supportedExtensions;
    for (const QByteArray &format : QImageReader::supportedImageFormats()) {
        supportedExtensions.insert(
            QString::fromLatin1(format).toLower()
        );
    }

    const QFileInfoList entries = QDir(directoryPath).entryInfoList(
        QDir::Files | QDir::Readable | QDir::Hidden,
        QDir::NoSort
    );
    for (const QFileInfo &entry : entries) {
        if (supportedExtensions.contains(entry.suffix().toLower())) {
            paths.append(QDir::cleanPath(entry.absoluteFilePath()));
        }
    }

    sortPaths(paths);
    return paths;
}
}

ImageNavigator::ImageNavigator(const QString &imagePath)
{
    reload(imagePath);
}

void ImageNavigator::reload(const QString &imagePath)
{
    const QString currentImagePath =
        QDir::cleanPath(QFileInfo(imagePath).absoluteFilePath());
    m_directoryPath =
        QDir::cleanPath(QFileInfo(currentImagePath).absolutePath());
    m_imagePaths = scanDirectory(m_directoryPath);
    m_currentIndex = m_imagePaths.indexOf(currentImagePath);
    if (m_currentIndex < 0) {
        m_currentIndex = m_imagePaths.isEmpty() ? -1 : 0;
    }
}

QString ImageNavigator::currentPath() const
{
    if (m_currentIndex < 0 || m_currentIndex >= m_imagePaths.size()) {
        return {};
    }
    return m_imagePaths.at(m_currentIndex);
}

QString ImageNavigator::firstPath() const
{
    return m_imagePaths.isEmpty() ? QString() : m_imagePaths.first();
}

QString ImageNavigator::lastPath() const
{
    return m_imagePaths.isEmpty() ? QString() : m_imagePaths.last();
}

QString ImageNavigator::nextPath() const
{
    if (m_imagePaths.isEmpty()) {
        return {};
    }
    const qsizetype nextIndex =
        (m_currentIndex + 1 + m_imagePaths.size()) % m_imagePaths.size();
    return m_imagePaths.at(nextIndex);
}

QString ImageNavigator::previousPath() const
{
    if (m_imagePaths.isEmpty()) {
        return {};
    }
    const qsizetype previousIndex =
        (m_currentIndex + m_imagePaths.size() - 1) % m_imagePaths.size();
    return m_imagePaths.at(previousIndex);
}

QString ImageNavigator::pathAtOffset(qsizetype offset) const
{
    if (m_imagePaths.isEmpty()) {
        return {};
    }

    const qsizetype lastIndex = m_imagePaths.size() - 1;
    qsizetype targetIndex;
    if ((offset < 0 && m_currentIndex == 0)
        || (offset > 0 && m_currentIndex == lastIndex)) {
        targetIndex = (m_currentIndex + offset) % m_imagePaths.size();
        if (targetIndex < 0) {
            targetIndex += m_imagePaths.size();
        }
    } else {
        targetIndex = std::clamp(
            m_currentIndex + offset,
            qsizetype(0),
            lastIndex
        );
    }

    return m_imagePaths.at(targetIndex);
}

QStringList ImageNavigator::imagePaths() const
{
    return m_imagePaths;
}

qsizetype ImageNavigator::currentIndex() const
{
    return m_currentIndex;
}

QString ImageNavigator::directoryPath() const
{
    return m_directoryPath;
}

bool ImageNavigator::refresh()
{
    const QString previousPath = currentPath();
    const qsizetype previousIndex = m_currentIndex;
    m_imagePaths = scanDirectory(m_directoryPath);

    if (m_imagePaths.isEmpty()) {
        m_currentIndex = -1;
        return previousPath != QString();
    }

    qsizetype preservedIndex = m_imagePaths.indexOf(previousPath);
    if (preservedIndex >= 0) {
        m_currentIndex = preservedIndex;
        return false;
    }

    const qsizetype fallbackIndex =
        std::clamp(previousIndex, qsizetype(0), m_imagePaths.size() - 1);
    m_currentIndex = fallbackIndex;
    return true;
}

bool ImageNavigator::setCurrentPath(const QString &imagePath)
{
    const QString normalizedPath =
        QDir::cleanPath(QFileInfo(imagePath).absoluteFilePath());
    qsizetype index = m_imagePaths.indexOf(normalizedPath);

    if (index < 0) {
        reload(normalizedPath);
        index = m_imagePaths.indexOf(normalizedPath);
    }
    if (index < 0) {
        return false;
    }

    m_currentIndex = index;
    return true;
}
