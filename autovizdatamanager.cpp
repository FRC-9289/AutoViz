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

void AutoVizDataManager::loadOrConnect(QString const &filePath, QString const &projectName) {

    qDebug() << "Starting AutoViz::loadOrConnect";
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

    runPythonServer(projectName);
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

void AutoVizDataManager::runPythonServer(const QString &projectName) {
    QString ntDataPath = QCoreApplication::applicationDirPath() + "/backend/NTData";
    QDir ntDataDir(ntDataPath);
    if (!ntDataDir.exists()) {
        qWarning() << "NTData directory does not exist at" << ntDataDir.absolutePath();
        return;
    }

    qDebug() << "Starting Python server in" << ntDataDir.absolutePath();
    pythonProcess->setWorkingDirectory(ntDataDir.absolutePath());
    pythonProcess->start("python3", QStringList() << "main.py" << projectName);
}

void AutoVizDataManager::watchConfig() {
    QString ntDataPath = QCoreApplication::applicationDirPath() + "/backend/NTData";
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

    qDebug() << obj["Robot State"].toString();

    if(obj["Robot State"].toString()=="Disabled"){
        AutoVizDataManager::stopServer();
    }

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

void AutoVizDataManager::processCSV(const QString projectName) {
    if (processCSVProcess) {
        processCSVProcess->deleteLater();
    }

    processCSVProcess = new QProcess(this);  // ← you missed this earlier

    // ✅ Now it's safe to configure
    QString workingDir = QCoreApplication::applicationDirPath();
    processCSVProcess->setWorkingDirectory(workingDir + "/backend");

    // Output logging
    connect(processCSVProcess, &QProcess::readyReadStandardOutput, this, [=]() {
        qDebug() << "STDOUT:" << processCSVProcess->readAllStandardOutput();
    });

    connect(processCSVProcess, &QProcess::readyReadStandardError, this, [=]() {
        qDebug() << "STDERR:" << processCSVProcess->readAllStandardError();
    });

    connect(processCSVProcess, &QProcess::started, this, []() {
        qDebug() << "Process started successfully.";
    });

    connect(processCSVProcess, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
        qWarning() << "Process error occurred:" << error;
    });

    connect(processCSVProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [](int exitCode, QProcess::ExitStatus exitStatus) {
                qDebug() << "Process finished with exit code:" << exitCode << "and status:" << exitStatus;
            });
    processCSVProcess->start("./backendcontroller",QStringList() << "process" << projectName);

    if (!processCSVProcess->waitForStarted(3000)) {
        qWarning() << "Failed to start process.";
    }
}


AutoVizDataManager::ProjectData AutoVizDataManager::getCSV(const QString projectName) {
    if (processCSVProcess) {
        processCSVProcess->deleteLater();
    }

    processCSVProcess = new QProcess(this);  // ← you missed this earlier

    // ✅ Now it's safe to configure
    QString workingDir = QCoreApplication::applicationDirPath();
    processCSVProcess->setWorkingDirectory(workingDir + "/backend");

    processCSVProcess->start("./backendcontroller", QStringList() << "get" << projectName);

    if (!processCSVProcess->waitForFinished(3000)) {
        qWarning() << "Failed to run process or timeout occurred.";
        return {};
    }

    QByteArray dataOutput = processCSVProcess->readAllStandardOutput();
    AutoVizDataManager::ProjectData projectData;

    QStringList lines = QString::fromUtf8(dataOutput).trimmed().split('\n');
    for (const QString& line : lines) {
        qDebug() << "Line: " << line;
        QStringList split = line.split(' ');

        projectData.v_x.append(split.at(0).toDouble());
        projectData.v_y.append(split.at(1).toDouble());
        projectData.omega.append(split.at(2).toDouble());
        projectData.ts.append(split.at(3).toDouble());
    }
    qDebug() << "    V_X: " << projectData.v_x;
    qDebug() << "    V_Y: " << projectData.v_y;
    qDebug() << "    Om: : " << projectData.omega;
    qDebug() << "    TS: " << projectData.ts;
    return projectData;
}

void AutoVizDataManager::stopServer() {
    if (pythonProcess && pythonProcess->state() != QProcess::NotRunning) {
        qDebug() << "Stopping Python server...";
        pythonProcess->terminate();
        pythonProcess->waitForFinished(3000);
    }

    if (socket && socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Closing TCP socket...";
        socket->disconnectFromHost();
    }

    if (csvFile.isOpen()) {
        qDebug() << "Closing CSV file.";
        csvFile.close();
    }
}

