#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QIcon>
#include <QtQml/QQmlContext>
#include "NTManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");

    app.setWindowIcon(QIcon(":/Logo/app-logo.png"));

    QQmlApplicationEngine engine;

    NTManager ntmanager;
    engine.rootContext()->setContextProperty("NTManager", &ntmanager);

    engine.load(QUrl(QStringLiteral("qrc:/QML/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
