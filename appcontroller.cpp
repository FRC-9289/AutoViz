#include "AppController.h"
#include "managejson.h"
#include <QDebug>
#include "robot.h"

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
    QString filePath = "backend/projects.json";  // example path to your JSON file
    QJsonObject jsonObj = readJson(filePath);

    return jsonObj.keys();
}

void AppController::processCSV(const QString& projectName) {
    connect(dataManager, &AutoVizDataManager::projectDataReady, this,
            [this](const AutoVizDataManager::ProjectData& projectData) {

                QVariantMap map;
                QVariantList vx, vy, omega, ts;

                for (double val : projectData.v_x) vx.append(val);
                for (double val : projectData.v_y) vy.append(val);
                for (double val : projectData.omega) omega.append(val);
                for (double val : projectData.ts) ts.append(val);

                map["v_x"] = vx;
                map["v_y"] = vy;
                map["omega"] = omega;
                map["ts"] = ts;

                emit this->csvProcessed(map);  // <-- create this signal and use in QML
            });

    dataManager->processCSV(projectName);
}


QVariantMap AppController::getCSV(const QString projectName) {
    AutoVizDataManager::ProjectData projectData = dataManager->getCSV(projectName);

    QVariantMap map;
    QVariantList vx, vy, omega, ts;

    for (double val : projectData.v_x) vx.append(val);
    for (double val : projectData.v_y) vy.append(val);
    for (double val : projectData.omega) omega.append(val);
    for (double val : projectData.ts) ts.append(val);

    map["v_x"] = vx;
    map["v_y"] = vy;
    map["omega"] = omega;
    map["ts"] = ts;

    return map;
}

void AppController::stopServer() {
    dataManager->stopServer();
}

double AppController::degreesToRadians(double angle) { return qDegreesToRadians(angle);}
double AppController::radiansToDegrees(double angle) { return qRadiansToDegrees(angle);}

void AppController::updateRobot(double v_x, double v_y, double omega, double dt, Robot *robot){
    robot->setVelocity(v_x, v_y, omega);    // 1. Set the new robot-relative velocity
    robot->updateHeading(omega, dt);        // 2. Update heading (based on angular velocity and duration)
    robot->updateTime(dt);                  // 4. Update simulation time
}

double AppController::getHeading(Robot *robot) { if(robot) return robot->getHeading();}
void AppController::setHeading(double angle, Robot *robot) {if(robot) robot->setHeading(angle);} //Radian

QVariantList AppController::getRobotRelativeVelocity(Robot *robot) {
    Eigen::Vector2d relativeVelocities = robot->getRelativeVelocity();
    QVariantList velocity;
    velocity.append(relativeVelocities.x()); velocity.append(relativeVelocities.y());

    return velocity;
}
