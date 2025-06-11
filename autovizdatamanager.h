#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QFileSystemWatcher>

class AutoVizDataManager : public QObject {
    Q_OBJECT

public:
    explicit AutoVizDataManager(QObject* parent = nullptr);
    void loadOrConnect(QString const &filePath, QString const &projectName);
    void parseAndWriteToCSV(const QByteArray& jsonData);
    bool loadFromCSV();
    void runPythonServer(const QString &projectName);
    void watchConfig();
    void connectToSocket(int port);

    QTcpSocket* socket;
    QFile csvFile;
    QTextStream csvStream;
    QProcess* pythonProcess;
    QFileSystemWatcher* configWatcher;
    void stopServer();
    bool csvHeaderWritten = false;

private slots:
    void onReadyRead();
    void onConfigChanged(const QString& path);

private:
};
