#ifndef ROBOT_H
#define ROBOT_H

#include <Eigen/Dense>
#include <array>
#include <QObject>

class Robot : public QObject {
    Q_OBJECT

public:
    explicit Robot(QObject *parent = nullptr)
        : QObject(parent),
        omega(0.0),
        velocity(Eigen::Vector2d::Zero()),
        isSlipping{{false, false, false, false}}
    {

    }

    Eigen::Vector2d getRobotVelocity() const {
        return velocity;
    }

    void setRobotVelocity(const Eigen::Vector2d &vel) {
        velocity = vel;
    }

    double getRobotOmega() const {
        return omega;
    }

    void setRobotOmega(double om) {
        omega = om;
    }

    std::array<bool, 4> getSlippingStatus() const {
        return isSlipping;
    }

    void setSlippingStatus(const std::array<bool, 4> &status) {
        isSlipping = status;
    }

    double normalizeAngle180(double angle) {
        angle = std::fmod(angle + 180.0, 360.0);
        if (angle < 0)
            angle += 360.0;
        return angle - 180.0;
    }


    void updateHeading(double dt) {
        heading += dt * omega;
        heading = normalizeAngle180(heading);
    }



    double getHeading() {
        return heading;
    }

private:
    double omega;
    Eigen::Vector2d velocity;
    std::array<bool, 4> isSlipping;
    double heading=179.2;
    double coef=1.25;

};

#endif // ROBOT_H
