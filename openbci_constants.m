
% COM port configuration
OPENBCI_PORT = '/dev/ttyUSB0';
OPENBCI_BAUDRATE = 115200;
OPENBCI_PORT_STATUS_OPEN = 'open';

% COM Messages for openbci
START_STREAMING_MSG = char('b');
STOP_STREAMING_MSG = char('s');
RESET_MSG = char('v');

% packets length
%STATUS_PACKET_LENGTH = 118;
STATUS_PACKET_LENGTH = 84;
DATA_PACKET_LENGTH = 33;

NB_CHANNELS = 8;
PACKET_FIRST_WORD = 160; %0xA0
PACKET_LAST_WORD = 192;	%0xC0		

% sampling rate of the openbci 250Hz
SAMPLING_RATE = 250;
SAMPLING_PERIOD = 1/SAMPLING_RATE;

% ADS1299 Constants
ADS1299_Vref = 4.5		%reference voltage for ADS1299. set by the hardware
ADS1299_gain = 24.0		%assumed gain setting for ADS1299. set by its Arduino code.
scale_fac_uVolts_per_count = ADS1299_Vref/(2^(23)-1)/ADS1299_gain*1000000.
scale_fac_accel_G_per_count = 0.002 /(2^4)	%assume set to +/4G, so 2 mG 
lead_off_drive_amps = 6.0e-9		%6nA, set by the arduino code

% timeline is allocated in blocks of 120 seconds
TIMELINE_TIME_BLOCK = 120;
TIMELINE_SAMPLES_BLOCK = TIMELINE_TIME_BLOCK/SAMPLING_PERIOD;

% display history of last 1 seconds
TIMELINE_LENGTH_TIME = 5;
% display history of last X samples
TIMELINE_LENGTH_SAMPLES = TIMELINE_LENGTH_TIME/SAMPLING_PERIOD;

% traces color
COLOR = 'rgbkrgbk';

% curves static placement on the graph
CURVE_OFFSETS = [0 1 2 3 4 5 6 7];

DISPLAY_PERIOD_SAMPLES = 50;

DISPLAY_Y_MAX = 8;

LOG_BUFFER_SIZE = 130;

% logger, folders and filename
LOG_FOLDER = 'log/';
LOG_FILENAME_BASE = 'trial_';
LOG_EXT = '.bci_dat';