#include "AppController.h"
#include "managejson.h"
#include <QDebug>

AppController::AppController(QObject* parent)
    : QObject(parent), dataManager(new AutoVizDataManager(this)) {}

bool AppController::startNewProject(const QString projectName, const QString projectDir) {
    QString sanitizedName = projectName;
    sanitizedName.replace(' ', '-');  // Qt's built-in method

    qDebug() << "[AppController] Starting new project...";
    dataManager->loadOrConnect("backend/"+sanitizedName + ".csv", projectName);

    QString filePath = "backend/projects.json";  // example path to your JSON file

    QJsonObject jsonObj = readJson(filePath);

    if(!jsonObj.keys().contains(projectName)){ //If project name doesn't exist already

        // Create the inner "Robot" object
        QJsonObject robotObj;
        robotObj["Length"] = 1;
        robotObj["Width"] = 1;
        robotObj["Mass"] = 50;
        robotObj["Coef"] = 1.2;
        robotObj["module-type"] = "MK4i";

        // Create the outer project object
        QJsonObject projectObj;
        projectObj["Robot"] = robotObj;
        projectObj["Project-Directory"] = projectDir;

        // Wrap in a QJsonValue (QJsonValue can hold a QJsonObject)
        QJsonValue projectValue(projectObj);

        // Consistent: store using sanitizedName
        editJsonFile(filePath, projectName, projectValue);

        qDebug() << "[AppController] Project" << sanitizedName << "added to JSON.";

        qDebug() << jsonObj.keys();

        return true;
    }
    else {
        return false;
    }
}

QStringList AppController::getProjects(){
    QString filePath = "/backend/projects.json";  // example path to your JSON file
    QJsonObject jsonObj = readJson(filePath);

    return jsonObj.keys();
}

void AppController::stopServer() {
    dataManager->stopServer();
}

