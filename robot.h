#ifndef ROBOT_H
#define ROBOT_H

#include <Eigen/Dense>
#include <array>
#include <QObject>
#include <QtMath>

class Robot : public QObject {
    Q_OBJECT

public:
    explicit Robot(QObject *parent = nullptr)
        : QObject(parent),
        velocityX(0.0),
        velocityY(0.0),
        omega(0.0),
        isSlipping{{false, false, false, false}},
        heading(qDegreesToRadians(177.9)),
        x(4.51),
        y(6.58),
        time(0.000000)
    {}

    void updateHeading(double angularVelocity, double dt) {
        heading += angularVelocity * dt;
        heading = normalizeAnglePI(heading);
    }

    void updatePosition(double dt) {
        Eigen::Rotation2Dd rot(heading);
        Eigen::Vector2d v_robot(velocityX, velocityY);
        Eigen::Vector2d v_field = rot * v_robot;

        x += v_field.x() * dt;
        y += v_field.y() * dt;
    }

    void updateTime(double dt) {
        time += dt;
    }

    // Set current velocity (robot-relative)
    void setVelocity(double vx, double vy, double w) {
        velocityX = vx;
        velocityY = vy;
        omega = w;
    }

    // Getters
    double getHeading() const { return heading; }
    double getX() const { return x; }
    double getY() const { return y; }
    double getTime() const { return time; }
    double getVelocityX() const { return velocityX; }
    double getVelocityY() const { return velocityY; }
    double getOmega() const { return omega; }
    bool isModuleSlipping(int index) const { return isSlipping[index]; }

    // Set slipping state for a module
    void setModuleSlipping(int index, bool slipping) {
        if (index >= 0 && index < 4) {
            isSlipping[index] = slipping;
        }
    }

    void setVelocityRelative(){
        Eigen::Rotation2D<double> rot(M_PI/2 - heading);
        Eigen::Vector2d v(velocityX, velocityY);
        rot*v; //Work on
    }

private:
    double velocityX;  // Robot-relative velocity X (forward)
    double velocityY;  // Robot-relative velocity Y (left)
    double omega;      // Angular velocity (rad/s)

    std::array<bool, 4> isSlipping;

    double heading;    // Robot heading (radians), normalized to [-π, π]
    double x, y;       // Robot position on field (meters)
    double time;       // Simulation time (seconds)

    // Normalize angle to [-π, π]
    double normalizeAnglePI(double angle) const {
        while (angle > M_PI) angle -= 2.0 * M_PI;
        while (angle <= -M_PI) angle += 2.0 * M_PI;
        return angle;
    }
};

#endif // ROBOT_H
