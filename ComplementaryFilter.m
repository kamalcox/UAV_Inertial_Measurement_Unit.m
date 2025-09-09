clear; clc; close all;

% --- Parameters ---
fs = 200;              % Sampling rate (Hz)
dt = 1/fs;             % Time step
T  = 20;               % Total time (seconds)
t  = (0:dt:T)';

% --- True pitch angle (degrees) ---
true_pitch = 10*sin(2*pi*0.2*t);   % smooth sinusoid motion

% --- Simulate gyro (angular rate) ---
true_rate = [0; diff(true_pitch)] / dt;   % derivative
gyro_noise = 0.5*randn(size(t));          % random noise
gyro_meas = true_rate + gyro_noise;       % gyro reading

% --- Simulate accelerometer (angle estimate) ---
accel_noise = 2*randn(size(t));           % noise (degrees)
accel_meas = true_pitch + accel_noise;    % accel angle

% --- Complementary filter ---
alpha = 0.02;   % blending factor (smaller #= trust gyro more, bigger #= trust accelerometer more)
pitch_est = zeros(size(t));
pitch_est(1) = accel_meas(1);   % start with accel reading

for k = 2:length(t)
    gyro_pred = pitch_est(k-1) + gyro_meas(k)*dt;
    pitch_est(k) = (1-alpha)*gyro_pred + alpha*accel_meas(k);
end

% --- Calculate error ---
error = pitch_est - true_pitch;
rmse = sqrt(mean(error.^2));

% --- Plot results ---
figure;
plot(t, true_pitch, 'w', 'LineWidth', 2); hold on;
plot(t, pitch_est, 'b', 'LineWidth', 1.5);
plot(t, accel_meas, 'r:');
legend('True Pitch', 'Estimated Pitch', 'Accel Only');
xlabel('Time (s)');
ylabel('Pitch (deg)');
title(sprintf('Complementary Filter Pitch Estimation (RMSE = %.2f°)', rmse));
grid on;
