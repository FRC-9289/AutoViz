#include "managejson.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

QJsonObject readJson(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << file.errorString();
        return QJsonObject();  // fail safe
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return QJsonObject();  // fail safe
    }

    if (jsonDoc.isObject()) {
        return jsonDoc.object();
    }

    return QJsonObject(); // catch-all fallback
}

bool editJsonFile(const QString& filePath, const QString& key, const QJsonValue& newValue) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        return false;
    }

    QByteArray fileData = file.readAll();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        file.close();
        return false;
    }

    if (!jsonDoc.isObject()) {
        file.close();
        return false;
    }

    QJsonObject jsonObj = jsonDoc.object();
    jsonObj[key] = newValue;
    jsonDoc.setObject(jsonObj);

    file.seek(0);
    file.write(jsonDoc.toJson());
    file.resize(file.pos()); // Truncate any remaining data
    file.close();
    return true;
}

