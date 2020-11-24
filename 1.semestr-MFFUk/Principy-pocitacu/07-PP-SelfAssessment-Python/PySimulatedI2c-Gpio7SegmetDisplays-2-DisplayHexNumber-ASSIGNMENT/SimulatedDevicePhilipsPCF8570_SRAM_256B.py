import SimulatedGenericI2cController
from numpy import *
import random

class DevicePhilipsPCF8570_SRAM_256B:
    def __init__(self, pinA2, pinA1, pinA0, randomSeed = 42):
        self.slaveAddress = 0b1010 << 3 | pinA2 << 2 | pinA1 << 1 | pinA0
        self.addressRegister = uint8(0)

        random.seed(randomSeed)         # Get same random sequence every time
        self.byteArray = []
        for i in range(0, 256):
            self.byteArray.append(uint8(random.randint(0, 255)))

    def start(self, slaveAddr, isRead):
        if (slaveAddr != self.slaveAddress):
            return False

        return True

    def write(self, dataBytes):
        if SimulatedGenericI2cController.enableLog: print(">>> SIMULATION LOG > device PCF8570 SRAM > I2C write: addressRegister:", self.addressRegister, "-> ", end="")
        self.addressRegister = uint8(dataBytes[0])
        if SimulatedGenericI2cController.enableLog: print(self.addressRegister, ", writting", len(dataBytes) - 1, "bytes")
        for byteValue in dataBytes[1:]:
            self.byteArray[self.addressRegister] = uint8(byteValue)
            self.addressRegister = uint8(self.addressRegister + 1)

        return len(dataBytes)

    def read(self, bytesExpected):
        if SimulatedGenericI2cController.enableLog: print(">>> SIMULATION LOG > device PCF8570 SRAM > I2C read: addressRegister:", self.addressRegister)
        result = []
        for i in range(0, bytesExpected):
            result.append(self.byteArray[self.addressRegister])
            self.addressRegister = uint8(self.addressRegister + 1)   # Truncate to 8 bits -> equivalent to address space wrap around to 0 from 255 (or mod 256)
            
        return result
