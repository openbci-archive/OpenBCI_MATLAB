addpath(genpath('/home/ibagon/OpenBCI/OpenBCI_MATLAB/Matlab-Python/labstreaminglayer'))

%% instantiate the library
disp('Loading the library...');
lib = lsl_loadlib();

% resolve a stream...
disp('Resolving an EEG stream...');
result = {};
while isempty(result)
    result = lsl_resolve_byprop(lib,'type','EEG'); end

% create a new inlet
disp('Opening an inlet...');
inlet = lsl_inlet(result{1});
PsychDefaultSetup(2);

screenNumber = 0;

black = BlackIndex(screenNumber);
white = WhiteIndex(screenNumber);
grey = white / 2;

% Open an on screen window and color it grey
[window, windowRect] = PsychImaging('OpenWindow', screenNumber, black);

% Set the blend funciton for the screen
Screen('BlendFunction', window, 'GL_SRC_ALPHA', 'GL_ONE_MINUS_SRC_ALPHA');

% Get the size of the on screen window in pixels
% For help see: Screen WindowSize?
[screenXpixels, screenYpixels] = Screen('WindowSize', window);

% Get the centre coordinate of the window in pixels
% For help see: help RectCenter
[xCenter, yCenter] = RectCenter(windowRect);
Screen('TextSize', window, 80);
Screen('TextFont', window, 'Courier');
DrawFormattedText(window, 'Rest', 'center',screenYpixels*.75, white);
Screen('Flip', window);
We set the text size to be nice and big here

[vec,ts] = inlet.pull_sample();
start = ts;
eeg_record = [];
while ts - start < 10
    [vec,ts] = inlet.pull_sample();
    eeg_record = [eeg_record;vec];
    if ((ts-start) > 2.5) && ((ts-start) <7.5)
        DrawFormattedText(window, 'Meditation', 'center', screenYpixels*.75, white);
        Screen('Flip', window);
          disp('IN')
    end
    if ((ts - start) > 7.5)
        DrawFormattedText(window, 'Rest', 'center', screenYpixels*.75, white);
        Screen('Flip', window);
    end
    disp('out')
    fprintf('%.5f\n',ts);
end
% KbStrokeWait;
% Clear the screen
sca;
