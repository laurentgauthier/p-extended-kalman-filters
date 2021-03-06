#include "kalman_filter.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;

// Please note that the Eigen library does not initialize 
// VectorXd or MatrixXd objects with zeros upon creation.

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in)
{
    x_ = x_in;
    P_ = P_in;
    F_ = F_in;
    H_ = H_in;
    R_ = R_in;
    Q_ = Q_in;
}

void KalmanFilter::Predict()
{
    x_ = F_ * x_;
    MatrixXd Ft = F_.transpose();
    P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z)
{
    VectorXd z_pred = H_ * x_;
    VectorXd y = z - z_pred;
    MatrixXd Ht = H_.transpose();
    MatrixXd S = H_ * P_ * Ht + R_;
    MatrixXd Si = S.inverse();
    MatrixXd PHt = P_ * Ht;
    MatrixXd K = PHt * Si;

    // New estimate
    x_ = x_ + (K * y);
    long x_size = x_.size();
    MatrixXd I = MatrixXd::Identity(x_size, x_size);
    P_ = (I - K * H_) * P_;
}


#define PI 3.14159265

void KalmanFilter::UpdateEKF(const VectorXd &z)
{
    VectorXd z_pred = VectorXd(3);
    float c1 = x_(0)*x_(0) + x_(1)*x_(1);
    if (c1 < 0.00001) {
        // Prevent division by zero
        c1 = 0.00001;
    }
    z_pred << sqrtf(c1),
              atan2(x_(1), x_(0)),
              (x_(0)*x_(2)+x_(1)*x_(3))/sqrtf(c1);
    VectorXd y = z - z_pred;

    // Make sure the angle is in the [-pi, +pi] range.
    while (y(1) < -PI) {
        y(1) += 2 * PI;
    }
    while (y(1) > PI) {
        y(1) -= 2 * PI;
    }
    // Remaining equations are similar to the linear KF, but H is a jacobian
    MatrixXd Ht = H_.transpose();
    MatrixXd S = H_ * P_ * Ht + R_;
    MatrixXd Si = S.inverse();
    MatrixXd PHt = P_ * Ht;
    MatrixXd K = PHt * Si;

    // New estimate
    x_ = x_ + (K * y);
    long x_size = x_.size();
    MatrixXd I = MatrixXd::Identity(x_size, x_size);
    P_ = (I - K * H_) * P_;
}
