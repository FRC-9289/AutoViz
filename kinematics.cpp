#include "kinematics.h"

kinematics::kinematics(QObject *parent) : QObject(parent) {}

QVector2D kinematics::computeRobotVelocity(const QJsonObject &modules) const {
    double sumX = 0.0;
    double sumY = 0.0;

    const QStringList keys = { "FL", "FR", "BL", "BR" };

    for (const QString &key : keys) {
        if (!modules.contains(key) || !modules[key].isObject())
            continue;

        QJsonObject moduleData = modules.value(key).toObject();
        double velocity = moduleData.value("velocity").toDouble();
        double angle = moduleData.value("angle").toDouble();

        double angleRad = qDegreesToRadians(angle);
        sumX += velocity * std::cos(angleRad);
        sumY += velocity * std::sin(angleRad);
    }

    return QVector2D(sumX, sumY);  // Or sumX/4, sumY/4 if averaging
}
