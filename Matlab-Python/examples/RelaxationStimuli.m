addpath(genpath('/home/ibagon/OpenBCI/OpenBCI_MATLAB/Matlab-Python/labstreaminglayer'))

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
start = ts;
eeg_record = [];
KbStrokeWait;
% Clear the screen
sca;

while (i<1000)
    i=i+1;
    disp(i);
end
DrawFormattedText(window, 'Alpha', 'center',screenYpixels*.75, white);
while (j<1000)
    j=j+1;
    
end


KbStrokeWait;
% Clear the screen
sca;
