from SimulatedDeviceEverlightALS_PDIC17_57B import *
from SimulatedDeviceAnalogMicroAMS6915 import *
from SimulatedDevicePhilipsPCF8570_SRAM_256B import *
from numpy import *

enableLog = False

connectedI2cDevices = [
    DeviceEverlightALS_PDIC17_57B(),
    DeviceAnalogMicroAMS6915(),
    DevicePhilipsPCF8570_SRAM_256B(1, 1, 0),
    DevicePhilipsPCF8570_SRAM_256B(1, 0, 1, randomSeed = 113),
    DevicePhilipsPCF8570_SRAM_256B(0, 0, 1, randomSeed = 15)
]

class I2c:
    def __init__(self):
        self.currentSlave = None

    def send(self, bytesToSend) -> int:
        if len(bytesToSend) < 1:
            raise Exception("List bytesToSend must contain at least 1 byte, i.e. the address byte.")

        self.currentSlave = None

        addrByte = uint8(bytesToSend[0])
        slaveAddr = addrByte >> 1
        isRead = (addrByte & 0x01) != 0
        if SimulatedGenericI2cController.enableLog: print(">>> SIMULATION LOG > I2C controller > Request for slave: 0x", format(slaveAddr, "02X"), "READ" if isRead else "WRITE", "transaction containing bytes:", bytesToSend)

        for slave in connectedI2cDevices:
            if slave.start(slaveAddr, isRead):
                if isRead:
                    self.currentSlave = slave
                    return 1
                else:
                    dataBytes = bytesToSend[1:]
                    if len(dataBytes) > 0:
                        return slave.write(dataBytes) + 1
                    else:
                        return 1

        if SimulatedGenericI2cController.enableLog: print(">>> SIMULATION LOG > I2C controller > NO SLAVE WITH ADDRESS 0x", format(slaveAddr, "02X"), "ACKNOWLEDGED THIS TRANSACTION!")
        return 0

    def receive(self, bytesExpected : int):
        if self.currentSlave == None:
            raise Exception("No previously addressed and ACKed slave device available.")

        bytesReceived = self.currentSlave.read(bytesExpected)
        if len(bytesReceived) > bytesExpected:
            bytesReceived = bytesReceived[:bytesExpected]
        if len(bytesReceived) < bytesExpected:
            # If slave is sending no more data, but master is still driving clock expecting more bytes from slave
            # the data line will remain in default 1 state (as slave is not pulling it down to 0) -> looks as if slave
            # is sending all bits set to 1:
            bytesReceived.extend([uint8(0xFF)] * (bytesExpected - len(bytesReceived)))

        if SimulatedGenericI2cController.enableLog: print(">>> SIMULATION LOG > I2C controller > Request for up to", bytesExpected, "bytes, slave sent bytes:", bytesReceived)

        self.currentSlave = None
        return bytesReceived



