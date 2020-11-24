import SimulatedGenericI2cController
from numpy import *
import random

INPUT_PORT0_REGISTER                 = 0x00
OUTPUT_PORT0_REGISTER                = 0x02
POLARITY_INVERSION_PORT0_REGISTER    = 0x04
CONFIG_PORT0_REGISTER                = 0x06

class DeviceTexasInstrumentsTCA9555_GPIO:
    def __init__(self, pinA2, pinA1, pinA0, p10p17Output, p00p07Output):
        self.slaveAddress = 0b0100 << 3 | pinA2 << 2 | pinA1 << 1 | pinA0
        self.outputs = [ p00p07Output, p10p17Output ]
        self.controlRegister = uint8(0)
        
        self.registers = [
            uint8(0x00), uint8(0x00), # Input port 0 and 1 registers
            uint8(0xFF), uint8(0xFF), # Output port 0 and 1 registers
            uint8(0x00), uint8(0x00), # Polarity inversion ports 0 and 1 registers - bit set to 1 inverts polarity of specific bit in input port
            uint8(0xFF), uint8(0xFF)  # Configuration port 0 and 1 registers
        ]
        return

    def start(self, slaveAddr, isRead):
        if (slaveAddr != self.slaveAddress):
            return False

        return True

    def write(self, dataBytes):
        if SimulatedGenericI2cController.enableLog: print(">>> SIMULATION LOG > device TCA9555 GPIO > I2C write: controlRegister:", self.controlRegister, "-> ", end="")
        self.controlRegister = uint8(dataBytes[0]) & 0x07   # TCA9555 datasheet does not state clearly how the device behaves, if the 5 most significant bits
                                                            # of the command byte are not set to 0 as requited by the datasheet. Because bits B2, B1, B0 are stored
                                                            # in control register, we are assuming here that other bits are ignored (we are masking them out).
                                                            # This behavior implies register aliasing of TCA9555 registers "teoretical" 8-bit address space,
                                                            # which may not be the actual behavior of the device.
        if SimulatedGenericI2cController.enableLog: print(self.controlRegister, ", writting", len(dataBytes) - 1, "bytes")
        
        if self.controlRegister & 0xFE == INPUT_PORT0_REGISTER:
            # Input registers are read-only
            pass
        else:
            # print(self.registers)
            for value in dataBytes[1:]:
                self.registers[self.controlRegister] = uint8(value)
                self.updateOutputPort(self.controlRegister & 0x01)
                self.controlRegister = self.controlRegister ^ 0x01  # Flip LSb - multiple reads read repeatedly from same register pair (see figure 30, and 31 in datasheet)
                                                                    # Assuming the same behavior for writes as well.

        return len(dataBytes)

    def updateOutputPort(self, portId):
        outputMask = ~self.registers[CONFIG_PORT0_REGISTER + portId]                # 1 = input, 0 = output, so we need to flip all bits to be able to mask out bits that should not be output.
        outputValue = self.registers[OUTPUT_PORT0_REGISTER + portId] & outputMask   # We assume that 0 is equivalent to floating state in our output simulation.
        self.outputs[portId].outputChanged(outputValue)
        return

    def read(self, bytesExpected):
        result = []
        for i in range(0, bytesExpected):
            result += [ self.registers[self.controlRegister] ]
            self.controlRegister = self.controlRegister ^ 0x01  # Flip LSb - multiple reads read repeatedly from same register pair (see figure 30, and 31 in datasheet)

        return result
