import SimulatedGenericI2cController
from numpy import *
import time

"""
README
-------------------------------------
Naimplemntovány jsou všechny úlohy, takže je jedno, kterou zkontrolujete. Pokud se nepletu, tak ve vaši implementaci i2c controlleru je chyba a 1=W, 0=R. Musel jsem proto přizpůsobit volání.
-------------------------------------
"""
# NOTE: Uncomment the following line to enable log messages from inside of the I2C bus and device simulation:
# SimulatedGenericI2cController.enableLog = True
i2c = SimulatedGenericI2cController.I2c()

### Actual implementation here - ALS measurement:
### Exercise 16AL and 17AL from self-assessment assignments for 5th lecture

# TODO: Add any initialization if necessary
i2c.send([0x53, 0x04])
for i in range(0, 15):
    # TODO: Ask ALS-PDIC17-57B/TR8 to do 300 ms integration and light intensity measurement
    i2c.send([0x53, 0x08])
    time.sleep(0.3)
    i2c.send([0x53, 0x30])
    i2c.send([0x52])
    rcv_bytes = i2c.receive(2)
    count =  ((rcv_bytes[1] & 0x7F)  << 8) | rcv_bytes[0]      # TODO: From returned packet parse the actual value of measured "counts" (exercise 16AL and 17AL)
    print("Main program: Received count", count, end=" -> ")
    value = count*0.45       # TODO: From count value compute the actual light intensity measured in lux (exercise 17AL)
    print("measured light intensity of", round(value, 2), "lux")

print()

### Actual implementation here - Pressure and temperature measurement:
### Exercise 18TT and 19TT from self-assessment assignments for 5th lecture

# TODO: Add any initialization if necessary
pMin = 700
pMax = 1200
pMaxDigout = 14745
pMinDigout = 1638
sens = (pMaxDigout-pMinDigout)/(pMax-pMin)
for i in range(0, 10):
    if(i != 0):
        time.sleep(0.25)
    # TODO: Send request for pressure and temperature measurement to I2C connected AMS 6915-1200-B sensor
    i2c.send([0x50])
    rcv_bytes = i2c.receive(4)

    # Pressure:
    pDigout = ((rcv_bytes[0] & 0x3F) << 8) | rcv_bytes[1]      # TODO: Get pDigout (encoded pressure data) from received packet (exercise 18TT and 19TT)

    p = (pDigout-pMinDigout)/sens + pMin          # TODO: And compute original pressure value p in milibar (exercise 19TT)

    # Temperature:
    tDigout = (rcv_bytes[2] << 3) | (rcv_bytes[3] >> 5)    # TODO: Get tDigout (encoded temperature) from received packet (exercise 18TT and 19TT)
    t = tDigout*200/2048 - 50           # TODO: And compute original temperature value t in degrees Celsius (exercise 19TT)

    print("Main program: pDigout:", pDigout, ", pressure", round(p, 1), "mbar, tDigout:", tDigout, ", temperature", round(t, 1), "deg. C")

### Expected output for ambient light sensor measurements:
# Main program: Received count 20018 -> measured light intensity of 9008.1 lux
# Main program: Received count 20022 -> measured light intensity of 9009.9 lux
# Main program: Received count 20020 -> measured light intensity of 9009.0 lux
# Main program: Received count 22184 -> measured light intensity of 9982.8 lux
# Main program: Received count 23358 -> measured light intensity of 10511.1 lux
# Main program: Received count 23360 -> measured light intensity of 10512.0 lux
# Main program: Received count 22456 -> measured light intensity of 10105.2 lux
# Main program: Received count 10218 -> measured light intensity of 4598.1 lux
# Main program: Received count 6700 -> measured light intensity of 3015.0 lux
# Main program: Received count 5113 -> measured light intensity of 2300.85 lux
# Main program: Received count 5122 -> measured light intensity of 2304.9 lux
# Main program: Received count 5116 -> measured light intensity of 2302.2 lux
# Main program: Received count 5104 -> measured light intensity of 2296.8 lux
# Main program: Received count 5109 -> measured light intensity of 2299.05 lux
# Main program: Received count 5109 -> measured light intensity of 2299.05 lux

### Expected output for pressure and temperature sensor measurements:
# Main program: pDigout: 8192 , pressure 950.0 mbar, tDigout: 743 , temperature 22.6 deg. C
# Main program: pDigout: 8802 , pressure 973.3 mbar, tDigout: 745 , temperature 22.8 deg. C
# Main program: pDigout: 9350 , pressure 994.2 mbar, tDigout: 748 , temperature 23.0 deg. C
# Main program: pDigout: 9785 , pressure 1010.8 mbar, tDigout: 750 , temperature 23.2 deg. C
# Main program: pDigout: 10063 , pressure 1021.4 mbar, tDigout: 752 , temperature 23.4 deg. C
# Main program: pDigout: 10158 , pressure 1025.0 mbar, tDigout: 754 , temperature 23.6 deg. C
# Main program: pDigout: 10058 , pressure 1021.2 mbar, tDigout: 755 , temperature 23.7 deg. C
# Main program: pDigout: 9775 , pressure 1010.4 mbar, tDigout: 756 , temperature 23.8 deg. C
# Main program: pDigout: 9334 , pressure 993.6 mbar, tDigout: 757 , temperature 23.9 deg. C
# Main program: pDigout: 8781 , pressure 972.5 mbar, tDigout: 758 , temperature 24.0 deg. C

