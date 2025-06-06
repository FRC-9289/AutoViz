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

    QJsonObject jsonObj;

    // If the file exists, read it
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Could not open file for reading:" << filePath;
            return false;
        }

        QByteArray fileData = file.readAll();
        file.close();

        if (!fileData.isEmpty()) {
            QJsonParseError jsonError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData, &jsonError);

            if (jsonError.error != QJsonParseError::NoError) {
                qWarning() << "JSON parse error:" << jsonError.errorString();
                return false;
            }

            if (jsonDoc.isObject()) {
                jsonObj = jsonDoc.object();
            } else {
                qWarning() << "File does not contain a JSON object.";
                return false;
            }
        } else {
            // Empty file, treat as new empty object
            jsonObj = QJsonObject();
        }
    } else {
        // File does not exist — create new empty object
        jsonObj = QJsonObject();
    }

    // Set the key
    jsonObj[key] = newValue;

    // Now write the file back
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qWarning() << "Could not open file for writing:" << filePath;
        return false;
    }

    QJsonDocument newDoc(jsonObj);
    file.write(newDoc.toJson(QJsonDocument::Indented)); // Optional: pretty print
    file.close();

    return true;
}



