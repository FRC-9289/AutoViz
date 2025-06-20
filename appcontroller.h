#pragma once

#include <QObject>
#include "autovizdatamanager.h"

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject* parent = nullptr);
    Q_INVOKABLE void stopServer();

    Q_INVOKABLE bool startNewProject(const QString projectName, const QString projectDir);

    Q_INVOKABLE QStringList getProjects();
    Q_INVOKABLE void processCSV(const QString projectName);
    Q_INVOKABLE AutoVizDataManager::ProjectData getCSV(const QString projectName);

private:
    AutoVizDataManager* dataManager;
};
