#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <QObject>
#include <QJsonObject>
#include <QVector2D>
#include <QStringList>
#include <QtMath>

class kinematics : public QObject
{
    Q_OBJECT

public:
    explicit kinematics(QObject *parent = nullptr);

    Q_INVOKABLE QVector2D computeRobotVelocity(const QJsonObject &modules) const;
};

#endif // KINEMATICS_H
