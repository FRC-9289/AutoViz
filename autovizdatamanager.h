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
    //When getting v_x, v_y, omega from CSV: this is the struct
    struct ProjectData {
        QList<double> v_x;
        QList<double> v_y;
        QList<double> omega;
        QList<double> ts;
    };



    explicit AutoVizDataManager(QObject* parent = nullptr);
    void loadOrConnect(QString const &filePath, QString const &projectName);
    void parseAndWriteToCSV(const QByteArray& jsonData);
    bool loadFromCSV();
    void runPythonServer(const QString &projectName);
    void watchConfig();
    void connectToSocket(int port);
    void processCSV(const QString& projectName);
    AutoVizDataManager::ProjectData getCSV(const QString& projectName);

    QTcpSocket* socket;
    QFile csvFile;
    QTextStream csvStream;
    QProcess* pythonProcess;
    QFileSystemWatcher* configWatcher;
    void stopServer();
    bool csvHeaderWritten = false;
    QProcess* processCSVProcess = nullptr;

private slots:
    void onReadyRead();
    void onConfigChanged(const QString& path);

private:
};
