% Basic EEG pre-processing
%
% USAGE:
% >> output_matrix = eeg_filter(eeg_matrix,lowcut, highcut)
% 
% To output filtered data to a file, use the following commands:
% >> fid = fopen('name_of_output.txt', 'wt');
% >> fprinft(fid, '%16.16f\n', 'wt');
% >> fclose(fid);


%function call
function [filtered_file] = eeg_filter(eeg_data, f_low, f_high)
% declare constants
fs = 250;               %sampling rate
fn = fs/2;              %Nyquist frequency
filter_order = 2;       %this is what Chip uses

%% Filter design
% Notch window
wn = [59 61]            % Cutoff frequencies
% 2nd order Butterworth filters
[b,a]=butter(filter_order,f_high/(fs/2),'low');      % Low pass filter coefficients
[b1,a1]=butter(filter_order,f_low/(fs/2),'high');    % High pass filter coefficients
[bn,an] = butter(filter_order,wn/(fs/2),'stop');     % Notch filter coefficients
filtered_file = []
for i = 1:length(eeg_data(1,:))
    channel = eeg_data(:,i);            % isolate the channel
    y = filtfilt(bn,an,channel);        % notch filter
    y = filtfilt(b1,a1,y);              % high pass filter
    y = filtfilt(b,a,y);                % low pass filter
    filtered_file = [filtered_file y];
end