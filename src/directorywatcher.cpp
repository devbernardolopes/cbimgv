#include "directorywatcher.h"

#include <QDir>

DirectoryWatcher::DirectoryWatcher(QObject *parent)
    : QObject(parent)
{
    m_debounceTimer.setSingleShot(true);
    m_debounceTimer.setInterval(250);
    m_recoveryTimer.setSingleShot(true);
    m_recoveryTimer.setInterval(500);

    connect(
        &m_watcher,
        &QFileSystemWatcher::directoryChanged,
        this,
        &DirectoryWatcher::onDirectoryChanged
    );
    connect(
        &m_debounceTimer,
        &QTimer::timeout,
        this,
        &DirectoryWatcher::onDebounceTimeout
    );
    connect(
        &m_recoveryTimer,
        &QTimer::timeout,
        this,
        &DirectoryWatcher::onRecoveryTimeout
    );
}

void DirectoryWatcher::setDirectory(const QString &directoryPath)
{
    const QString normalized =
        QDir::cleanPath(directoryPath);
    if (normalized == m_directoryPath) {
        ensureWatch();
        return;
    }

    m_debounceTimer.stop();
    m_recoveryTimer.stop();

    if (!m_directoryPath.isEmpty()) {
        const QStringList watched =
            m_watcher.directories();
        if (watched.contains(m_directoryPath)) {
            m_watcher.removePath(m_directoryPath);
        }
    }

    m_directoryPath = normalized;
    ensureWatch();
}

QString DirectoryWatcher::directory() const
{
    return m_directoryPath;
}

void DirectoryWatcher::onDirectoryChanged(const QString &path)
{
    if (path != m_directoryPath) {
        return;
    }

    m_debounceTimer.start();
}

void DirectoryWatcher::onDebounceTimeout()
{
    ensureWatch();
    emit directoryChanged();
}

void DirectoryWatcher::onRecoveryTimeout()
{
    ensureWatch();
    emit directoryChanged();
}

void DirectoryWatcher::ensureWatch()
{
    if (m_directoryPath.isEmpty()) {
        return;
    }

    if (!QDir(m_directoryPath).exists()) {
        m_recoveryTimer.start();
        return;
    }

    m_recoveryTimer.stop();
    if (!m_watcher.directories().contains(m_directoryPath)) {
        m_watcher.addPath(m_directoryPath);
    }
}
