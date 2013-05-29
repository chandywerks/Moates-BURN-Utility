Linux drivers for the Moates BURN I and II EEPROM burner.

Arguments
	-d FILE
		Specify a device file, defaults to /dev/ttyUSB0

	-c WORD
		Specify the chip type, supported ships are...
			- SST27SF512
			- AT29C256
			- AM29F040
			- 2732A 	(Read-Only)

	-r FILE
		Specify a file to dump EEPROM data to

	-w FILE
		Specify a file to write to the EEPROM

	-e
		Erase contents of the EEPROM
