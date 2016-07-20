OpenBCI_Matlab
==============

This repository contains the files needed for obtaining an [OpenBCI](http://openbci.com/) data stream directly into Matlab.

Currently, there is one method for streaming data into Matlab:

1. Labstreaminglayer (Python to Matlab)

More methods currently under development!

**If you are running into any problems, check the "Troubleshooting" section of this guide first! If it is not covered in that section, please pull up an issue on this repo.**

## Lab Streaming Layer (Python to Matlab)
[Lab streaming layer \(LSL\)](https://github.com/sccn/labstreaminglayer) is a networking tool that allows for real time streaming, recording, and analysis of biodata. The `openbci_pylsl` program uses Python to establish an LSL stream that can be received using scripts in Matlab. Follow the steps below to learn how to setup and begin using Matlab for real-time data analysis.

#### Steps
* [Setup](#setup)
  * [Download Repo](#1-download-the-openbci_matlab-repo-from-github)
  * [Python Setup](#2-python-setup)
  * [Matlab Setup](#3-matlab-setup)
* [Usage](#usage)
  * [Start Stream](#start-a-stream-in-python)
  * [Receive Stream](#receive-the-stream-in-matlab)
  * [Monitor Stream](#monitoring-your-stream)


### Setup

#### 1. **Download the OpenBCI_Matlab repo from Github**

**Two methods:** 

  1. **Method One**: Type `git clone https://github.com/gabrielibagon/OpenBCI_MATLAB.git` on the command line

  2. **Method Two**: Click "Clone or download" near the top right of the page, and click "Download Zip". Unzip the file into a directory of your choice

#### 2. **Python Setup**
**Install Python and pip:**

  1. [Download Python](https://www.python.org/downloads/) (either version 2.7 or 3.5) onto your computer.
  2. Check if `pip` was also installed:
    - On the command line, enter: `pip list`. If you get a list of modules as output, pip is installed!
    - If not installed, follow these instructions: [Pip Installation](https://pip.pypa.io/en/stable/installing/)
			
        > Note: If you are having trouble installing pip, you can skip this step - just make sure to download the Python libraries manually in the next step.

**Install libraries**
  1. If you have pip installed, navigate to the "Matlab-Python" directory and enter on your command line:
    `pip install -r requirements.txt`

  2. If you do not have pip installed, manually install the two libraries:
    1. [pyserial](https://pypi.python.org/pypi/pyserial)
    2. [pylsl](https://pypi.python.org/pypi/pylsl/1.10.3)

#### 3. **Matlab Setup**

*The next few steps will assume you have a recent version of Matlab already installed and running on your computer*

1. **Add the "labstreaminglayer" directory to your Matlab path**:

  **Two methods**:

    1. **Method One:** On the Matlab command line, type the following:

      `>> addpath(genpath('/path/OpenBCI_MATLAB/Matlab-Python/labstreaminglayer'))` 
      
      (Replace "path" with the path to where you downloaded this repository)

    2. **Method Two:** Go to **Environment** on your Toolstrip and click **Set Path**. Click **Add with Subfolders** and select the folder *labstreaminglayer* from the Github download.

To test if LSL is set up correctly in Matlab, enter the following on your Matlab command line:

`>> lsl_loadlib()`

If you do not get a red error message, LSL is installed correctly!

### Usage

Getting a stream into Matlab can be done in two steps:

  1) Start a stream with the `openbci_pylsl.py` program
  2) Receive the stream with an lsl script in Matlab

####**Start a stream in Python**
  1. Plug in your dongle and power up your OpenBCI Board
  2. Navigate to the "Matlab-Python" directory on your command line and enter:
    ` python openbci_pylsl.py -p "PORT" `

    The PORT should be replaced with the serial port that your OpenBCI dongle is plugged into, i.e. "COM1" Windows or "/dev/ttyUSB0" on Linux.

    If you are using the 16 channel board, add the "-d" (daisy) option:

    ` python openbci_pylsl.py -p "PORT" -d `  

      >Note:

      > If you don't know the serial port in use, try the following:

      >Windows:
      >	1. Go to "Devices and Printers" on your PC
      >	2. Right click, and select "Device Administration"
      > 3. Select ports, and find the "USB Serial Port"

      >	Mac / Linux
      > 1. In terminal, type:
      > `dmesg | grep ttyUSB*`
      >2. Find the FTDI USB Serial Device convertor (usually located near the end of the list)

	  For example, here is the command for my computer:

        python openbci_pylsl.py -p "/dev/ttyUSB0"

  3. After Board initialization, you should see a "BEGIN" and a new prompt on the command line. You are now ready to start streaming!

    To begin streaming, type `/start`

    To stop streaming, type `/stop`

    To disconnect from the serial port, type `/exit`

The data stream from your board should now be pushed to the lab streaming layer. The next step is to receive this data Matlab.

####**Receive the stream in Matlab**
##### Matlab Scripts

  Regardless of your Matlab workflow, the basic syntax of receiving the stream is simple:

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

You should experiment with different methods of working with the output of `inlet.pull_sample()`. You can adjust the while loop, and use your script to call a variety of different scripts while you collect data. You can also experiment with parallel computing in Matlab to allow the labstreaminglayer to run in the background while running other scripts.


##### Toolboxes
There are Matlab toolboxes that have built-in methods for working with LSL streams. Here are some examples:

  1. **BCILAB**


####**Monitoring your stream**

  You might want to monitor your stream in Matlab before you deploy any scripts in order to make sure that the biodata is correctly streaming into Matlab.

  You can use the `vis_stream` function to bring up the Matlab Visualizer toolbox for LSL.

  Once you have a stream deployed in Python, on your Matlab Command Line, type:

	`vis_stream`

  A window should appear asking for information about the stream. Try the following into the window:

  ![vis_stream](https://github.com/gabrielibagon/OpenBCI_MATLAB/raw/master/images/vis_stream.png)

  You may need to adjust some parameters. For instance, if you are streaming 16 channels from the OpenBCI Board, you will want to type "[1:17]" in the "Channels to Display" field.

  Press "Ok" and you should be seeing the voltages of your signals displayed over time. Check to see if any of the channels are railed (showing no signal). 



### Examples

#### Collecting data while running experiments
	
  TODO



## Troubleshooting

Here are some frequently encountered errors and their solutions:

**In Python**

1. **"WARNING:root:Skipped x bytes before start found"**

	This is a known issue with the Python serial port parser. This should not cause any major issues with your data, and it can be ignored.

2. **"UnicodeDecodeError: 'utf-8' codec can't decode byte 0xc0 in position 0: invalid start byte"**

	This is another known issue with the serial port parser. If you get this error, simply unplug the dongle and power down the board and try again.

**In Matlab**

1. **"supergui error: argument 'fig' must be numeric" in Matlab**

  To fix this, change the offending line (something around 122 to 126) from:
    `'fig' 'real' [] 0;`
  to:
    `'fig'       ''   []      0;`

2. **Error using lsl_loadlib (line 62) Apparently the file "/path/BCILAB-1.1/dependencies/liblsl-Matlab/bin/liblsl64.so" is missing on your computer. Cannot load the lab streaming layer.**

  To solve this, copy the contents of the OpenBCI_MATLAB/Matlab-Python/labstreaminglayer/liblsl-Matlab/bin to the path where it is looking for liblsl64 binary (in the example above, this path is '/path/BCILAB-1.1/dependencies/liblsl-Matlab/bin/')
