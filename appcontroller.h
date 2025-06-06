#pragma once

#include <QObject>
#include "autovizdatamanager.h"

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject* parent = nullptr);
    Q_INVOKABLE void stopServer();

    Q_INVOKABLE void startNewProject(const QString projectName, const QString projectDir);

private:
    AutoVizDataManager* dataManager;
};
