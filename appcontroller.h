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
    Q_INVOKABLE void processCSV(const QString& projectName); //autovizdatamanager
    Q_INVOKABLE QVariantMap getCSV(const QString projectName); //autovizdatamanager

    Q_INVOKABLE double getHeading(Robot *robot); //robot
    Q_INVOKABLE void setHeading(double angle, Robot *robot);
    Q_INVOKABLE double degreesToRadians(double angle);
    Q_INVOKABLE double radiansToDegrees(double angle);
    Q_INVOKABLE void updateRobot(double v_x, double v_y, double omega, double dt, Robot *robot);
    Q_INVOKABLE QVariantList getFieldRelativeVelocity(Robot *robot);

signals:
    void csvProcessed(QVariantMap result);

private:
    AutoVizDataManager* dataManager;
};
