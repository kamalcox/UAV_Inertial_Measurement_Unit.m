/* uav_filter_simple_random.c
   Simple complementary filter sim with randomized noise each run. */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(void) {
    /* ---- Basic settings ---- */
    const double fs   = 200.0;            // samples per second
    const double dt   = 1.0 / fs;         // seconds per sample
    const double T    = 20.0;             // total time (seconds)
    const int    N    = (int)(T * fs) + 1;

    const double amp_deg = 10.0;          // +/- 10 deg motion
    const double freq_hz = 0.2;           // 0.2 Hz (period 5 s)

    const double gyro_noise_sigma  = 1.0; // deg/s (a bit noisier now)
    const double accel_noise_sigma = 3.0; // deg

    const double alpha = 0.02;            // blending factor

    double true_pitch[5005], true_rate[5005];
    double gyro_meas[5005], accel_meas[5005], est[5005];

    #define URAND2 ((2.0 * rand() / (double)RAND_MAX) - 1.0)

    /* Randomize seed each run */
    srand((unsigned)time(NULL));

    /* Build truth and rate */
    for (int k = 0; k < N; ++k) {
        double t = k * dt;
        true_pitch[k] = amp_deg * sin(2.0 * M_PI * freq_hz * t);
        if (k == 0) {
            true_rate[k] = 0.0;
        } else {
            true_rate[k] = (true_pitch[k] - true_pitch[k-1]) / dt;
        }
    }

    /* Make noisy sensors */
    for (int k = 0; k < N; ++k) {
        gyro_meas[k]  = true_rate[k]  + gyro_noise_sigma  * URAND2;
        accel_meas[k] = true_pitch[k] + accel_noise_sigma * URAND2;
    }

    /* Complementary filter */
    est[0] = accel_meas[0];
    for (int k = 1; k < N; ++k) {
        double gyro_pred = est[k-1] + gyro_meas[k] * dt;
        est[k] = (1.0 - alpha) * gyro_pred + alpha * accel_meas[k];
    }

    /* Compute RMSE */
    double mse = 0.0;
    for (int k = 0; k < N; ++k) {
        double e = est[k] - true_pitch[k];
        mse += e * e;
    }
    double rmse = sqrt(mse / N);

    printf("Complementary Filter RMSE = %.2f deg (20 s @ %.0f Hz)\n", rmse, fs);

    return 0;
}
