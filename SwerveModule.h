#ifndef SWERVEMODULE_H
#define SWERVEMODULE_H

#include <cmath>

class SwerveModule {
public:

    // Max velocity (m/s) = wheel circumference * wheel rotations per second
    static double maxVelocity(double maxMotorRPM, double gearRatio, double wheelRadius) {
        double wheelRPM = maxMotorRPM / gearRatio;
        double wheelCircumference = 2.0 * M_PI * wheelRadius;
        return wheelCircumference * (wheelRPM / 60.0);
    }

    // Max force (N) = torque at wheel / wheel radius
    static double maxForce(double maxMotorTorque, double gearRatio, double wheelRadius) {
        double wheelTorque = maxMotorTorque * gearRatio; // motor torque * gear ratio
        return wheelTorque / wheelRadius;
    }

    // Max acceleration (m/s²) = (friction force) / (robot mass)
    static double maxAcceleration(double robotMass, double frictionCoefficient) {
        double totalNormalForce = robotMass * 9.81; // W = mg
        double maxFrictionForce = frictionCoefficient * totalNormalForce; //Dampen or extend by scaling by μ
        return maxFrictionForce / robotMass; //Return by a = F/m - how much acceleration to push itself forward
    }
};


#endif //SWERVEMODULE_H
