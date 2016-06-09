%	This script works as an example of how to use the OpenBCI Matlab script

%% start streaming in NTX_FIX_EXPERIMENT

% load NTX_FIX_EXPERIMENT
NTX_FIX_Experiment;

raw_data_storage;

time = 1:length(raw_data_storage);

eeg_val = [];

for i =1:length(raw_data_storage)
	eeg_val = [eeg_val, raw_data_storage(i,1)];
end

% FILTERING

% notch (60)
notch_Hz = [59.0 61.0]
[b,a]=butter(2,notch_Hz/125,'stop')
eeg_val = filter(b,a,eeg_val)

% bandpass (1,50)
bandpass_frequencies = [1 50.0]
[b,a] = butter(4,bandpass_frequencies/125,'bandpass');
eeg_val = filter(b,a,eeg_val);

plot(time,eeg_val)
axis([0 length(time) -200 200])