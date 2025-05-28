#include "NTManager.h"
#include "managejson.h"

#include <QProcess>
#include <QDir>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QHostAddress>
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>
#include <QThread>
#include <Eigen/Dense>

#include <QtGui/qvectornd.h>
#include <cmath>

#include <QFileSystemWatcher>
#include "SwerveModule.h"

#include <QElapsedTimer>

void NTManager::startServer() {
    if (process->state() == QProcess::Running) {
        process->terminate();
        process->waitForFinished();
    }

    QString appDirPath = QCoreApplication::applicationDirPath();
    QDir dir(appDirPath);
    dir.cd("../../../../../NTData");

    QString finalDir = dir.absolutePath();
    QString scriptPath = dir.absoluteFilePath("main.py");

    qInfo() << "Final working directory:" << finalDir;
    qInfo() << "Script full path:" << scriptPath;
    qInfo() << "Script exists:" << QFile::exists(scriptPath);

    process->setWorkingDirectory(finalDir);
    process->setProcessChannelMode(QProcess::MergedChannels);

    process->start("python3", QStringList() << scriptPath);

    if (!process->waitForStarted()) {
        qWarning() << "Failed to start process:" << process->errorString();
    }
}

void NTManager::onReadyRead() {
    buffer.append(socket->readAll());

    int newlineIndex;
    while ((newlineIndex = buffer.indexOf('\n')) != -1) {
        QByteArray message = buffer.left(newlineIndex);
        buffer.remove(0, newlineIndex + 1);

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(message, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "JSON parse error:" << parseError.errorString();
            continue;
        }

        if (!doc.isObject()) {
            qWarning() << "Received JSON is not an object";
            continue;
        }

        QJsonObject obj = doc.object();
        QStringList moduleNames = obj.keys();
        if (moduleNames.contains("time")) moduleNames.removeAll("time");

        if (moduleNames.size() != 4) {
            qWarning() << "Expected 4 modules, got" << moduleNames.size();
            continue;
        }

        std::array<Kinematics::ModuleData, 4> modules;
        int i = 0;
        for (const QString& moduleName : moduleNames) {
            QJsonObject modObj = obj[moduleName].toObject();
            double angle = modObj["angle"].toDouble();
            double velocity = modObj["velocity"].toDouble();
            qInfo() << "Module " << moduleName << "Angle: " << angle << "| Velocity: " << velocity;
            modules[i++] = {qDegreesToRadians(angle), velocity};
        }

        Kinematics::Output result = kinematics->estimate(modules);

        // Logging for debugging
        qInfo() << "Omega (deg/s): " << qRadiansToDegrees(result.omega);

        // Store angular velocity in radians internally
        robot->setRobotOmega(qRadiansToDegrees(result.omega));
        robot->setRobotVelocity(Eigen::Vector2d(result.v_x, result.v_y));

        // Update heading using omega in radians and dt = 0.02 s
        robot->updateHeading(0.02);

        emit moduleDataReceived(obj);

        qInfo() << "Heading: " << robot->getHeading();
    }
}


void NTManager::stopServer() {
    stopPolling();

    if (process->state() == QProcess::Running) {
        qInfo() << "Terminating Python process.";
        process->terminate();
        process->waitForFinished(3000);
    }

    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
    }
}

void NTManager::connectToNTActual() {
    QString configPath = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../../../../../NTData/config.json");

    if (!QFile::exists(configPath)) {
        qWarning() << "Config file not found at:" << configPath;
        return;
    }

    QJsonObject config = readJson(configPath);
    int port = config.value("free-port").toInt();

    qInfo() << "Connecting to localhost @ port:" << port;
    socket->connectToHost(QHostAddress::LocalHost, port);

    if (socket->waitForConnected(3000)) {
        qInfo() << "Connected!";
    } else {
        qWarning() << "Unable to connect to NetworkTables TCP socket";
    }
}
