# download LSL and pylsl from https://code.google.com/p/labstreaminglayer/
# Eg: ftp://sccn.ucsd.edu/pub/software/LSL/SDK/liblsl-Python-1.10.2.zip
# put in "lib" folder (same level as user.py)
import sys; sys.path.append('lib') # help python find pylsl relative to this example program
from pylsl import StreamInfo, StreamOutlet
import open_bci_v3
import time

# Use LSL protocol to broadcast data using one stream for EEG and one stream for AUX
class StreamerLSL():
	# From IPlugin
	def __init__(self):
		print("woooot")
		# make outlets

	def create_lsl(self):
		self.eeg_channels = 8
		self.sample_rate = 250
		eeg_stream = "OpenBCI_EEG"
		eeg_id = "openbci_eeg_id1"
		aux_stream = "OpenBCI_AUX"
		aux_id = "openbci_aux_id1"
		info_eeg = StreamInfo(eeg_stream, 'EEG', self.eeg_channels,self.sample_rate,'float32',eeg_id);
		# info_aux = StreamInfo(aux_stream, 'AUX', self.aux_channels,self.sample_rate,'float32',aux_id)
		self.outlet_eeg = StreamOutlet(info_eeg)
		# self.outlet_aux = StreamOutlet(info_aux)


	def send(self,sample):
		# print(sample.channel_data)
		self.outlet_eeg.push_sample(sample.channel_data)
		# self.outlet_aux.push_sample(sample.aux_data)


def main():
	lsl = StreamerLSL()
	lsl.create_lsl()
	board = open_bci_v3.OpenBCIBoard(port='/dev/ttyUSB0')
	board.ser.write('v')
	time.sleep(2)
	board.start_streaming(lsl.send)

if __name__ == '__main__':
	main()