# Libelium-LoRa

The SX1272 LoRa module can work in several embedded system as Waspmote, Plug & Sense, Arduino, Raspeberry Pi or Intel Galileo. 
The normal topology for this technology is a star: all the nodes transmit towards a central sink node. This central sink node can be:

- another node;
- a LoRa Gateway.

This project want give some tools for using a LoRa module on a Linux system for testing the reception and sending of LoRa ASCII messages.
To perform these actions you need a LoRa node and a LoRa Gateway, the device that allows you to communicate through a USB port with the module.

You also need a terminal program like Cutecom (Linux), Hyperterminal or Putty (Windows). 

For more information about LoRa module and gateway go to Libelium site:
[http://www.libelium.com/development/waspmote/documentation/lora-gateway-tutorial/]

## Comunications setup

The communication with the Gateway has to have this setup:

* Baudrate: 38400
* 8 Data bits
* No parity
* 1 Stop bit

If you want know the device of the LoRa gateway you can use *dmesg* command when plug the LoRa gateway:

```
$ dmesg | grep FTDI | tail -n 3
[282366.601552] usb 1-1.5: Manufacturer: FTDI
[282366.604111] ftdi_sio 1-1.5:1.0: FTDI USB Serial Device converter detected
[282366.604570] usb 1-1.5: FTDI USB Serial Device converter now attached to ttyUSB0
```

## Compile and install

To compile use the command:

```
$ make clean && make release NAME=lora_config
$ make clean && make release NAME=lora_setup
$ make clean && make release NAME=lora_sender
$ make clean && make release NAME=lora_daemon
```

If there are no errors will be generated the following files:

* lora_config
* lora_setup
* lora_sender

Copy binary files in /usr/bin or /usr/local/bin with the root privileges:

```
$ cp lora_config lora_setup lora_sender /usr/local/bin
```

## lora_config

This command reads the configuration of the LoRa module connected to the LoRa Gateway.
Syntax is:

```
Usage: lora_config [-v 0|1|2]  [-s serial_device] [-b serial_bitrate]
       lora_config -h

 -b : serial bitrate [1200|2400|4800|9600|19200|38400|57600|115200]. Default value is 38400.
 -d : serial device. Default value is /dev/ttyUSB0.
 -h : display this message.
 -v : set verbosity level [0|1|2].
```

Bitrate parameter is optional, LoRa gateway works always to 38400 bps. Default device is */dev/ttyUSB0*. 

Command output is the following:

```
Current configuration:
	Addr    : 3
	Freq    : 868 MHz
	Chan    : 13
	BW      : 125 KHz
	CR      : 6
	SF      : 12
	SNR     : 0
	RSSI    : -107
	RSSI PCK: 119
```

## lora_setup

This command writea ne configuration in the LoRa module (connected to the LoRa Gateway). This command changes all the LoRa module parameter:

* address: new node address. It is a number between 1 and 255. 0 is not an alloewd value: it is used for broadcast message.
* frequency band: bands allowed are 868 MHz (Europe) and 900 MHz (US).
* channel: channel allowed are 1' to 17 for 868 MHz band and 0 to 12 for 900 MHz band. 
* bandwidth: allowed values are 125, 250 and 500 MHz.
* coding rate: it must be a number between 5 and 8.
* spreading factor: it must be a number between 6 and 12. 

Syntax is:

```
Usage: lora_setup [-v 0|1|2] [-d serial_device] [-b serial_bitrate] [-a address] [-f frequency] [-c channel] [-w bandwidth] [-r coding_rate] [-s spreading_factor]
       lora_setup -h

 -a : node address. It must be a number between 1 and 255. Default value is 0 (broadcast)
 -b : serial bitrate [1200|2400|4800|9600|19200|38400|57600|115200]. Default value is 38400.
 -c : channel. Channel allowed are 1' to 17 for 868 MHz band and 0 to 12 for 900 MHz band. Default channel id 10.
 -d : serial device. Default value is /dev/ttyUSB0.
 -f : frequency band. Bands allowed are 900 and 868 MHz. Default value is 868.
 -r : coding rate. It must be a number between 5 and 8. Default value is 5.
 -s : spreading factor. It must be a number between 6 and 12. Default value is 6.
 -v : set verbosity level [0|1|2] .
 -w : bandwidth. Allowed values are 125, 250 and 500 MHz. Default value is 125.
```

Bitrate parameter is optional, LoRa gateway works always to 38400 bps. Default device is */dev/ttyUSB0*. 
If no errors, command will print the new configuration in the same format of *lora_config* command.


## lora_sender

This command sends an ASCII message to a LoRa node. 


Syntax is:

```
Usage: lora_sender [-v 0|1|2] [-d serial_device] [-b serial_bitrate][-a [0-255]] [-m \"message\"] [-t timeot]
       lora_sender -h

 -a : destination address. It must be a number between 1 and 255, 0 is for broadcast message. Default value is 0 (broadcast)
 -b : serial bitrate [1200|2400|4800|9600|19200|38400|57600|115200]. Default value is 38400.
 -d : serial device. Default value is /dev/ttyUSB0.
 -h : display this message.
 -m : message to send. It must be a string ASCII.
 -t : timeout to wait response in seconds. if it is 0 no response are waited. Default value is 100 seconds
 -v : set verbosity level  [0|1|2].
```

This command waits an acknowledge from the destination, if you want disable this feature you can use the option *-t 0*.


## lora_daemon

This command sends all ASCII message sequence reveived from a FIFO file to a LoRa node. All messages ends with a new line.

Syntax is:

```
Usage: lora_daemon [-v 0|1|2] [-s serial_device] [-b serial_bitrate] [-a [0-255]] [-p <pipe-path>] [-t timeout]
       lora_daemon -h

 -a : destination address. It must be a number between 1 and 255, 0 is for broadcast message. Default value is 0 (broadcast)
 -b : serial bitrate [1200|2400|4800|9600|19200|38400|57600|115200]. Default value is 38400.
 -d : serial device. Default value is /dev/ttyUSB0.
 -h : display this message.
 -p : pipe used for receiving data to send. Default value is /tmp/lora.pipe.
 -t : minimum time between two send operation. Default value is 4 seconds
 -v : set verbosity level  [0|1|2].
```

This command waits an acknowledge from the destination, if you want disable this feature you can use the option *-t 0*.
