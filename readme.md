OpenBCI_Matlab
==============

This repository contains the files needed for obtaining an [OpenBCI](http://openbci.com/) data stream directly into Matlab.

Currently, there is one method for streaming data into Matlab:

1. Labstreaminglayer (Python to Matlab)

**If you are running into any problems, check the "Troubleshooting" section of this Guide first! If it is not covered in that section, please pull up an issue on this repo**

## Lab Streaming Layer (Python to Matlab)
[Lab streaming layer \(LSL\)](https://github.com/sccn/labstreaminglayer) is a networking tool that allows for real time streaming, recording, and analysis of biodata. The `openbci_pylsl` program uses Python to establish an LSL stream that can be received using scripts in Matlab. Follow the steps below to learn how to setup and begin using Matlab for real-time data analysis.

### Setup

1. **Download the OpenBCI_Matlab repo from Github**

	**Two methods:** 

	  + **Method One**: Type `git clone https://github.com/gabrielibagon/OpenBCI_MATLAB.git` on the command line

	  + **Method Two**: Click "Clone or download" at the top of the page, and click "Download Zip". Unzip the file into a directory of your choice

2. **Python Setup**
    - **If you do not have Python and `pip` installed on your computer:**
	  	1. [Download Python](https://www.python.org/downloads/) (either version 2.7 or 3.5) onto your computer.
	  	2. Check if `pip` was also installed:
	  		- On the command line, enter: `pip list`
	  		- If not installed, follow these instructions: [Pip Installation](https://pip.pypa.io/en/stable/installing/)
	> Note: If you are having trouble installing pip, you can skip this step - just make sure to download the Python libraries manually in the next step.

    - **Install libraries**
        1. If you have pip installed, navigate to the "Matlab-Python" directory and enter on your command line:
        `pip install -r requirements.txt`

        2. If you do not have pip installed, manually install the two libraries:
        	1. [pyserial](https://pypi.python.org/pypi/pyserial)
        	2. [pylsl](https://pypi.python.org/pypi/pylsl/1.10.3)

2. **Matlab Setup**
*The next few steps will assume you have a recent version of Matlab already installed and running on your computer*
    1. Add the "labstreamiglayer" directory to your Matlab path.

	  **Two methods**:
            1. On the Matlab command line, type the following: 
.		
		      `>> addpath(genpath('/path/OpenBCI_MATLAB/Matlab-Python/labstreaminglayer'))` 
.
	          (Replace "path" with the path to where you downloaded this repository)
.
            2. Go to "Environment" on your Toolstrip and click "Set Path". Click "Add with Subfolders" and select the folder "labstreaminglayer" from the Github download.

To test if LSL is setup in Matlab enter the following on your Matlab command line:

`>> lsl_loadlib()`

If you do not get an error message, LSL is installed correctly!

### Usage

Getting a stream into Matlab can be done in two steps:
    1) Start a stream with the `openbci_pylsl.py` program
    2) Receive the stream with an lsl script in Matlab

- **Start a stream in Python**
	1. Plug in your dongle and power up your OpenBCI Board
	2. Navigate to the "Python-Matlab" directory on your command line and enter:
            ` python openbci_pylsl.py -p "PORT" `

		The PORT should be replaced with the serial port that your OpenBCI dongle is plugged into, i.e. "COM1" Windows or "/dev/ttyUSB0" on Linux.

		>If you don't know your serial port, try the following:

		>Windows:
		>	1. Go to "Devices and Printers" on your PC
		>	2. Right click, and select "Device Administration"
			3. Select ports, and find the "USB Serial Port"

		>	Mac / Linux
		>		1. In terminal, type:
		>			    `dmesg | grep ttyUSB*`
        >		2. Find the FTDI USB Serial Device convertor (usually located near the end of the list)

		For example, here is the command for my computer:

			python openbci_pylsl.py -p "/dev/ttyUSB0"

	3. 	After Board initialization, you should see a "BEGIN" and a new prompt on the command line. You are now ready to start streaming!

		To begin streaming, type `/start`

		To stop streaming, type `/stop`

		To disconnect from the serial port, type `/exit`

	The data stream from your board should now be pushed to the lab streaming layer. The next step is to receive this data Matlab.

- **Receive the stream in Matlab**
Regardless of your Matlab workflow, the basic syntax of receiving the stream are simple:

	From ReceiveData.m in *examples*:
 		```
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

		disp('Now receiving data...');
		while true
		    % get data from the inlet
		    [vec,ts] = inlet.pull_sample();
		    % and display it
		    fprintf('%.2f\t',vec);
		    fprintf('%.5f\n',ts);
		end    	
		```
You should experiment with different methods of working with the output of `inlet.pull_sample()`. You can add to that while loop to call a variety of different scripts while you collect data. You can also experiment with parallel computing in Matlab to allow the labstreaminglayer to run in the background while running other scripts.

