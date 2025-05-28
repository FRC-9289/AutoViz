#ifndef NTMANAGER_H
#define NTMANAGER_H

#include "managejson.h"
#include "Kinematics.h"

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

#include <QFileSystemWatcher>

#include "robot.h"
#include "Kinematics.h"

class NTManager : public QObject {
    Q_OBJECT

public:
    explicit NTManager(QObject *parent = nullptr)
        : QObject(parent),
        process(new QProcess(this)),
        socket(new QTcpSocket(this)),
        pollTimer(new QTimer(this)),
        prevVelocities{{0.0,0.0,0.0,0.0}}
    {
        connect(socket, &QTcpSocket::readyRead, this, &NTManager::onReadyRead);
        if(!robot){
            robot = new Robot();
        }

        if(!kinematics){
            kinematics = new Kinematics(1,1); //Dummy Values
        }
        watcher.addPath(QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../../../../../NTData/config.json"));
    }

    ~NTManager() {
        stopServer();
        delete process;
        delete socket;
        delete pollTimer;
        delete kinematics;
        delete robot;
    }

signals:
    void moduleDataReceived(QJsonObject data);

public slots:
    void startServer();

    void connectToNT() {/* Use a single shot timer instead of blocking sleep*/ connect(&watcher, &QFileSystemWatcher::fileChanged, this, &NTManager::connectToNTActual);}

    void connectToNTActual();

    void startPolling(int intervalMs = 1) {pollTimer->start(intervalMs);}

    void stopPolling() {pollTimer->stop();}

    void stopServer();


    void onReadyRead();

public:
    QProcess *process;
    QTcpSocket *socket;
    QByteArray buffer;
    QTimer *pollTimer;
    QFileSystemWatcher watcher;
    Kinematics *kinematics = nullptr;
    std::array<float, 4> prevVelocities;
    Robot *robot = nullptr;
};

#endif // NTMANAGER_H
