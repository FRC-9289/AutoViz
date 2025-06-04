#include "AutoVizDataManager.h"
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileSystemWatcher>
#include <QProcess>
#include <QCoreApplication>

AutoVizDataManager::AutoVizDataManager(QObject* parent)
    : QObject(parent),
    socket(new QTcpSocket(this)),
    pythonProcess(new QProcess(this)),
    configWatcher(new QFileSystemWatcher(this)) {
    connect(socket, &QTcpSocket::readyRead, this, &AutoVizDataManager::onReadyRead);
    connect(configWatcher, &QFileSystemWatcher::fileChanged,
            this, &AutoVizDataManager::onConfigChanged);
}

void AutoVizDataManager::loadOrConnect(QString const &filePath) {
    csvFile.setFileName(filePath);

    if (csvFile.exists()) {
        return;
    }

    // Open for writing before starting anything else
    if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Unable to open CSV for writing.";
        return;
    }
    csvStream.setDevice(&csvFile);

    runPythonServer();
    watchConfig();
}



bool AutoVizDataManager::loadFromCSV() {
    if (!csvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open CSV file.";
        return false;
    }

    QTextStream in(&csvFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        qDebug() << "CSV Line:" << line;
        // TODO: Use this data in your app
    }

    csvFile.close();
    return true;
}

void AutoVizDataManager::runPythonServer() {
    QString ntDataPath = QCoreApplication::applicationDirPath() + "/NTData";
    QDir ntDataDir(ntDataPath);
    if (!ntDataDir.exists()) {
        qWarning() << "NTData directory does not exist at" << ntDataDir.absolutePath();
        return;
    }
    qDebug() << "Starting Python server in" << ntDataDir.absolutePath();
    pythonProcess->setWorkingDirectory(ntDataDir.absolutePath());
    pythonProcess->start("python3", QStringList() << "main.py");
}

void AutoVizDataManager::watchConfig() {
    QString ntDataPath = QCoreApplication::applicationDirPath() + "/NTData";
    QDir ntDataDir(ntDataPath);
    QString configPath = ntDataDir.filePath("config.json");

    if (!QFile::exists(configPath)) {
        qWarning() << "Waiting for config.json to be created at" << configPath;
    }

    if (configWatcher->files().contains(configPath)) {
        configWatcher->removePath(configPath);
    }
    configWatcher->addPath(configPath);
}

void AutoVizDataManager::onConfigChanged(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open config.json at" << path;
        return;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "Invalid JSON in config.json:" << err.errorString();
        return;
    }

    QJsonObject obj = doc.object();
    int port = obj["free-port"].toInt(-1);

    if (port > 0) {
        qDebug() << "Connecting to Python server at port" << port;
        connectToSocket(port);
    } else {
        qWarning() << "Invalid or missing free-port in config.json";
    }
}


void AutoVizDataManager::connectToSocket(int port) {
    qDebug() << "Connecting to port" << port << "...";
    socket->connectToHost("127.0.0.1", port);
}

void AutoVizDataManager::onReadyRead() {
    if (!csvFile.isOpen())
        return; // Don't write if we're not writing to CSV

    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        parseAndWriteToCSV(line);
    }
}


void AutoVizDataManager::parseAndWriteToCSV(const QByteArray& jsonData) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "Invalid JSON:" << err.errorString();
        return;
    }

    QJsonObject obj = doc.object();
    QStringList keys = { "LF", "RF", "LB", "RB" };

    if (!csvHeaderWritten) {
        csvStream << "timestamp";
        for (const QString& key : keys)
            csvStream << "," << key << "_angle," << key << "_velocity";
        csvStream << "\n";
        csvHeaderWritten = true;
    }

    csvStream << obj["timestamp"].toDouble();
    for (const QString& key : keys) {
        QJsonObject m = obj[key].toObject();
        csvStream << "," << m["angle"].toDouble() << "," << m["velocity"].toDouble();
    }

    csvStream << "\n";
    csvStream.flush();
}

void AutoVizDataManager::stopServer() {
    if (pythonProcess && pythonProcess->state() != QProcess::NotRunning) {
        qDebug() << "Stopping Python server...";
        pythonProcess->terminate();
        pythonProcess->waitForFinished(3000);
    }
}

