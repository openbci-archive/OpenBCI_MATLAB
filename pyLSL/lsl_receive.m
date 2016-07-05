% Simple LSL script in MATLAB

lib = lsl_loadlib(); %instantiate the library

disp('Resolving an EEG stream...');
result = {};

while isempty(result);
	result = lsl_resolve_byprop(lib,'type','EEG'); end

%createe a new inlet
disp('Opening an inlet...');
inlet = lsl_inlet(result{1});

disp('Now receiving data...');
while true
	[vec, ts] = inlet.pull_sample();

	fprintf('%.2f\t',vec);
	fprintf('%.5f\n', ts);
end