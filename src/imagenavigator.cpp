#include "imagenavigator.h"

#include <QCollator>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QSet>

#include <algorithm>

ImageNavigator::ImageNavigator(const QString &imagePath)
{
    const QString currentImagePath =
        QDir::cleanPath(QFileInfo(imagePath).absoluteFilePath());

    QSet<QString> supportedExtensions;
    for (const QByteArray &format : QImageReader::supportedImageFormats()) {
        supportedExtensions.insert(QString::fromLatin1(format).toLower());
    }

    const QDir imageDirectory(QFileInfo(currentImagePath).absolutePath());
    const QFileInfoList entries = imageDirectory.entryInfoList(
        QDir::Files | QDir::Readable | QDir::Hidden,
        QDir::NoSort
    );

    for (const QFileInfo &entry : entries) {
        if (supportedExtensions.contains(entry.suffix().toLower())) {
            m_imagePaths.append(QDir::cleanPath(entry.absoluteFilePath()));
        }
    }

    if (!m_imagePaths.contains(currentImagePath)) {
        m_imagePaths.append(currentImagePath);
    }

    QCollator collator;
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    collator.setNumericMode(true);
    std::stable_sort(
        m_imagePaths.begin(),
        m_imagePaths.end(),
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

    m_currentIndex = m_imagePaths.indexOf(currentImagePath);
}

QString ImageNavigator::currentPath() const
{
    return m_imagePaths.at(m_currentIndex);
}

QString ImageNavigator::nextPath() const
{
    const qsizetype nextIndex = (m_currentIndex + 1) % m_imagePaths.size();
    return m_imagePaths.at(nextIndex);
}

QString ImageNavigator::previousPath() const
{
    const qsizetype previousIndex =
        (m_currentIndex + m_imagePaths.size() - 1) % m_imagePaths.size();
    return m_imagePaths.at(previousIndex);
}

bool ImageNavigator::setCurrentPath(const QString &imagePath)
{
    const QString normalizedPath =
        QDir::cleanPath(QFileInfo(imagePath).absoluteFilePath());
    const qsizetype index = m_imagePaths.indexOf(normalizedPath);
    if (index < 0) {
        return false;
    }

    m_currentIndex = index;
    return true;
}
