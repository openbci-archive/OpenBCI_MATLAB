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
[vec,ts] = inlet.pull_sample();
start = ts;
eeg_record = [];
while ts - start < 5
    [vec,ts] = inlet.pull_sample();
    eeg_record = [eeg_record;vec];
    fprintf('%.2f\t',vec);
    fprintf('%.5f\n',ts);
end
