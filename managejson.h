#ifndef READJSON_H
#define READJSON_H

#include <QJsonObject>
#include <QString>

// Reads a JSON file and returns its root object.
// Optionally accepts an arrayKey if you're extracting a sub-object from an array (not used yet).
QJsonObject readJson(const QString &filePath);
bool editJsonFile(const QString& filePath, const QString& key, const QJsonValue& newValue); //TODO: Write a class that writes to the config.json

#endif // READJSON_H
