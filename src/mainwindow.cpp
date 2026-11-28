#include "mainwindow.h"
#include "imageview.h"
#include "thumbnailribbon.h"
#include "directorywatcher.h"

#include <QApplication>
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QImageReader>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QList>
#include <QMouseEvent>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QStatusBar>

MainWindow::MainWindow(
    const QImage &image,
    const QString &imagePath,
    QWidget *parent
)
    : QMainWindow(parent),
      m_navigator(imagePath)
{
    setWindowTitle(QFileInfo(imagePath).fileName() + " - cbimgv");
    resize(800, 600);

    m_imageView = new ImageView(image, m_navigator.currentPath());
    m_thumbnailRibbon = new ThumbnailRibbon(this);
    m_thumbnailRibbon->setImages(
        m_navigator.imagePaths(),
        m_navigator.currentIndex()
    );
    m_directoryWatcher = new DirectoryWatcher(this);
    m_directoryWatcher->setDirectory(m_navigator.directoryPath());
    QApplication::instance()->installEventFilter(this);

    auto *centralContainer = new QWidget(this);
    auto *centralLayout = new QVBoxLayout(centralContainer);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralLayout->addWidget(m_imageView, 1);
    centralLayout->addWidget(m_thumbnailRibbon, 0);

    auto *footer = new QWidget(centralContainer);
    auto *footerLayout = new QHBoxLayout(footer);
    footerLayout->setContentsMargins(0, 0, 0, 0);
    footerLayout->setSpacing(0);

    const auto createButton = [footer](const QString &label, bool checkable) {
        auto *button = new QPushButton(label, footer);
        button->setCheckable(checkable);
        return button;
    };

    m_fullscreenButton = createButton("F11", true);
    m_wheelNavigationButton = createButton("Tab", true);
    auto *fitButton = createButton("F", false);
    auto *originalSizeButton = createButton("1", false);
    m_thumbnailRibbonButton = createButton("G", true);
    m_statusBarButton = createButton("S", true);
    auto *closeButton = createButton("Esc", false);
    const QList<QPushButton *> buttons = {
        m_fullscreenButton,
        m_wheelNavigationButton,
        fitButton,
        originalSizeButton,
        m_thumbnailRibbonButton,
        m_statusBarButton,
        closeButton
    };

    m_fileNameLabel = new QLabel;
    m_fileNameLabel->setMinimumWidth(0);
    m_fileNameLabel->setSizePolicy(
        QSizePolicy::Ignored,
        QSizePolicy::Preferred
    );
    m_dimensionsLabel = new QLabel;
    m_dimensionsLabel->setMinimumWidth(0);
    m_dimensionsLabel->setSizePolicy(
        QSizePolicy::Ignored,
        QSizePolicy::Preferred
    );
    m_zoomLabel = new QLabel;
    m_zoomLabel->setMinimumWidth(0);
    m_zoomLabel->setSizePolicy(
        QSizePolicy::Ignored,
        QSizePolicy::Preferred
    );
    m_statusBar = new QStatusBar(footer);
    m_statusBar->addWidget(m_fileNameLabel, 1);
    m_statusBar->addPermanentWidget(m_dimensionsLabel);
    m_statusBar->addPermanentWidget(m_zoomLabel);
    m_statusBar->setMinimumWidth(0);
    m_statusBar->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

    int buttonWidth = 0;
    for (QPushButton *button : buttons) {
        buttonWidth = qMax(buttonWidth, button->sizeHint().width());
    }
    const int buttonHeight = m_statusBar->sizeHint().height();
    for (QPushButton *button : buttons) {
        button->setFixedSize(buttonWidth, buttonHeight);
        if (button == closeButton) {
            footerLayout->addWidget(m_statusBar, 1);
        }
        footerLayout->addWidget(button);
    }
    footer->setFixedHeight(buttonHeight);
    centralLayout->addWidget(footer);
    setCentralWidget(centralContainer);

    connect(
        m_fullscreenButton,
        &QPushButton::clicked,
        this,
        &MainWindow::toggleFullscreen
    );
    connect(
        m_wheelNavigationButton,
        &QPushButton::clicked,
        this,
        &MainWindow::toggleWheelNavigation
    );
    connect(fitButton, &QPushButton::clicked, m_imageView, &ImageView::fitToWindow);
    connect(
        originalSizeButton,
        &QPushButton::clicked,
        m_imageView,
        &ImageView::showOriginalSize
    );
    connect(
        m_thumbnailRibbonButton,
        &QPushButton::clicked,
        this,
        &MainWindow::toggleThumbnailRibbon
    );
    connect(
        m_statusBarButton,
        &QPushButton::clicked,
        this,
        &MainWindow::toggleStatusBar
    );
    connect(closeButton, &QPushButton::clicked, this, &MainWindow::close);

    connect(
        m_imageView,
        &ImageView::previousImageRequested,
        this,
        [this] { navigateTo(m_navigator.previousPath()); }
    );
    connect(
        m_imageView,
        &ImageView::nextImageRequested,
        this,
        [this] { navigateTo(m_navigator.nextPath()); }
    );
    connect(
        m_imageView,
        &ImageView::zoomChanged,
        this,
        &MainWindow::updateZoomStatus
    );
    connect(
        m_thumbnailRibbon,
        &ThumbnailRibbon::imageSelected,
        this,
        [this](const QString &imagePath) { navigateTo(imagePath); }
    );
    connect(
        m_thumbnailRibbon,
        &ThumbnailRibbon::previousImageRequested,
        this,
        [this] { navigateTo(m_navigator.previousPath()); }
    );
    connect(
        m_thumbnailRibbon,
        &ThumbnailRibbon::nextImageRequested,
        this,
        [this] { navigateTo(m_navigator.nextPath()); }
    );
    connect(
        m_directoryWatcher,
        &DirectoryWatcher::directoryChanged,
        this,
        &MainWindow::refreshDirectory
    );
    updateImageStatus(image, imagePath);
    updateZoomStatus(m_imageView->zoomPercentage());
    updateToggleButtons();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Copy)) {
        m_imageView->copyToClipboard();
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Delete
        && (event->modifiers() == Qt::NoModifier
            || event->modifiers() == Qt::ShiftModifier)) {
        deleteCurrentImage(event->modifiers() == Qt::ShiftModifier);
        event->accept();
        return;
    }

    switch (event->key()) {
    case Qt::Key_Escape:
    case Qt::Key_Q:
        close();
        event->accept();
        return;
    case Qt::Key_Tab:
        if (event->modifiers() == Qt::NoModifier) {
            toggleWheelNavigation();
            event->accept();
            return;
        }
        QMainWindow::keyPressEvent(event);
        return;
    case Qt::Key_G:
        toggleThumbnailRibbon();
        event->accept();
        return;
    case Qt::Key_S:
        toggleStatusBar();
        event->accept();
        return;
    case Qt::Key_Plus:
        m_imageView->zoomIn();
        event->accept();
        return;
    case Qt::Key_Equal:
        if (event->modifiers().testFlag(Qt::ShiftModifier)
            || event->text() == QStringLiteral("+")) {
            m_imageView->zoomIn();
            event->accept();
            return;
        }
        QMainWindow::keyPressEvent(event);
        return;
    case Qt::Key_Minus:
        m_imageView->zoomOut();
        event->accept();
        return;
    case Qt::Key_Left:
        if (!m_navigator.imagePaths().isEmpty()) {
            navigateTo(m_navigator.previousPath());
        }
        event->accept();
        return;
    case Qt::Key_Right:
        if (!m_navigator.imagePaths().isEmpty()) {
            navigateTo(m_navigator.nextPath());
        }
        event->accept();
        return;
    case Qt::Key_Home:
        if (!m_navigator.imagePaths().isEmpty()) {
            navigateTo(m_navigator.firstPath());
        }
        event->accept();
        return;
    case Qt::Key_End:
        if (!m_navigator.imagePaths().isEmpty()) {
            navigateTo(m_navigator.lastPath());
        }
        event->accept();
        return;
    case Qt::Key_PageUp:
        navigateTo(m_navigator.pathAtOffset(-10));
        event->accept();
        return;
    case Qt::Key_PageDown:
        navigateTo(m_navigator.pathAtOffset(10));
        event->accept();
        return;
    case Qt::Key_F11:
        toggleFullscreen();
        event->accept();
        return;
    case Qt::Key_F:
        m_imageView->fitToWindow();
        event->accept();
        return;
    case Qt::Key_1:
        m_imageView->showOriginalSize();
        event->accept();
        return;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        updateToggleButtons();
    }
}

void MainWindow::toggleFullscreen()
{
    isFullScreen() ? showNormal() : showFullScreen();
    updateToggleButtons();
}

void MainWindow::toggleWheelNavigation()
{
    m_imageView->toggleWheelNavigation();
    updateToggleButtons();
}

void MainWindow::toggleThumbnailRibbon()
{
    m_thumbnailRibbon->setVisible(m_thumbnailRibbon->isHidden());
    updateToggleButtons();
}

void MainWindow::toggleStatusBar()
{
    m_statusBar->setVisible(m_statusBar->isHidden());
    updateToggleButtons();
}

void MainWindow::updateToggleButtons()
{
    if (!m_fullscreenButton || !m_wheelNavigationButton
        || !m_thumbnailRibbonButton || !m_statusBarButton
        || !m_imageView || !m_thumbnailRibbon || !m_statusBar) {
        return;
    }

    m_fullscreenButton->setChecked(isFullScreen());
    m_wheelNavigationButton->setChecked(
        m_imageView->wheelNavigatesImages()
    );
    m_thumbnailRibbonButton->setChecked(!m_thumbnailRibbon->isHidden());
    m_statusBarButton->setChecked(!m_statusBar->isHidden());
}

void MainWindow::deleteCurrentImage(bool permanently)
{
    const QString imagePath = m_navigator.currentPath();
    if (imagePath.isEmpty()) {
        return;
    }

    QFile file(imagePath);
    const bool succeeded = permanently
        ? file.remove()
        : file.moveToTrash();
    if (!succeeded) {
        QMessageBox::warning(
            this,
            "cbimgv",
            QString("Could not %1 image:\n\n%2\n\n%3")
                .arg(
                    permanently ? "permanently delete" : "move to Trash",
                    imagePath,
                    file.errorString()
                )
        );
        return;
    }

    refreshDirectory();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    auto *widget = qobject_cast<QWidget *>(watched);
    if (watched != this
        && (!widget || (widget != this && !isAncestorOf(widget)))) {
        return QMainWindow::eventFilter(watched, event);
    }

    if (event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            close();
            mouseEvent->accept();
            return true;
        }
        if (mouseEvent->button() == Qt::MiddleButton) {
            toggleWheelNavigation();
            mouseEvent->accept();
            return true;
        }
    } else if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        keyPressEvent(keyEvent);
        return keyEvent->isAccepted();
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::navigateTo(
    const QString &imagePath,
    bool forceReload
)
{
    if (imagePath.isEmpty()) {
        return;
    }
    if (!forceReload && imagePath == m_navigator.currentPath()) {
        return;
    }

    QImageReader reader(imagePath);
    reader.setAutoTransform(true);
    const QImage image = reader.read();
    if (image.isNull()) {
        QMessageBox::warning(
            this,
            "cbimgv",
            QString("Could not open image:\n\n%1\n\n%2")
                .arg(imagePath, reader.errorString())
        );
        return;
    }

    const QStringList previousPaths = m_navigator.imagePaths();
    if (!m_navigator.setCurrentPath(imagePath)) {
        QMessageBox::warning(
            this,
            "cbimgv",
            QString("Could not navigate to image:\n\n%1").arg(imagePath)
        );
        return;
    }

    m_imageView->setImage(image, imagePath);
    m_directoryWatcher->setDirectory(m_navigator.directoryPath());
    if (m_navigator.imagePaths() != previousPaths) {
        m_thumbnailRibbon->setImages(
            m_navigator.imagePaths(),
            m_navigator.currentIndex()
        );
    } else {
        m_thumbnailRibbon->setSelectedIndex(m_navigator.currentIndex());
    }
    setWindowTitle(QFileInfo(imagePath).fileName() + " - cbimgv");
    updateImageStatus(image, imagePath);
}

void MainWindow::refreshDirectory()
{
    if (m_directoryWatcher->directory()
        != m_navigator.directoryPath()) {
        m_directoryWatcher->setDirectory(m_navigator.directoryPath());
    }

    const QStringList previousPaths = m_navigator.imagePaths();
    const bool selectionChanged = m_navigator.refresh();
    const QStringList refreshedPaths = m_navigator.imagePaths();

    if (refreshedPaths.isEmpty()) {
        m_imageView->clearImage();
        m_thumbnailRibbon->setImages({}, -1);
        setWindowTitle("cbimgv");
        m_fileNameLabel->clear();
        m_dimensionsLabel->clear();
        m_zoomLabel->clear();
        return;
    }

    if (selectionChanged) {
        m_thumbnailRibbon->setImages(
            refreshedPaths,
            m_navigator.currentIndex()
        );
        navigateTo(m_navigator.currentPath(), true);
        return;
    }

    if (refreshedPaths != previousPaths) {
        m_thumbnailRibbon->setImages(
            refreshedPaths,
            m_navigator.currentIndex()
        );
    } else {
        m_thumbnailRibbon->setSelectedIndex(
            m_navigator.currentIndex()
        );
    }
}

void MainWindow::updateImageStatus(
    const QImage &image,
    const QString &imagePath
)
{
    m_fileNameLabel->setText(QFileInfo(imagePath).fileName());
    m_dimensionsLabel->setText(
        QString("%1 x %2").arg(image.width()).arg(image.height())
    );
}

void MainWindow::updateZoomStatus(qreal percentage)
{
    m_zoomLabel->setText(QString("%1%").arg(qRound(percentage)));
}