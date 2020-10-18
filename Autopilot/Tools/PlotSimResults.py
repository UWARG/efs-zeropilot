import numpy as np
import matplotlib.pyplot as plt
import sys


measuredAirspeed = []
commandedAirspeed = []

try:
    with open ("../SimulationBuild/SensorOutputs/airspeed.txt", "r") as airspeedFile :
        measuredAirspeed = [float(x) for x in airspeedFile.read().split()]
        measuredAirspeed = measuredAirspeed[0::3]

    with open ("../SimulationBuild/ActuatorCommands/throttle.txt", "r") as throttleFile :
        commandedAirspeed = [float(x) for x in throttleFile.read().split()]

except:
    print("Failed to open Simulation file. Make sure you are running this script from the ZeroPilot-SW/Autopilot/Tools and that you have already run the simulation.")
    sys.exit()


# evenly sampled time at 200ms intervals
t = np.arange(0, len(measuredAirspeed), 1)

# red dashes, blue squares and green triangles
plt.plot(t, commandedAirspeed, 'r', label='commanded Airspeed')
plt.plot(t, measuredAirspeed, 'b', label='actual Airspeed')
plt.legend()
plt.show()
