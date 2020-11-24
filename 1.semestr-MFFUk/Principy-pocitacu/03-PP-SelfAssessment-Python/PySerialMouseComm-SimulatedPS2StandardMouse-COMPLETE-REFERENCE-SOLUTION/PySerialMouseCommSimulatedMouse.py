import serial
import time
import keyboard
from datetime import timedelta

# Below is a list of simulated events that will occur on the simulated RS-232 controller after program starts.
# Every line represents an event of simulated single byte reception from a simulated serial mouse.
# The value after the "seconds=" argument represents a relative time in seconds from program start, when the simulated byte will appear in the controller's data register.
# The second number of each pair below is the actual value of the byte received, that will be placed into the controller's data register at the specified time.

# This is an example of data captured from a mouse using the "PS/2 standard mode" communication protocol:
serial.setSimulatedInputEvents([
    (timedelta(seconds=1.0), 0x08),
    (timedelta(seconds=1.1), 0),
    (timedelta(seconds=1.2), 0),
    
    (timedelta(seconds=1.5), 0x09),
    (timedelta(seconds=1.6), 0),
    (timedelta(seconds=1.7), 0),

    (timedelta(seconds=2.0), 0x0B),
    (timedelta(seconds=2.1), 0),
    (timedelta(seconds=2.2), 0),

    (timedelta(seconds=2.5), 0x0F),
    (timedelta(seconds=2.6), 0),
    (timedelta(seconds=2.7), 0),

    (timedelta(seconds=3.0), 0x0E),
    (timedelta(seconds=3.1), 8),
    (timedelta(seconds=3.2), 9),

    (timedelta(seconds=3.5), 0x0A),
    (timedelta(seconds=3.6), 241),
    (timedelta(seconds=3.7), 109),

    (timedelta(seconds=4.0), 0x1A),
    (timedelta(seconds=4.1), 8),
    (timedelta(seconds=4.2), 10),

    (timedelta(seconds=5.0), 0x38),
    (timedelta(seconds=5.1), 100),
    (timedelta(seconds=5.2), 10),

    (timedelta(seconds=6.0), 0x28),
    (timedelta(seconds=6.1), 100),
    (timedelta(seconds=6.2), 0),

    (timedelta(seconds=7.0), 0x29),
    (timedelta(seconds=7.1), 0),
    (timedelta(seconds=7.2), 255),
])

print("Initializing RS-232 port ...")

serialPort = serial.Serial()
serialPort.baudrate = 1200
serialPort.bytesize = serial.EIGHTBITS
serialPort.stopbits = serial.STOPBITS_ONE
serialPort.parity = serial.PARITY_NONE

#region RS-232 port and mouse initialization
serialPort.port = "COM4"

serialPort.timeout = 0.5

serialPort.open()

print("Initializing serial mouse ...")

# Make sure the mouse is powered down

serialPort.setRTS(False);
serialPort.setDTR(False);

time.sleep(0.150)
serialPort.flushInput();
serialPort.flushOutput();
time.sleep(0.150)

# Power up the mouse
serialPort.setDTR(True);
serialPort.setRTS(True);

# Read mouse identification data
# -> The identification data are not important to us, as we have single mouse type protocol hard-wired in our code,
#    so we read and throw ways everything mouse sends us at this time.
#    We expect the user not to move or click the mouse close to starting up the application, so the 0.5 second timeout
#    should be reasonable pause between initial mouse transmission of its identification data, and later data of actual
#    mouse move packets. 

done = False
while not done:
    data = serialPort.read(1024)
    if len(data) == 0:
        done = True
    else:
        print("Mouse identification data: ", data)
    
print("Serial mouse initialization completed ...")
#endregion

# Actual communication using PS/2 standard mode mouse protocol

####
#### VERSION 3: Getting info about pressed buttons ####    
####
print()
print("V3: Using bitwise ops to parse button state: waiting for mouse packets (press and hold Ctrl key to continue to next example):")
done = False
pkt = []
while not done:
    # Read 3 byte packet from mouse into pkt list variable ...
    nextByte = serialPort.read(1)
    if len(nextByte) == 1:
        pkt.append(nextByte[0])
        if len(pkt) == 3:  # We got the whole 3 byte packet, start processing it ...

            print("Packet[", format(pkt[0], "02X"), format(pkt[1], "02X"), format(pkt[2], "02X"), "]=", end="")

            if pkt[0] & 0x01 != 0: 
                print("L", end="")
            else: 
                print("-", end="")

            if pkt[0] & 0x04 != 0: 
                print("M", end="")
            else: 
                print("-", end="")

            if pkt[0] & 0x02 != 0: 
                print("R", end="")
            else: 
                print("-", end="")

            x = pkt[1]
            if pkt[0] & 0x10 != 0:
                x |= 0x100

            print(", x=", x, end="")

            y = pkt[2]
            if pkt[0] & 0x20 != 0:
                y |= 0x100

            print(", y=", y, end="")

            print(flush=True)   # Put every packet info on its own line.
            pkt = []

    # If Ctrl key is pressed, then ...
    #region
    if keyboard.is_pressed('ctrl'):
        print("*** Ctrl key is now pressed, please release Ctrl key to continue ***")
        while keyboard.is_pressed('ctrl'):
            pass
        #endregion
        done = True

serialPort.close()
