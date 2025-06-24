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

AutoVizDataManager::ProjectData AutoVizDataManager::processCSV(const QString& projectName) {

    if (processCSVProcess) {
        processCSVProcess->disconnect();
        if (processCSVProcess->state() != QProcess::NotRunning) {
            processCSVProcess->kill();
            processCSVProcess->waitForFinished();
        }
        processCSVProcess->deleteLater();
    }

    processCSVProcess = new QProcess(this);
    QString workingDir = QCoreApplication::applicationDirPath() + "/backend";
    processCSVProcess->setWorkingDirectory(workingDir);

    connect(processCSVProcess, &QProcess::readyReadStandardOutput, this, [=]() {
        qDebug() << "processCSV STDOUT:" << processCSVProcess->readAllStandardOutput();
    });

    connect(processCSVProcess, &QProcess::readyReadStandardError, this, [=]() {
        qDebug() << "processCSV STDERR:" << processCSVProcess->readAllStandardError();
    });

    connect(processCSVProcess, &QProcess::started, this, []() {
        qDebug() << "processCSV started.";
    });

    connect(processCSVProcess, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
        qWarning() << "processCSV error:" << error;
    });

    connect(processCSVProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, projectName](int exitCode, QProcess::ExitStatus status) {
                qDebug() << "processCSV finished with exit code:" << exitCode << " status:" << status;
                AutoVizDataManager::ProjectData projectData = getCSV(projectName);  // run getCSV after processCSV done
                return projectData;
            });

    processCSVProcess->start("./backendcontroller", QStringList() << "process" << projectName);

    if (!processCSVProcess->waitForStarted(3000)) {
        qWarning() << "processCSV failed to start.";
    }
}


AutoVizDataManager::ProjectData AutoVizDataManager::getCSV(const QString& projectName) {
    qDebug() << "Running getCSV for project:" << projectName;

    QProcess getProcess;
    QString workingDir = QCoreApplication::applicationDirPath() + "/backend";
    getProcess.setWorkingDirectory(workingDir);

    getProcess.start("./backendcontroller", QStringList() << "get" << projectName);

    if (!getProcess.waitForStarted(3000)) {
        qWarning() << "getCSV failed to start.";
        return {};
    }

    if (!getProcess.waitForFinished(10000)) {
        qWarning() << "getCSV timed out.";
        return {};
    }

    if (getProcess.exitStatus() != QProcess::NormalExit || getProcess.exitCode() != 0) {
        qWarning() << "getCSV process crashed or failed with code" << getProcess.exitCode();
        return {};
    }

    QByteArray output = getProcess.readAllStandardOutput();
    QByteArray errorOutput = getProcess.readAllStandardError();
    if (!errorOutput.isEmpty()) {
        qWarning() << "getCSV STDERR:" << errorOutput;
    }

    ProjectData projectData;

    if (!output.trimmed().isEmpty()) {
        QStringList lines = QString::fromUtf8(output).trimmed().split('\n', Qt::SkipEmptyParts);
        for (const QString& line : lines) {
            QStringList split = line.split(' ', Qt::SkipEmptyParts);
            if (split.size() >= 4) {
                projectData.v_x.append(split.at(0).toDouble());
                projectData.v_y.append(split.at(1).toDouble());
                projectData.omega.append(split.at(2).toDouble());
                projectData.ts.append(split.at(3).toDouble());
            } else {
                qWarning() << "Malformed line:" << line;
            }
        }
    } else {
        qWarning() << "No output from backendcontroller get.";
    }

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

