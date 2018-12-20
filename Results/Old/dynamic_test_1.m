A = dlmread('test_18-12-18_3.txt')
B = dlmread('test_18-12-18_4.txt')

A(:,3) = -1*A(:,3)/10000;
B(:,3) = -1*B(:,3)/10000;
A(:,4) = -1*A(:,4)/10000;
B(:,4) = -1*B(:,4)/10000;

A(:,7) = A(:,7)/100;
B(:,7) = B(:,7)/100;
A(:,8) = A(:,8)/100;
B(:,8) = B(:,8)/100;

%{
figure;
plot(A(:,7),A(:,3));

figure;
plot(B(:,8),B(:,4));
%}

subplot(2,1,1)
plot(B(:,7), 'LineWidth', 2)
xlim([0 2000])
ylim([0 3])
title('Dynamic test (a) -- forward direction')
%xlabel('Time')
ylabel('Distance to obstacle (m)')
grid on
grid minor

subplot(2, 1 ,2)
plot(B(:,3), 'LineWidth', 2)
xlim([0 2000])
ylim([0 1.5])
%xTick([0 500 1000 1500 2000])
%xTickLabels({'0','5','10', '15', '20'})
xlabel('Time (s)')
ylabel('Feedback force produced')
grid on
grid minor

figure;
subplot(2, 1, 1)
plot(B(:,8), 'LineWidth', 2)
xlim([0 2000])
ylim([0 3])
title('Dynamic test (b) -- right side')
%xlabel('Time')
ylabel('Distance to obstacle (m)')
grid on
grid minor

subplot(2, 1 ,2)
plot(B(:,4), 'LineWidth', 2)
xlim([0 2000])
ylim([0 1.5])
%xTick([0 500 1000 1500 2000])
%xTickLabels({'0','5','10', '15', '20'})
xlabel('Time (s)')
ylabel('Feedback force produced')
grid on
grid minor
%{
subplot(2,3,3)
plot(C(:,6), 'LineWidth', 2)
xlim([0 2000])
ylim([0 2.5])
title('Dynamic test (c)')
%xlabel('Time')
ylabel('Distance to obstacle (m)')
grid on
grid minor

subplot(2, 3 ,6)
plot(C(:,3), 'LineWidth', 2)
xlim([0 2000])
ylim([0 4])
%xTick([0 500 1000 1500 2000])
%xTickLabels({'0','5','10', '15', '20'})
xlabel('Time (s)')
ylabel('Feedback force produced')
grid on
grid minor

%}