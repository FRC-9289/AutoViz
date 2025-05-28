#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QIcon>
#include <QtQml/QQmlContext>

#include <QApplication>
#include <QMainWindow>

#include "NTManager.h"
#include "loadFileDialog.h"
#include "Kinematics.h"
#include "robot.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);



    QQuickStyle::setStyle("Material");

    app.setWindowIcon(QIcon(":/Logo/app-logo.png"));

    QQmlApplicationEngine engine;

    NTManager ntmanager;
    engine.rootContext()->setContextProperty("NTManager", &ntmanager);

    LoadFileDialog loadFileDialog;
    engine.rootContext()->setContextProperty("LoadFileDialog", &loadFileDialog);

    Robot robot;
    engine.rootContext()->setContextProperty("Robot", &robot);

    engine.load(QUrl(QStringLiteral("qrc:/QML/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
