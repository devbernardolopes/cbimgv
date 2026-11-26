#pragma once

#include <QString>
#include <QStringList>

class ImageNavigator
{
public:
    explicit ImageNavigator(const QString &imagePath);

    QString currentPath() const;
    QString nextPath() const;
    QString previousPath() const;
    bool setCurrentPath(const QString &imagePath);

private:
    QStringList m_imagePaths;
    qsizetype m_currentIndex = 0;
};
