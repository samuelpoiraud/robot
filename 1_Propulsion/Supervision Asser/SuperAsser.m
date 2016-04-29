clear all
close all
clc

a = instrfind;
delete(a);

NB_VALUE = 1000;
NB_VIEW = 60;

s = serial('COM24'); %assigns the object s to serial port
set(s, 'InputBufferSize', 1024); %number of bytes in input buffer
set(s, 'FlowControl', 'hardware');
set(s, 'BaudRate', 230400);
set(s, 'Parity', 'none');
set(s, 'DataBits', 8);
set(s, 'StopBit', 1);
set(s, 'Timeout',10);
ecartTranslation = zeros(NB_VALUE, 1);
commandeTranslation = zeros(NB_VALUE, 1);
ecartRotation = zeros(NB_VALUE, 1);
commandeRotation = zeros(NB_VALUE, 1);
time = [0:1:NB_VALUE-1]';
fopen(s); %opens the serial port

figure('units','normalized','outerposition',[0 0 1 1]);
subplot(2, 1,  1);
title('Translation');
axis([0 NB_VALUE -400 400]);
grid on
subplot(2, 1,  2);
title('Rotation');
axis([0 NB_VALUE -100 100]);
grid on

echant = 1;
display = 0;
while(1)
    while fread(s, 1, 'char') ~= 'm' end
    fscanf(s, '%c');
    
    tempStr = fscanf(s, '%s');
    if(length(str2num(tempStr)) == 1)
        ecartTranslation(echant) = str2num(tempStr);
    end
    
    tempStr = fscanf(s, '%s');
    if(length(str2num(tempStr)) == 1)
        commandeTranslation(echant) = -str2num(tempStr);
    end
    
    tempStr = fscanf(s, '%s');
    if(length(str2num(tempStr)) == 1)
        ecartRotation(echant) = str2num(tempStr);
    end
    
    tempStr = fscanf(s, '%s');
    if(length(str2num(tempStr)) == 1)
        commandeRotation(echant) = str2num(tempStr);
    end
    
    if(display == 3)
        subplot(2, 1,  1);
        plot(time, ecartTranslation, '-', time, commandeTranslation, '-.');
        title('Translation');
        legend('Consigne','Réel')
        if(echant <= NB_VIEW)
            V1min = min(ecartTranslation(1:NB_VIEW, :));
            V1max = max(ecartTranslation(1:NB_VIEW, :));
            V2min = min(commandeTranslation(1:NB_VIEW, :));
            V2max = max(commandeTranslation(1:NB_VIEW, :));
            Vmin = min(V1min, V2min)-1;
            Vmax = max(V1max, V2max)+1;
            axis([0 NB_VIEW Vmin Vmax]);
        else
            V1min = min(ecartTranslation(echant-NB_VIEW:echant, :));
            V1max = max(ecartTranslation(echant-NB_VIEW:echant, :));
            V2min = min(commandeTranslation(echant-NB_VIEW:echant, :));
            V2max = max(commandeTranslation(echant-NB_VIEW:echant, :));
            Vmin = min(V1min, V2min)-1;
            Vmax = max(V1max, V2max)+1;
            axis([echant-NB_VIEW echant Vmin Vmax]);
        end
        grid on

        subplot(2, 1,  2);
        plot(time, ecartRotation, '-', time, commandeRotation, '-.');
        title('Rotation');
        legend('Consigne','Réel')
        if(echant <= NB_VIEW)
            V1min = min(ecartRotation(1:NB_VIEW, :));
            V1max = max(ecartRotation(1:NB_VIEW, :));
            V2min = min(commandeRotation(1:NB_VIEW, :));
            V2max = max(commandeRotation(1:NB_VIEW, :));
            Vmin = min(V1min, V2min)-1;
            Vmax = max(V1max, V2max)+1;
            axis([0 NB_VIEW Vmin Vmax]);
        else
            V1min = min(ecartRotation(echant-NB_VIEW:echant, :));
            V1max = max(ecartRotation(echant-NB_VIEW:echant, :));
            V2min = min(commandeRotation(echant-NB_VIEW:echant, :));
            V2max = max(commandeRotation(echant-NB_VIEW:echant, :));
            Vmin = min(V1min, V2min)-1;
            Vmax = max(V1max, V2max)+1;
            axis([echant-NB_VIEW echant Vmin Vmax]);
        end
        grid on

        drawnow
        display = 0;
    end
    
    if echant == NB_VALUE
        echant = 1;
    else
        echant = echant + 1;
    end
    display = display + 1;
end

fclose(s);
delete(s);