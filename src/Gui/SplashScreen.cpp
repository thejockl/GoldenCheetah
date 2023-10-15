#include "SplashScreen.h"

#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QApplication>


SplashScreen::SplashScreen
(const QString &pixmapPath, const QString &version, int versionX, int versionY)
: QSplashScreen()
{
    QPixmap pixmap(pixmapPath);
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
