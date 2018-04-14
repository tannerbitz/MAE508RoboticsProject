%% Load Data
data1 = load('sysid_data1.txt');
data2 = load('sysid_data2.txt');

%% Put Data In Vectors
rmotorv1 = data1(:,1);
lmotorv1 = data1(:,2);

rmotortheta1 = data1(:,3);
lmotortheta1 = data1(:,4);

rmotorv2 = data2(:,1);
lmotorv2 = data2(:,2);

rmotortheta2 = data2(:,3);
lmotortheta2 = data2(:,4);

%% Preprocess Data


costerms = data1(:,5);
t = zeros(1,length(rmotorv1));
for i = 1:length(rmotorv1)
   t(i) = 0.05*(i-1);    
end
tdiff = t(1:length(t)-1);

figure(1);
plot(t,rmotorv1, 'b')
hold on
plot(t, rmotortheta1, 'r')
hold off

figure(2)
plot(t(1:length(rm_thetadiff)), rm_thetadiff)



