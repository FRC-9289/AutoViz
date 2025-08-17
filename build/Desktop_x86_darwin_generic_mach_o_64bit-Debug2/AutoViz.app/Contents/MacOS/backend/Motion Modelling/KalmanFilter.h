#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <Eigen/Dense>
#include <cmath>

class KalmanFilter2DRobot {
public:
    KalmanFilter2DRobot(double dt, double process_var = 0.01, double meas_var = 1e-1)
        : dt(dt)
    {
        // State vector: [x, y, heading]
        x = Eigen::Vector3d(6.5, 4.0, M_PI/4.0);

        // Covariance
        P = Eigen::Matrix3d::Identity();

        // Process noise covariance
        Q = process_var * Eigen::Matrix3d::Identity();

        // Measurement matrix
        H = Eigen::Matrix3d::Identity();

        // Measurement noise covariance
        R = meas_var * Eigen::Matrix3d::Identity();

        // Initial state transition
        F = Eigen::Matrix3d::Identity();
    }

    // Predict next state given velocities
    Eigen::Vector3d predict(double vx, double vy, double omega) {
        // Update F (simplified)
        F=Eigen::Matrix3d::Identity();

        // Predict state
        x(0) += vx * dt;
        x(1) += vy * dt;
        x(2) += omega * dt;

        // Predict covariance
        P = F * P * F.transpose() + Q;

        return x;
    }

    // Update state with measurement z
    Eigen::Vector3d update(const Eigen::Vector3d& z) {
        Eigen::Vector3d y = z - H * x;          // Innovation
        Eigen::Matrix3d S = H * P * H.transpose() + R; // Innovation covariance
        Eigen::Matrix3d K = P * H.transpose() * S.inverse(); // Kalman gain

        x = x + K * y;
        P = P - K * H * P;

        return x;
    }

private:
    double dt;
    Eigen::Vector3d x;
    Eigen::Matrix3d P;
    Eigen::Matrix3d F;
    Eigen::Matrix3d Q;
    Eigen::Matrix3d H;
    Eigen::Matrix3d R;
};

#endif // KALMANFILTER_H
