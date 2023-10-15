#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>


class SplashScreen : public QSplashScreen
{
    Q_OBJECT

public:
    SplashScreen(const QString &pixmapPath, const QString &version, int versionX, int versionY);
    virtual ~SplashScreen();

public slots:
    void showMessage(const QString &msg);
    void clearMessage();
};

#endif
