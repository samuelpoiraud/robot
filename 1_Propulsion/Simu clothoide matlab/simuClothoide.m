function varargout = simuClothoide(varargin)
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @simuClothoide_OpeningFcn, ...
                   'gui_OutputFcn',  @simuClothoide_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end

function simuClothoide_OpeningFcn(hObject, eventdata, handles, varargin)
global tic;                             % s
global VrotInit;                        % rad/s
global VtransInit;                      % mm/s
global alpha;                           % rad
global d;                               % mm
global forward;                         % sens de marche
global coef_acc_rot;                    % rad/s²
global VrotMax;                         % rad/s

tic = 0.005;
VrotInit = pi/2;
VtransInit = 250;
alpha = -pi/8;
d = 300;
forward = true;
coef_acc_rot = 5.3406;
VrotMax = pi;

if(forward)
    set(handles.backwardValue, 'Value', false);
    
    set(handles.distSlide, 'Max', 2000);
    set(handles.distSlide, 'Min', 0);
    set(handles.distSlide, 'Value', d);
    set(handles.distValue, 'String', strcat(num2str(d),' mm'));

    set(handles.VTransSlide, 'Max', 1500);
    set(handles.VTransSlide, 'Min', 0);
    set(handles.VTransSlide, 'Value', VtransInit);
    set(handles.VTransValue, 'String', strcat(num2str(VtransInit),' mm/s'));
    
else
    set(handles.backwardValue, 'Value', true);
        
    set(handles.distSlide, 'Max', 0);
    set(handles.distSlide, 'Min', -2000);
    set(handles.distSlide, 'Value', d);
    set(handles.distValue, 'String', strcat(num2str(d),' mm'));

    set(handles.VTransSlide, 'Max', 0);
    set(handles.VTransSlide, 'Min', -1500);
    set(handles.VTransSlide, 'Value', VtransInit);
    set(handles.VTransValue, 'String', strcat(num2str(VtransInit),' mm/s'));
end

set(handles.ticTimeValue, 'String', tic);

set(handles.coefAccValue, 'String', coef_acc_rot);

set(handles.VRotMaxSlide, 'Max', 2*pi);
set(handles.VRotMaxSlide, 'Min', 0);
set(handles.VRotMaxSlide, 'Value', VrotMax);
set(handles.VRotMaxValue, 'String', strcat(num2str(VrotMax, 3),' rad/s'));

set(handles.alphaSlide, 'Max', pi);
set(handles.alphaSlide, 'Min', -pi);
set(handles.alphaSlide, 'Value', alpha);
set(handles.alphaValue, 'String', strcat(num2str(alpha, 3),' rad'));

set(handles.VRotSlide, 'Max', 2*pi);
set(handles.VRotSlide, 'Min', -2*pi);
set(handles.VRotSlide, 'Value', VrotInit);
set(handles.VRotValue, 'String', strcat(num2str(VrotInit, 3),' rad/s'));


compute(handles);

handles.output = hObject;
guidata(hObject, handles);


function varargout = simuClothoide_OutputFcn(hObject, eventdata, handles)
varargout{1} = handles.output;

function VRotSlide_Callback(hObject, eventdata, handles)
global VrotInit;                        % rad/s
VrotInit = get(hObject, 'Value');
set(handles.VRotValue, 'String', strcat(num2str(VrotInit, 3),' rad/s'));
compute(handles);

function ticTimeValue_Callback(hObject, eventdata, handles)
global tic;                             % s
ticTemp = str2double(get(hObject, 'String'));
if(ticTemp > 0 && ticTemp <= 1)
    tic = ticTemp;
else
    set(hObject, 'String', tic);
end
compute(handles);

function VTransSlide_Callback(hObject, eventdata, handles)
global VtransInit;                      % mm/s
VtransInit = get(hObject, 'Value');
set(handles.VTransValue, 'String', strcat(num2str(VtransInit),' mm/s'));
compute(handles);

function alphaSlide_Callback(hObject, eventdata, handles)
global alpha;                           % rad
alpha = get(hObject, 'Value');
set(handles.alphaValue, 'String', strcat(num2str(alpha, 3),' rad'));
compute(handles);

function distSlide_Callback(hObject, eventdata, handles)
global d;                               % mm
d = get(hObject, 'Value');
set(handles.distValue, 'String', strcat(num2str(d),' mm'));
compute(handles);

function backwardValue_Callback(hObject, eventdata, handles)
global forward;                         % sens de marche
global d;                               % mm
global VtransInit;                      % mm/s

backward = get(hObject, 'Value');
if (backward)
    forward = false;
else
    forward = true;
end

d = -d;
VtransInit = -VtransInit;

if(forward)
    set(handles.backwardValue, 'Value', false);
    
    set(handles.distSlide, 'Max', 2000);
    set(handles.distSlide, 'Min', 0);
    set(handles.distSlide, 'Value', d);
    set(handles.distValue, 'String', strcat(num2str(d),' mm'));

    set(handles.VTransSlide, 'Max', 1500);
    set(handles.VTransSlide, 'Min', 0);
    set(handles.VTransSlide, 'Value', VtransInit);
    set(handles.VTransValue, 'String', strcat(num2str(VtransInit),' mm/s'));
    
else
    set(handles.backwardValue, 'Value', true);
        
    set(handles.distSlide, 'Max', 0);
    set(handles.distSlide, 'Min', -2000);
    set(handles.distSlide, 'Value', d);
    set(handles.distValue, 'String', strcat(num2str(d),' mm'));

    set(handles.VTransSlide, 'Max', 0);
    set(handles.VTransSlide, 'Min', -1500);
    set(handles.VTransSlide, 'Value', VtransInit);
    set(handles.VTransValue, 'String', strcat(num2str(VtransInit),' mm/s'));
end
compute(handles);

function VRotMaxSlide_Callback(hObject, eventdata, handles)
global VrotMax;                         % rad/s
VrotMax = get(hObject, 'Value');
set(handles.VRotMaxValue, 'String', strcat(num2str(VrotMax, 3),' rad/s'));
compute(handles);

function coefAccValue_Callback(hObject, eventdata, handles)
global coef_acc_rot;                    % rad/s²
coef_acc_rotTemp = str2double(get(hObject, 'String'));
if(coef_acc_rotTemp > 0 && coef_acc_rotTemp <= 30)
    coef_acc_rot = coef_acc_rotTemp;
else
    set(hObject, 'String', coef_acc_rot);
end
compute(handles);

function compute(handles)
global tic;                             % s
global VrotInit;                        % rad/s
global VtransInit;                      % mm/s
global alpha;                           % rad
global d;                               % mm
global forward;                         % sens de marche
global coef_acc_rot;                    % rad/s²
global VrotMax;                         % rad/s

%% Convertion des données
VrotInitx = VrotInit * tic;
VtransInitx = VtransInit * tic;
coef_acc_rotx = coef_acc_rot * tic * tic;
VrotMaxx = VrotMax * tic;

%% Variable de supervision
pointXGauche = zeros(1);
pointYGauche = zeros(1);
pointXDroite = zeros(1);
pointYDroite = zeros(1);

%% Calcul des données dans le nouveau référentiel

if(forward == false);
    alphax = alpha + pi;
    VtransInitx = -VtransInitx;
else
    alphax = alpha;
end

destX = d * cos(alphax);
destY = d * sin(alphax);

bgauche = false;
bdroite = false;

%% Calcul de la clothoïde de gauche
Vrot = VrotInitx;
Vtrans = VtransInitx;
alphax = 0;
px = 0;
py = 0;
loop = true;
nbPointGauche = 1;
while(loop)
    if(Vrot < VrotMaxx)
        Vrot = Vrot + coef_acc_rotx;
    end
    
    if(Vrot > VrotMaxx)
        Vrot = VrotMaxx;
    end
    
    alphax = alphax + Vrot;
    vx = Vtrans * cos(alphax);
    vy = Vtrans * sin(alphax);
    
    px = px + vx;
    py = py + vy;
    
    pointXGauche(nbPointGauche) = px;
    pointYGauche(nbPointGauche) = py;
    nbPointGauche = nbPointGauche + 1;
    
    if(alphax > pi/2)
        loop = false;
        bgauche = true;
    end
    
    if(px > destX)
        loop = false;
        if(destY < py)
            bgauche = true;
        else
            bgauche = false;
        end
    end
end

%% Calcul de la clothoïde de droite
Vrot = VrotInitx;
Vtrans = VtransInitx;
alphax = 0;
px = 0;
py = 0;
loop = true;
nbPointDroite = 1;
while(loop)
    if(Vrot > -VrotMaxx)
        Vrot = Vrot - coef_acc_rotx;
    end
    
    if(Vrot < -VrotMaxx)
        Vrot = -VrotMaxx;
    end
    
    alphax = alphax + Vrot;
    vx = Vtrans * cos(alphax);
    vy = Vtrans * sin(alphax);
    
    px = px + vx;
    py = py + vy;
    
    pointXDroite(nbPointDroite) = px;
    pointYDroite(nbPointDroite) = py;
    nbPointDroite = nbPointDroite + 1;
    
    if(alphax < -pi/2)
        loop = false;
        bdroite = true;
    end
    
    if(px > destX)
        loop = false;
        if(destY > py)
            bdroite = true;
        else
            bdroite = false;
        end
    end
end

if(bgauche)
    colorGauche = 'p green';
else
    colorGauche = 'p red';
end

if(bdroite)
    colorDroite = '+ green';
else
    colorDroite = '+ red';
end

%% Calcul de la trajectoire que devrait emprunter le robot

trajFoundG = false;
indexG = 1;
for i = 1:nbPointGauche-2
    if trajFoundG == false
        angleSucc = atan2(pointYGauche(i+1)-pointYGauche(i), pointXGauche(i+1)-pointXGauche(i));
        angleEnd = atan2(destY-pointYGauche(i), destX-pointXGauche(i));
        up = pointYGauche(i+1)-pointYGauche(i);
        if(abs(angleSucc - angleEnd) < 0.02 && (up > 0 || indexG > 100))
            trajFoundG = true;
        end
        indexG = indexG + 1;
    end
end

clear angleSucc angleEnd;
trajFoundD = false;
indexD = 1;
for i = 1:nbPointDroite-2
    if trajFoundD == false
        angleSucc = atan2(pointYDroite(i+1)-pointYDroite(i), pointXDroite(i+1)-pointXDroite(i));
        angleEnd = atan2(destY-pointYDroite(i), destX-pointXDroite(i));
        down = pointYDroite(i+1)-pointYDroite(i);
        if(abs(angleSucc - angleEnd) < 0.02 && (down < 0 || indexD > 100))
            trajFoundD = true;
        end
        indexD = indexD + 1;
    end
end

if(trajFoundD && trajFoundG)
    if(indexG >= indexD)
        trajX = pointXGauche(1:indexG);
        trajY = pointYGauche(1:indexG);
    else
        trajX = pointXDroite(1:indexD);
        trajY = pointYDroite(1:indexD);
    end
else
    if(trajFoundD)
        trajX = pointXDroite(1:indexD);
        trajY = pointYDroite(1:indexD);
    end
    
    if(trajFoundG)
        trajX = pointXGauche(1:indexG);
        trajY = pointYGauche(1:indexG);
    end
end

if(trajFoundD || trajFoundG)
    trajX(length(trajX)+1) = destX;
    trajY(length(trajY)+1) = destY;
end

if(trajFoundD == false && trajFoundG == false)
    trajX(1) = 0;
    trajY(1) = 0;
    trajX(2) = destX;
    trajY(2) = destY;
end
            

%% Affichage

axes(handles.axes1);
    
plot(pointXGauche, pointYGauche, colorGauche, ...
     pointXDroite, pointYDroite, colorDroite, ...
     trajX,        trajY,        '-',         ...
     destX,        destY,        'O');
axis equal;
ylabel('Y', 'FontSize', 10);
xlabel('X', 'FontSize', 10);
grid on;
set(gca,'FontSize',10);
