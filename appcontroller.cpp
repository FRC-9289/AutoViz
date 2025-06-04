#include "AppController.h"
#include <QDebug>

AppController::AppController(QObject* parent)
    : QObject(parent), dataManager(new AutoVizDataManager(this)) {}

void AppController::startNewProject(const QString projectName) {
    QString sanitizedName = projectName;
    sanitizedName.replace(' ', '-');  // Qt's built-in method

    qDebug() << "[AppController] Starting new project...";
    dataManager->loadOrConnect(sanitizedName + ".csv");
}

void AppController::stopServer() {
    dataManager->stopServer();
}
