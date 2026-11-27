#pragma once

#include <QString>
#include <QStringList>

class ImageNavigator
{
public:
    explicit ImageNavigator(const QString &imagePath);

    QString currentPath() const;
    QString firstPath() const;
    QString lastPath() const;
    QString nextPath() const;
    QString previousPath() const;
    QStringList imagePaths() const;
    qsizetype currentIndex() const;
    bool setCurrentPath(const QString &imagePath);

private:
    void reload(const QString &imagePath);

    QStringList m_imagePaths;
    qsizetype m_currentIndex = 0;
};
