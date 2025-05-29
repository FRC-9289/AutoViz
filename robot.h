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
        isSlipping{{false, false, false, false}}
    {

    }

    void setRobotXVelocity(const double &vel) {
        velocityX = vel;
    }

    void setRobotYVelocity(const double &vel) {
        velocityY = vel;
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

    double normalizeAnglePI(double angle) {
        angle = std::fmod(angle + M_PI, 2*M_PI);
        if (angle < 0)
            angle += 2*M_PI;
        return angle - M_PI;
    }


    void updateHeading(double dt) {
        heading += dt * omega*10;
        heading = normalizeAnglePI(heading);
    }

    Eigen::Vector2d getVectorRelComponent(double vel, double rad) {
        auto xComponent = vel*std::cos(rad);
        auto yComponent = vel*std::sin(rad);

        return Eigen::Vector2d(xComponent, yComponent);
    }



    double getHeading() {
        return heading;
    }

    double getRobotXMovement(){
        Eigen::Vector2d resultVelX = getVectorRelComponent(velocityX, heading);
        Eigen::Vector2d resultVelY = getVectorRelComponent(velocityY, heading-M_PI/2);

        return resultVelX.x()+resultVelY.x(); //Total X movement
    }

    double getRobotYMovement(){
        Eigen::Vector2d resultVelX = getVectorRelComponent(velocityX, heading);
        Eigen::Vector2d resultVelY = getVectorRelComponent(velocityY, heading-M_PI/2);

        return resultVelX.y()+resultVelY.y(); //Total X movement
    }

    double getRobotVelocity(){
        return std::hypot(velocityX, velocityY);
    }

    double getRobotXVelocity(){
        return velocityX;
    }

    double getRobotYVelocity(){
        return velocityY;
    }

private:
    double omega;
    double velocityX;
    double velocityY;
    std::array<bool, 4> isSlipping;
    double heading=179.2*M_PI/180;
    double coef=1.25;

};

#endif // ROBOT_H
