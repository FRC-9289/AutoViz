#ifndef NTMANAGER_H
#define NTMANAGER_H

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

#include <QtGui/qvectornd.h>
#include <cmath>

class NTManager : public QObject {
    Q_OBJECT

public:
    explicit NTManager(QObject *parent = nullptr)
        : QObject(parent),
        process(new QProcess(this)),
        socket(new QTcpSocket(this)),
        pollTimer(new QTimer(this))
    {
        connect(socket, &QTcpSocket::readyRead, this, &NTManager::onReadyRead);
        connect(pollTimer, &QTimer::timeout, this, &NTManager::pollNetworkTables);
    }

    ~NTManager() {
        stopServer();
    }

signals:
    void moduleDataReceived(QJsonObject data);

public slots:
    void startServer() {
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

    void connectToNT() {
        // Use a single shot timer instead of blocking sleep
        QTimer::singleShot(7000, this, &NTManager::connectToNTActual);
    }

    void connectToNTActual() {
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

    void startPolling(int intervalMs = 1) {
        pollTimer->start(intervalMs);
    }

    void stopPolling() {
        pollTimer->stop();
    }

    void stopServer() {
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

    QVector2D computeRobotVelocity(const QJsonObject &modules) const {
        double sumX = 0.0;
        double sumY = 0.0;

        const QStringList keys = { "FL", "FR", "BL", "BR" };

        for (const QString &key : keys) {
            QJsonObject moduleData = modules.value(key).toObject();
            double velocity = moduleData.value("velocity").toDouble();
            double angle = moduleData.value("angle").toDouble();

            double angleRad = angle * (M_PI / 180.0);
            double vx = velocity * std::cos(angleRad);
            double vy = velocity * std::sin(angleRad);

            sumX += vx;
            sumY += vy;
        }

        return QVector2D(sumX, sumY);
    }

private slots:
    void onReadyRead() {
        buffer.append(socket->readAll());

        int newlineIndex;
        // Process all complete messages
        while ((newlineIndex = buffer.indexOf('\n')) != -1) {
            QByteArray message = buffer.left(newlineIndex);
            buffer.remove(0, newlineIndex + 1); // Remove message + newline

            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(message, &parseError);

            if (parseError.error != QJsonParseError::NoError) {
                qWarning() << "JSON parse error:" << parseError.errorString();
                continue;
            }

            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                qDebug() << "Received JSON:" << obj;
                emit moduleDataReceived(obj);
            }
        }
    }

    void pollNetworkTables() {
        // This slot can be used to do periodic polling if needed
        // For now, you can leave it empty or implement if needed
    }

private:
    QProcess *process;
    QTcpSocket *socket;
    QByteArray buffer;
    QTimer *pollTimer;
};

#endif // NTMANAGER_H
