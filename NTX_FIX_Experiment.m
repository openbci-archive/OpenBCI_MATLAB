%
% This script tests a Matlab openBCI interface for trial based experiments
%
% It is meant to be used with OpenBCI 32bit board "chipkit". With this hardware,
% the dongle gets the data from the board and broadcast it on the serial 
% port: COM8, using the FTDI driver. 
%
% The test instantiate the openbci object and record 5 trials
%
% Frederic Simard, Atom Embedded, 2015
%

%
% Instantiate the openBCI interface
% this:
%    - open the port
%    - resets the device
%    - check the status packet
%

%load the t_openbci.m object, store in openbci
openbci = t_openbci;

%%
%
% Run a small experiment mockup



%fprintf(datestr(now,'mmmm dd, yyyy HH:MM:SS.FFF AM'))

% gives instructions
%clc;
%fprintf('ready for next trial?\n')
% wait for ok!
%pause;

% start the trial and record
%clc;
fprintf('Trial on-going\n')

duration = 20;


		% load constants
		openbci_constants;
		
		%empty the buffer
		openbci.interface.flush_buffer();
		
		% open logger and file
		trial_id = 1;
		log_file = fopen([LOG_FOLDER LOG_FILENAME_BASE num2str(trial_id) LOG_EXT],'w');
		
		%log all samples for duration
		tic; % initial tic
		
		first_record = false;
		
		%start streaming
		openbci.interface.start_streaming();
		
		raw_data_storage = [];

		current_entry = [];

		while(toc < duration)
			
			

			openbci_constants;

			if openbci.interface.com_port.BytesAvailable > 0
				packet_bytes = fread(openbci.interface.com_port,openbci.interface.com_port.BytesAvailable);
									
									
				for c_ii =1:length(packet_bytes)			
					if(packet_bytes(c_ii) == PACKET_FIRST_WORD) % decimal: 160
					
						if(length(current_entry) ~= 0)
							
							[ eeg_data ] = unpack_openbci_eeg(current_entry,1); % return zeros if invalid
							
							if(max(eeg_data) ~= 0 )
							
								% RECORD EEEG
								
										% DFOR EBUG:
										
										%fprintf('Entry:')
										%current_entry = current_entry
										eeg_data = eeg_data;

										row = [];
										for col =1:length(eeg_data)
											row = [row, eeg_data(col)];
										end
										raw_data_storage = [raw_data_storage ; row];
										%pause;
										
								if(first_record == false)
									tic; % reset tic (timestamp 0 for first entry)
									
									%%% INIT COMMUNICATION HERE IF MATLAB IS USED AS A BACK-END FOR A PROGRAM %%%
								
									%u = udp('127.0.0.1',8051);

									%fopen(u)
									%fwrite(u, 'Here are the values...')
									%fclose(u)
	
									first_record = true;
								end



								fprintf(log_file,'%f ',toc); 
								for jj=1:NB_CHANNELS
									fprintf(log_file,'%f ',eeg_data(1,jj)); 
								end
								fprintf(log_file,'\n');

		
				
								
							end
							
							current_entry = [];
						end
					end

					current_entry(end+1) = packet_bytes(c_ii);

				end
				

				
			end
				

		end
		
		%stop streaming
		openbci.interface.stop_streaming();
		
		%empty the buffer
		openbci.interface.flush_buffer();
		
		%close datafile
		fclose(log_file);



%fprintf(datestr(now,'mmmm dd, yyyy HH:MM:SS.FFF AM'))



