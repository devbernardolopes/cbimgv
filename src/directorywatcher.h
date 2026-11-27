#pragma once

#include <QFileSystemWatcher>
#include <QObject>
#include <QString>
#include <QTimer>

class DirectoryWatcher : public QObject
{
    Q_OBJECT

public:
    explicit DirectoryWatcher(QObject *parent = nullptr);

    void setDirectory(const QString &directoryPath);
    QString directory() const;

signals:
    void directoryChanged();

private slots:
    void onDirectoryChanged(const QString &path);
    void onDebounceTimeout();
    void onRecoveryTimeout();

private:
    void ensureWatch();

    QFileSystemWatcher m_watcher;
    QTimer m_debounceTimer;
    QTimer m_recoveryTimer;
    QString m_directoryPath;
};
