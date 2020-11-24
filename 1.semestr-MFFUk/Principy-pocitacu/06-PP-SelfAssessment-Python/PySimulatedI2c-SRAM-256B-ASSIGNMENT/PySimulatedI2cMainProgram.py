import SimulatedGenericI2cController
from numpy import *
import time

def WriteTo(slaveAddr : uint8) -> uint8:
    return slaveAddr << 1

def ReadFrom(slaveAddr : uint8) -> uint8:
    return (slaveAddr << 1) | 0x01

# NOTE: Uncomment the following line to enable log messages from inside of the I2C bus and device simulation:
# SimulatedGenericI2cController.enableLog = True
i2c = SimulatedGenericI2cController.I2c()
SRAM_ADDRESS = 0x56
### Actual implementation of exercise 14 - use connected PCF8570 SRAM with A0 = 0, A1 = 1, A2 = 1:


# TODO: Write value 123 to byte at address 10 
i2c.send([WriteTo(SRAM_ADDRESS), 10, 123])
# TODO: Read back value of byte at address 10, and print it to output -> should be value 123 written above
i2c.send([WriteTo(SRAM_ADDRESS), 10])
i2c.send([ReadFrom(SRAM_ADDRESS)])
data = i2c.receive(1)

print("Byte at addres 10: ", data[0])
### Actual implementation of exercise 15 - use connected PCF8570 SRAM with A0 = 0, A1 = 1, A2 = 1:

# TODO: Read all bytes from SRAM, and print them to output:
# TODO: Read 4 bytes from address $08, and print them to output
# TODO: Read 2 bytes from address $FE + read 2 bytes from address $00 and print all to outpu
# TODO: Write bytes 100, 0, 200, 0, 100 to address $04
# TODO: Read all bytes from SRAM again, and print them to output:

print()
### Actual implementation of exercise 16 - use connected PCF8570 SRAM with A0 = 0, A1 = 1, A2 = 1 as source memory:

# TODO: Read 4 bytes from top of source memory (from highest addresses)
# TODO: Then probe (look for) for any other PCF8570 SRAM connected to the I2C bus,
#       and for every found target SRAM (with the exception of the original source memory):
# TODO: Step 1: Read 8 bytes from address $00, and print them to output
# TODO: Step 2: Clear the whole memory (= write value 0x00 to every of its bytes)
# TODO: Step 3: Write the 4 "top bytes" (see above) from source memory to address $00 in found target memory
# TODO: Step 4: Read 8 bytes from address $00 once again, and print them to output

### Expected output for exercise 14:
# Byte at address 10: 123

### Expected output for exercise 15 includes also:
# All SRAM data: [57, 12, 140, 125, 114, 71, 52, 44, 216, 16, 123, 47, 111, 119, 13, 101, 214, 112, 229, 142, 3, 81, 216, 174, 142, 79, 110, 172, 52, 47, 194, 49, 183, 176, 135, 22, 235, 63, 193, 40, 150, 185, 98, 35, 23, 116, 148, 40, 119, 51, 194, 142, 232, 186, 83, 189, 181, 107, 136, 36, 87, 125, 83, 236, 194, 138, 112, 166, 28, 117, 16, 161, 205, 137, 33, 108, 161, 108, 255, 202, 234, 73, 135, 71, 126, 134, 219, 204, 185, 112, 70, 252, 46, 24, 56, 78, 81, 216, 32, 197, 195, 239, 128, 5, 58, 136, 174, 57, 150, 222, 80, 232, 1, 134, 91, 54, 152, 101, 78, 191, 82, 0, 165, 250, 9, 57, 185, 157, 122, 29, 123, 40, 43, 248, 35, 64, 65, 243, 84, 135, 216, 108, 102, 159, 204, 191, 224, 231, 61, 126, 115, 32, 173, 10, 117, 112, 3, 36, 30, 117, 34, 16, 169, 36, 121, 142, 248, 109, 67, 242, 124, 242, 208, 97, 48, 49, 220, 181, 216, 210, 239, 27, 50, 31, 206, 173, 55, 127, 98, 97, 229, 71, 216, 93, 142, 236, 127, 38, 226, 50, 25, 7, 47, 121, 85, 208, 248, 246, 109, 205, 30, 84, 194, 1, 199, 135, 232, 146, 216, 249, 79, 97, 151, 111, 29, 31, 160, 29, 25, 244, 80, 29, 41, 95, 35, 34, 120, 206, 61, 126, 20, 41, 214, 161, 133, 104, 160, 122, 135, 202, 67, 153, 234, 161, 37, 4] , 256 bytes
# Bytes at addresses $08, $09, $0A, $0B: [216, 16, 123, 47]
# Bytes at addresses $FE, $FF, $00, $01: [37, 4, 57, 12]
# 5 bytes written to address $04.
# All SRAM data: [57, 12, 140, 125, 100, 0, 200, 0, 100, 16, 123, 47, 111, 119, 13, 101, 214, 112, 229, 142, 3, 81, 216, 174, 142, 79, 110, 172, 52, 47, 194, 49, 183, 176, 135, 22, 235, 63, 193, 40, 150, 185, 98, 35, 23, 116, 148, 40, 119, 51, 194, 142, 232, 186, 83, 189, 181, 107, 136, 36, 87, 125, 83, 236, 194, 138, 112, 166, 28, 117, 16, 161, 205, 137, 33, 108, 161, 108, 255, 202, 234, 73, 135, 71, 126, 134, 219, 204, 185, 112, 70, 252, 46, 24, 56, 78, 81, 216, 32, 197, 195, 239, 128, 5, 58, 136, 174, 57, 150, 222, 80, 232, 1, 134, 91, 54, 152, 101, 78, 191, 82, 0, 165, 250, 9, 57, 185, 157, 122, 29, 123, 40, 43, 248, 35, 64, 65, 243, 84, 135, 216, 108, 102, 159, 204, 191, 224, 231, 61, 126, 115, 32, 173, 10, 117, 112, 3, 36, 30, 117, 34, 16, 169, 36, 121, 142, 248, 109, 67, 242, 124, 242, 208, 97, 48, 49, 220, 181, 216, 210, 239, 27, 50, 31, 206, 173, 55, 127, 98, 97, 229, 71, 216, 93, 142, 236, 127, 38, 226, 50, 25, 7, 47, 121, 85, 208, 248, 246, 109, 205, 30, 84, 194, 1, 199, 135, 232, 146, 216, 249, 79, 97, 151, 111, 29, 31, 160, 29, 25, 244, 80, 29, 41, 95, 35, 34, 120, 206, 61, 126, 20, 41, 214, 161, 133, 104, 160, 122, 135, 202, 67, 153, 234, 161, 37, 4] , 256 bytes

### Expected output for exercise 16:
# Found SRAM at address 0x 51 , bytes $00-$07: [106, 5, 18, 80, 122, 8, 28, 75] -> [234, 161, 37, 4, 0, 0, 0, 0]
# Found SRAM at address 0x 55 , bytes $00-$07: [15, 140, 9, 244, 178, 122, 223, 15] -> [234, 161, 37, 4, 0, 0, 0, 0]
