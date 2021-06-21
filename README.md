# CO2-detector
For better office air quality.

The detector will alarm you when the CO2 concentration is high enough to make you sleepy.

## Wiring
![Graphical wiring diagram](assets/co2-detector.png)
__Note__: The temperature and humidity sensor in the diagram is DHT22 and it is possible to replace it with DHT11 for lower costs.
In order to use DHT11, you have to edit the definition macro from `#define DHTTYPE DHT22` to `#define DHTTYPE DHT11`.

## Required Libraries
The following libraries are necessary for reading sensor data and displaying.
* __LiquidCrystal I2C__ by Marco Schwartz
* __DHT sensor library__ by Adafruit

Both are available in Arduino IDE library manager.

## 3D Printing Notes
The 3D mesh files are `co2-detector_case.stl` and `co2-detector_lid.stl` under assests directory. It is only tested on Ultimaker 2+ with adhesion option checked, infill 40% and layer height 1.5. The material is white ABS.

__Note__: The latch of the case is fragile. Please do not assemble the case and the lid by force.
