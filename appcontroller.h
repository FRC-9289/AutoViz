#pragma once

#include <QObject>
#include "autovizdatamanager.h"
#include "robot.h"

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject* parent = nullptr);
    Q_INVOKABLE void stopServer();

    Q_INVOKABLE bool startNewProject(const QString projectName, const QString projectDir);

    Q_INVOKABLE QStringList getProjects(); //Custom
    Q_INVOKABLE QVariantMap processCSV(const QString projectName); //autovizdatamanager
    Q_INVOKABLE QVariantMap getCSV(const QString projectName); //autovizdatamanager

    Q_INVOKABLE void updateHeading(double omega, double dt, Robot *robot); //robot
    Q_INVOKABLE double getHeading(Robot *robot); //robot
    Q_INVOKABLE void setHeading(double angle, Robot *robot);
    Q_INVOKABLE double degreesToRadians(double angle);
    Q_INVOKABLE double radiansToDegrees(double angle);

private:
    AutoVizDataManager* dataManager;
};
