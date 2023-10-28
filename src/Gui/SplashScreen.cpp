#include "SplashScreen.h"

#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QApplication>
#include <QScreen>


SplashScreen::SplashScreen
(const QString &pixmapPath, const QString &version, int versionX, int versionY)
: QSplashScreen()
{
    QPixmap pixmap(pixmapPath);
    QScreen *screen = QApplication::primaryScreen();
    if (screen->geometry().width() <= 1280) {
        // Scale pixmap for lower screen resolutions
        int targetPixmapWidth = 420;
        if (screen->geometry().width() < 1024) {
            targetPixmapWidth = 320;
        }
        int origPixmapWidth = pixmap.rect().width();
        int origPixmapHeight = pixmap.rect().height();
        pixmap = pixmap.scaledToWidth(targetPixmapWidth, Qt::SmoothTransformation);
        int newPixmapWidth = pixmap.rect().width();
        int newPixmapHeight = pixmap.rect().height();
        versionX *= newPixmapWidth / static_cast<double>(origPixmapWidth);
        versionY *= newPixmapHeight / static_cast<double>(origPixmapHeight);
    }
    QPainter painter(&pixmap);
    if (version.size() > 0) {
        QFont f = font();
        f.setPointSize(16);
        painter.setFont(f);
        QFontMetrics fm(f);
        QRect versionRect = fm.boundingRect(version);
        painter.drawText(versionX - versionRect.width(), versionY + versionRect.height(), version);
    }
    setPixmap(pixmap);

    show();
}

SplashScreen::~SplashScreen
()
{
    close();
}


void
SplashScreen::showMessage
(const QString &msg)
{
    QSplashScreen::showMessage(msg);
    QApplication::processEvents();
}


void
SplashScreen::clearMessage
()
{
    QSplashScreen::clearMessage();
    QApplication::processEvents();
}
