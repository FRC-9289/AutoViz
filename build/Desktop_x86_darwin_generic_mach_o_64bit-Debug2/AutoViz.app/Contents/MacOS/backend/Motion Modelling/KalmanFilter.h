// kalman_pose_estimator.cpp
#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <Eigen/Dense>
#include <cmath>
#include <iostream>

class KalmanPoseEstimator {
public:
    KalmanPoseEstimator() {
        state.setZero();
        P.setIdentity();
        Q << 0, 0,    0,
             0,    0, 0,
             0,    0,    0;
    }

    void predict(const Eigen::Vector3d& u, double dt) {
        Eigen::Matrix3d F = computeF(state, u, dt);
        state = motionModel(state, u, dt);
        P = F * P * F.transpose() + Q;
    }

    void correct(const Eigen::Vector2d& z, const Eigen::Matrix2d& R) {
        Eigen::Vector2d z_pred = state.head<2>();
        Eigen::Matrix<double, 2, 3> H;
        H << 1, 0, 0,
             0, 1, 0;

        Eigen::Vector2d y = z - z_pred;
        Eigen::Matrix2d S = H * P * H.transpose() + R;
        Eigen::Matrix<double, 3, 2> K = P * H.transpose() * S.inverse();

        state += K * y;
        P = (Eigen::Matrix3d::Identity() - K * H) * P;
    }

    Eigen::Vector3d getState() const {
        return state;
    }

private:
    Eigen::Vector3d state;       // x, y, theta
    Eigen::Matrix3d P;           // Covariance matrix
    Eigen::Matrix3d Q;           // Process noise

    Eigen::Vector3d motionModel(const Eigen::Vector3d& state, const Eigen::Vector3d& u, double dt) {
        double theta = state(2);
        double cos_theta = std::cos(theta);
        double sin_theta = std::sin(theta);

        double vx_field = u(0) * cos_theta - u(1) * sin_theta;
        double vy_field = u(0) * sin_theta + u(1) * cos_theta;

        Eigen::Vector3d new_state;
        new_state(0) = state(0) + vx_field * dt;
        new_state(1) = state(1) + vy_field * dt;
        new_state(2) = state(2) + u(2) * dt;
        return new_state;
    }

    Eigen::Matrix3d computeF(const Eigen::Vector3d& state, const Eigen::Vector3d& u, double dt) {
        double theta = state(2);
        double cos_theta = std::cos(theta);
        double sin_theta = std::sin(theta);
        double vx = u(0);
        double vy = u(1);

        Eigen::Matrix3d F;
        F << 1, 0, (-vx * sin_theta - vy * cos_theta) * dt,
             0, 1, (vx * cos_theta - vy * sin_theta) * dt,
             0, 0, 1;
        return F;
    }
};

#endif //KALMANFILTER_H