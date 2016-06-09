classdef t_openbci_logger < handle
%
% This class implements a logger for the openbci library. It logs
% the eeg data samples and dumps them to a file, using an ASCII format
% periodically.
%
% methods available:
%  - t_openbci_logger, constructor, filename required
%  - add_data, add a sample to the logger
%  - close_logger, dumps the last data and close the file
%
% Frederic Simard, Atom Embedded, 2015
%
    properties(Access=private)
        
        fid; % file id
        
        timestamps_buffer = []; % keeps the time stamps
        data_buffer = []; % keeps the data
        
        dump_to_file_counter = 0; % buffer index and count before dump to file
        
		
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		% ADDED (Modified by: Nicomaque)
		window_buffer = 125;
		window_buffer_values = [];
		
    end
    
    methods(Access=public)
        
        % constructor, opens a file and get ready to log the eeg samples of a trial
        function openbci_logger = t_openbci_logger(trial_id)
            openbci_constants;
			
            
            openbci_logger.dump_to_file_counter = 0;
            
            openbci_logger.timestamps_buffer = zeros(LOG_BUFFER_SIZE,1);
            openbci_logger.data_buffer = zeros(LOG_BUFFER_SIZE,NB_CHANNELS);
            
            openbci_logger.fid = fopen([LOG_FOLDER LOG_FILENAME_BASE num2str(trial_id) LOG_EXT],'w');
            
            if openbci_logger.fid == -1
               error('logger couldn''t open the file'); 
            end
			
			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			% ADDED (Modified by: Nicomaque)
			openbci_logger.window_buffer_values = zeros(openbci_logger.window_buffer,1);
			
        end
        
        % add a set of samples to the buffer and dump them to a file if required
        % timestamps are also inserted at the beginning of the line
        % but the tiem stamp is repeated for all samples in the set
        function add_data(openbci_logger, time_stamp, eeg_data, nb_packets)
		

            openbci_constants;
            
            % add sample to buffer
            for ii=1:nb_packets
                openbci_logger.dump_to_file_counter = openbci_logger.dump_to_file_counter+1;
                openbci_logger.data_buffer(openbci_logger.dump_to_file_counter,:) = eeg_data(ii,:);
                openbci_logger.timestamps_buffer(openbci_logger.dump_to_file_counter) = time_stamp;
            end
            
			
			assignin('base', 'data_buffer', openbci_logger.data_buffer)
			%assignin('base', 'dump_to_file_counter', openbci_logger.dump_to_file_counter)
			%assignin('base', 'LOG_BUFFER_SIZE', LOG_BUFFER_SIZE)
		
			
			
			 
            % if buffer is full, dump to file
            if (openbci_logger.dump_to_file_counter > LOG_BUFFER_SIZE)
				
                openbci_logger.dump_to_file();
                openbci_logger.dump_to_file_counter = 0;
				
				
				
				%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				% ADDED (Modified by: Nicomaque)

				window_buffer = [];
				
				for ii=1:130
					window_buffer(ii,:) = openbci_logger.data_buffer(ii,:);
				end
				
				assignin('base', 'window_buffer', window_buffer);
				
				
				
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% ADDED (Modified by: Nicomaque -- Tahar's code)
		
        %Cz   = data.data(:,7);  % Cz
		
		Cz   = window_buffer(:,7);
		Cz_aligned = [];
		SMR_vect_max = [];
		SMR_vect_mean = [];
		

 Fs                 = 250;                    
 N                  = length(Cz);
 wind_width         = 0.5;    % window width in seconds
 Sample_numb        = wind_width * Fs;  

 
         for i=1:Sample_numb:N 
           if (i >= Sample_numb)
               Cz_aligned(i-Sample_numb+1:i) = Cz(i-Sample_numb+1:i)-mean(Cz(i-Sample_numb+1:i));
			   assignin('base', 'passed_1', 1);
           end
         end
         
               
 %------------------------------------------
Hlp1 = fdesign.lowpass ('Fp,Fst,Ap,Ast',0.1,18,0.1,60,Fs);     
Hhp1 = fdesign.highpass('Fst,Fp,Ast,Ap',10,18,60,0.1,Fs); 
Hlp2 = fdesign.lowpass ('Fp,Fst,Ap,Ast',10,15,0.1,60,Fs);     
Hhp2 = fdesign.highpass('Fst,Fp,Ast,Ap',12,15,60,0.1,Fs); 
 %---------------------------------------
 flp1 = design(Hlp1); fhp1 = design(Hhp1);
 flp2 = design(Hlp2); fhp2 = design(Hhp2);
 %---------------------------------------

 buffer_1   = filter(flp1,Cz_aligned); 
 buffer_2   = filter(fhp1,buffer_1); 
 buffer_3   = filter(flp2,buffer_2); 
 SMR        = filter(fhp2,buffer_3);  

 %---------------------------------------
         for i=1:Sample_numb:N 
           if (i >= Sample_numb)
				assignin('base', 'passed_2', 2);
              SMR_vect_max (i-Sample_numb+1:i)= max(SMR(i-Sample_numb+1:i));
              SMR_vect_mean(i-Sample_numb+1:i)= mean(SMR(i-Sample_numb+1:i));
           end
         end
  %---------------------------------------
  
  % Affichage
%subplot(211)
%   plot(SMR_vect_max)
%   grid on
%   title   ('Max SMR voltage (Subject # 1)')
%subplot(212)
%   plot(SMR_vect_mean)
%   grid on
%   title   ('Mean SMR voltage (Subject # 1)')

%wvtool(SMR)
%wvtool(Cz)

	

%%%%%%% Send over UDP
				
window_SMR_vect_max = max(SMR_vect_max);
window_SMR_vect_mean = max(SMR_vect_mean);
				
assignin('base', 'window_SMR_vect_max',  window_SMR_vect_max);
assignin('base', 'window_SMR_vect_mean', window_SMR_vect_mean);			
				

u = udp('127.0.0.1',8051);

fopen(u)

fwrite(u, num2str(window_SMR_vect_max));

fclose(u)			
				
				
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% END of ADDED section
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
			



				
            end
            
        end
        
        % dump what's in the buffer and closethe file
        function close_logger(openbci_logger)
            openbci_logger.dump_to_file();
            fclose(openbci_logger.fid);
        end
            
    end
    
    methods(Access=private)
        
        % dump the buffer to the ASCII file
        % each line:
        % TIMESTAMP EEG1 EEG2 ... EEG8
        function dump_to_file(openbci_logger)
		
		
		
		% MAKE THE VARIABLE GO GLOBAL
		%assignin('base', 'data_buffer', openbci_logger.data_buffer)
		%assignin('base', 'dump_to_file_counter', openbci_logger.dump_to_file_counter)
		
		
		%assignin('base', 'LOG_BUFFER_SIZE', LOG_BUFFER_SIZE)
		%assignin('base', 'LOG_BUFFER_SIZE', LOG_BUFFER_SIZE)
		

			
            openbci_constants;
            
             for ii=1:openbci_logger.dump_to_file_counter
                 fprintf(openbci_logger.fid,'%f ',openbci_logger.timestamps_buffer(ii)); 
             for jj=1:NB_CHANNELS
                 fprintf(openbci_logger.fid,'%f ',openbci_logger.data_buffer(ii,jj)); 
             end
                 fprintf(openbci_logger.fid,'\n');
             end
            
            
            
            
        end
        
    end
end

