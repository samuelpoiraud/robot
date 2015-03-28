clear all
close all
clc

a = instrfind;
delete(a);

NB_VAL_VIEW = 60;

s = serial('COM11'); %assigns the object s to serial port
set(s, 'InputBufferSize', 1024); %number of bytes in input buffer
set(s, 'FlowControl', 'hardware');
set(s, 'BaudRate', 9600);
set(s, 'Parity', 'none');
set(s, 'DataBits', 8);
set(s, 'StopBit', 1);
set(s, 'Timeout',10);
TSOP = zeros(1000, 16);
dist = zeros(1000, 1);
angle = zeros(1000, 1);
maxi = zeros(1000, 1);
moy = zeros(1000, 1);
time = [0:1:999]';
fopen(s); %opens the serial port

figure('units','normalized','outerposition',[0 0 1 1]);
echant = 1;
while(1)
    while fread(s, 1, 'char') ~= 'm' end
    dist(echant) = fscanf(s, '%d');
    angle(echant) = fscanf(s, '%d')/(3.14*4096)*180;
    value = fscanf(s, '%d');
    TSOP(echant, :) = value';
    maxi(echant) = 0;
    % Recherche des n maximum
    for y=1:3
        [v, id] = max(value);
        maxi(echant) = maxi(echant) + v;
        value(id) = 0;
    end
    if echant > 5
        moy(echant) = (maxi(echant)+moy(echant-3)+moy(echant-2)+moy(echant-1))/4;
    end  
    moy(echant);
    
    subplot(2, 2,  1);
    plot(time, TSOP, '-', time, maxi, '-.', time, moy, '-.');
    if(echant <= NB_VAL_VIEW)
        dis = TSOP(1:NB_VAL_VIEW, :);
        dis2 = maxi(1:NB_VAL_VIEW, :);
        dis3 = moy(1:NB_VAL_VIEW, :);
        mx = [max(max(dis))+1, max(max(dis2))+1, max(max(dis3))+1];
        axis([0 NB_VAL_VIEW 0 max(mx)]);
    else
        dis = TSOP(echant-NB_VAL_VIEW:echant, :);
        dis2 = maxi(echant-NB_VAL_VIEW:echant, :);
        dis3 = moy(echant-NB_VAL_VIEW:echant, :);
        mx = [max(max(dis))+1, max(max(dis2))+1, max(max(dis3))+1];
        axis([echant-NB_VAL_VIEW echant 0 max(mx)])
    end
    title('TSOP');
    grid on
    
    subplot(2, 2,  2);
    plot(time, angle, '-');
    if(echant <= NB_VAL_VIEW)
        dis = angle(1:NB_VAL_VIEW, :);
        mx = [max(max(dis))+20];
        my = [min(min(dis))-20];
        axis([0 NB_VAL_VIEW min([min(my), 0]) max(mx)]);
    else
        dis = angle(echant-NB_VAL_VIEW:echant, :);
        mx = [max(max(dis))+20];
        my = [min(min(dis))-20];
        axis([echant-NB_VAL_VIEW echant min([min(my), 0]) max(mx)+20]);
    end
    title('Angle en °');
    grid on
    
    subplot(2, 2,  3);
    if(dist(echant) < 400)
        plot(time, dist, 'r-');
    else
        plot(time, dist, 'b-');
    end
    if(echant <= NB_VAL_VIEW)
        dis = dist(1:NB_VAL_VIEW, :);
        mx = [max(max(dis))+1];
        axis([0 NB_VAL_VIEW 0 max(mx)+100]);
    else
        dis = dist(echant-NB_VAL_VIEW:echant, :);
        mx = [max(max(dis))+1];
        axis([echant-NB_VAL_VIEW echant 0 max(mx)+100]);
    end
    title('Distance en mm');
    grid on
    
    drawnow
    
    if echant == 1000
        echant = 1;
    else
        echant = echant + 1;
    end
end

fclose(s);
delete(s);