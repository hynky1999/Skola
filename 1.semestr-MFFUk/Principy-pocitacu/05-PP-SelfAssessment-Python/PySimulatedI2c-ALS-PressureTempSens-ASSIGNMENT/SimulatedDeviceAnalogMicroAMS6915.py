import SimulatedGenericI2cController
from numpy import *
from datetime import timedelta
from datetime import datetime

###
### Simplified simulation of: Analog Microelectronics AMS 6915 pressure and temperature sensor with digital output
###

# AMS 6915-1200-B device variant:
pMin = 700          # mbar
pMax = 1200         # mbar
pMinDigout = 1638   # counts
pMaxDigout = 14745  # counts
pSens = (pMaxDigout - pMinDigout) / (pMax - pMin)
nextMeasurementDelta = 0.05   # second - real device repeats measurement every 0.5 ms, here adjusted to 50 ms for simpler testing

# Simulation parameters:
simBasePressure = 950       # mbar
simPressurePeriod = 5       # seconds
simPressureDeltaMax = 75    # mbar
simBaseTemperature = 22.6      # deg. C
simTemperaturePeriod = 10      # seconds
simTemperatureDeltaMax = 1.4   # deg. C

class DeviceAnalogMicroAMS6915:
    def __init__(self):
        self.pressureCounts = uint16(0);
        self.temperatureCounts = uint16(0);
        self.lastMeasurement = None
        pass

    def start(self, slaveAddr, isRead):
        if (slaveAddr == 0x28 and isRead):
            return True

        return False

    def read(self, bytesExpected):
        global pMin, pMax, pMinDigout, pMaxDigout, pSens, nextMeasurementDelta
        global simBasePressure, simPressurePeriod, simPressureDeltaMax
        global simBaseTemperature, simTemperaturePeriod, simTemperatureDeltaMax

        now = datetime.now()
        if self.lastMeasurement == None or (now - self.lastMeasurement).total_seconds() > nextMeasurementDelta:
            if self.lastMeasurement == None:
                self.firstMeasurement = now

            # Simulation of a real pressure measurement
            pressure = simBasePressure + math.sin(((now - self.firstMeasurement).total_seconds() / simPressurePeriod) * 2 * math.pi) * simPressureDeltaMax
            pressure = round(pressure, 1)

            pDigout = ((pressure - pMin) * pSens) + pMinDigout
            self.pressureCounts = uint16(round(pDigout))

            # Simulation of a real pressure measurement
            temperature = simBaseTemperature + math.sin(((now - self.firstMeasurement).total_seconds() / simTemperaturePeriod) * 2 * math.pi) * simTemperatureDeltaMax
            temperature = round(temperature, 1)

            tDigout = ((temperature + 50) * 2048) / 200
            self.temperatureCounts = uint16(round(tDigout))
            
            self.lastMeasurement = now

            if SimulatedGenericI2cController.enableLog: print(">>> SIMULATION LOG > device AMS 6915 > New pressure:", pressure, ", pDigout:", pDigout, ", counts:", self.pressureCounts)
            if SimulatedGenericI2cController.enableLog: print(">>> SIMULATION LOG > device AMS 6915 > New temperature:", temperature, ", tDigout:", tDigout, ", counts:", self.temperatureCounts)

        pressureMsByte = uint8(self.pressureCounts >> 8) & 0x3F
        pressureLsByte = uint8(self.pressureCounts)
        data = [pressureMsByte, pressureLsByte]

        if (bytesExpected > 2):
            tempMsByte = uint8(self.temperatureCounts >> 3)
            tempLsByte = uint8(self.temperatureCounts << 5)
            data.append(tempMsByte)
            data.append(tempLsByte)

        return data
