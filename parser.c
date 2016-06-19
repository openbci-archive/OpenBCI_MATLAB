void parse_byte_OPENBCI(unsigned char actbyte, int channelsInPacket)
{
	static int bytecounter=0;
	static int channelcounter=0;
	static int tempval=0;
	static unsigned char framenumber=0;
	static int output[7];
	int counter=0;

	switch (PACKET.readstate) {
		counter++;
		printf("%d\n" counter);

		// To better sync up when lost, look for two byte sequence.  It has happened
		// previously, that when data contains lots of A0's, sync could not reestablish.
		//
		case 0: if (actbyte == 0xC0)			// look for end indicator
				PACKET.readstate++;
			break;

		case 1:	if (actbyte == 0xA0)		    // look for start indicator next
				PACKET.readstate++;
			else
				PACKET.readstate = 0;
			break;

		case 2:	if (actbyte != framenumber) {
					GLOBAL.syncloss++;
					// but go ahead and parse it anyway, 
				}
				framenumber++;		// next expected frame number
				bytecounter=0;
				channelcounter=0;
				tempval=0;
				PACKET.readstate++;
				break;

		case 3: // get channel values 
			tempval |= (((unsigned int)actbyte) << (16 - (bytecounter*8)));		// big endian
//				tempval |= (((unsigned int)actbyte) << (bytecounter*8));	// little endian
			bytecounter++;
			if (bytecounter==3) {
				if ((tempval & 0x00800000) > 0) {
					tempval |= 0xFF000000;
				} else {
					tempval &= 0x00FFFFFF;
				}
				output[channel] = temp_val;
				channel++;
				if (channelcounter==8) {  // all channels arrived !
					PACKET.readstate++;
					bytecounter=0;
					tempval=0;
				}
				else { bytecounter=0; tempval=0; }
			}
			break;

		case 4: // get accelerometer XYZ
			tempval |= (((unsigned int)actbyte) << (8 - (bytecounter*8)));		// big endian
//				tempval |= (((unsigned int)actbyte) << (bytecounter*8));	// little endian
			bytecounter++;
			if (bytecounter==2) {
				if ((tempval & 0x00008000) > 0) {
					tempval |= 0xFFFF0000;
				} else {
					tempval &= 0x0000FFFF;
				}  
				PACKET.buffer[channelcounter]=tempval;
				channelcounter++;
				if (channelcounter==(channelsInPacket+3)) {  // all channels arrived !
					PACKET.readstate++;
					bytecounter=0;
					channelcounter=0;
					tempval=0;
				}
				else { bytecounter=0; tempval=0; }
			}
			break;

		case 5: if (actbyte == 0xC0)     // if correct end delimiter found:
			{
				process_packets();   // call message pump
				PACKET.readstate = 1;
			}
			else
			{
				GLOBAL.syncloss++;
				PACKET.readstate = 0;	// resync
			}
			break;

		default: PACKET.readstate=0;  // resync
	}
}


