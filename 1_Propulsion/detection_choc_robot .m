function varargout = detection_choc_robot(varargin)
% DETECTION_CHOC_ROBOT MATLAB code for detection_choc_robot.fig
%      DETECTION_CHOC_ROBOT, by itself, creates a new DETECTION_CHOC_ROBOT or raises the existing
%      singleton*.
%
%      H = DETECTION_CHOC_ROBOT returns the handle to a new DETECTION_CHOC_ROBOT or the handle to
%      the existing singleton*.
%
%      DETECTION_CHOC_ROBOT('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in DETECTION_CHOC_ROBOT.M with the given input arguments.
%
%      DETECTION_CHOC_ROBOT('Property','Value',...) creates a new DETECTION_CHOC_ROBOT or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before detection_choc_robot_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to detection_choc_robot_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help detection_choc_robot

% Last Modified by GUIDE v2.5 09-Dec-2015 15:05:53

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @detection_choc_robot_OpeningFcn, ...
                   'gui_OutputFcn',  @detection_choc_robot_OutputFcn, ...
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
% End initialization code - DO NOT EDIT


% --- Executes just before detection_choc_robot is made visible.
function detection_choc_robot_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to detection_choc_robot (see VARARGIN)

% Choose default command line output for detection_choc_robot
handles.output = hObject;

handles.nb_val_view = 4000;
handles.s = serial('COM14'); %assigns the object s to serial port
set(handles.s, 'InputBufferSize', 1024); %number of bytes in input buffer
set(handles.s, 'FlowControl', 'hardware')
set(handles.s, 'BaudRate', 230400)
set(handles.s, 'Parity', 'none')
set(handles.s, 'DataBits', 8)
set(handles.s, 'StopBit', 1)
set(handles.s, 'Timeout',10)
handles.bp_stop = 0;
handles.bp_close = 0;

fopen(handles.s); 
disp('Port open')

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes detection_choc_robot wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = detection_choc_robot_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
acc_translation = zeros(handles.nb_val_view,1);
odo_translation = zeros(handles.nb_val_view,1);
acc_rotation = zeros(handles.nb_val_view,1);
odo_rotation = zeros(handles.nb_val_view,1);
time = (1:handles.nb_val_view)';
line = zeros(1, 4);
echant = 1;
first_passage = 1;

while(handles.bp_close == 0)
    if(handles.bp_stop == 0)
        while fread(s, 1, 'char') ~= 'd' end
        %disp('Start')
        line = fscanf(s, '%d %d %d %d\r', 14);
        if(size(line)==[4, 1])
           acc_translation(echant,1) = line(1,1);
           odo_translation(echant,1) = line(2,1);
           acc_rotation(echant,1) = line(3,1);
           odo_rotation(echant,1) = line(4,1);
        end
        
        %Translation
        if(first_passage)
            plot(handles.FIG_translation, time, acc_translation, 'b-', time, odo_translation, 'r-');
            max_acc_trans = max(acc_translation(1:echant));
            max_odo_trans = max(odo_translation(1:echant));
            min_acc_trans = min(acc_translation(1:echant));
            min_odo_trans = min(odo_translation(1:echant));
            max_trans = max([max_acc_trans max_odo_trans]) + 20;
            min_trans = min([min_acc_trans min_odo_trans]) - 20;
            axis(handles.FIG_translation, [1 max([echant 2]) min_trans max_trans]);
        else
            if(echant == handles.nb_val_view)
                plot(time, acc_translation(1:echant), 'b-', time, odo_translation(1:echant), 'r-', 'parent', handles.FIG_translation);
            else
                plot(time, [acc_translation(echant+1:handles.nb_val_view);acc_translation(1:echant)], 'b-', time, [odo_translation(echant+1:handles.nb_val_view);odo_translation(1:echant)], 'r-', 'parent', handles.FIG_translation);
            end
            max_acc_trans = max(acc_translation(:));
            max_odo_trans = max(odo_translation(:));
            min_acc_trans = min(acc_translation(:));
            min_odo_trans = min(odo_translation(:));
            max_trans = max([max_acc_trans max_odo_trans]) + 20;
            min_trans = min([min_acc_trans min_odo_trans]) - 20;
            axis(handles.FIG_translation,[1 handles.nb_val_view min_trans max_trans])
        end
        title(handles.FIG_translation,'Acceleration en translation');
        ylabel(handles.FIG_translation,{'Accéléromètre (bleu)';'Odométrie(rouge)'});
        grid(handles.FIG_translation, 'on')
        grid(handles.FIG_translation, 'minor')

        %Rotation
        if(first_passage)
            plot(handles.FIG_rotation, time, acc_rotation, 'b-', time, odo_rotation, 'r-');
            max_acc_rot = max(acc_rotation(1:echant));
            max_odo_rot = max(odo_rotation(1:echant));
            min_acc_rot = min(acc_rotation(1:echant));
            min_odo_rot = min(odo_rotation(1:echant));
            max_rot = max([max_acc_rot max_odo_rot]) + 20;
            min_rot = min([min_acc_rot min_odo_rot]) - 20;
            axis(handles.FIG_rotation,[1 max([echant 2]) min_rot max_rot]);
        else
            if(echant == handles.nb_val_view)
                plot(time, acc_rotation(1:echant), 'b-', time, odo_rotation(1:echant), 'r-', 'parent', handles.FIG_rotation);
            else
                plot(time, [acc_rotation(echant+1:handles.nb_val_view);acc_rotation(1:echant)], 'b-', time, [odo_rotation(echant+1:handles.nb_val_view);odo_rotation(1:echant)], 'r-', 'parent', handles.FIG_rotation);
            end
            max_acc_rot = max(acc_rotation(:));
            max_odo_rot = max(odo_rotation(:));
            min_acc_rot = min(acc_rotation(:));
            min_odo_rot = min(odo_rotation(:));
            max_rot = max([max_acc_rot max_odo_rot]) + 20;
            min_rot = min([min_acc_rot min_odo_rot]) - 20;
            axis(handles.FIG_rotation,[1 handles.nb_val_view min_rot max_rot])
        end
        title(handles.FIG_rotation,'Acceleration en rotation');
        ylabel(handles.FIG_rotation,{'Accéléromètre (bleu)';'Odométrie(rouge)'});
        grid(handles.FIG_rotation, 'on')
        grid(handles.FIG_rotation, 'minor')
        drawnow

        if echant == handles.nb_val_view
            echant = 1;
            first_passage = 0;
        else
            echant = echant + 1;
        end
    else
        disp('En pause')
    end
end

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in PB_pause.
function PB_pause_Callback(hObject, eventdata, handles)
% hObject    handle to PB_pause (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(handles.bp_stop == 0)
    handles.bp_stop = 1;
    set(hObject, 'BackgroundColor', [0 1 0]);
    set(hObject, 'String', 'Continue');
    guidata(hObject, handles)
    disp('stop on')
    uiwait(gcf)
else
    handles.bp_stop = 0;
    set(hObject, 'BackgroundColor', [1 0 0]);
    set(hObject, 'String', 'Pause');
    guidata(hObject, handles)
    disp('stop off')
    uiresume(gcf)
end

