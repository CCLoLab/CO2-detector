# CO2-detector
For better office air quality.

The detector will alarm you when the CO2 concentration is high enough to make you sleepy.

## Wiring
![Graphical wiring diagram](assets/co2-detector.png)
__Note__: The temperature and humidity sensor in the diagram is DHT22 and it is possible to replace it with DHT11 for lower costs. Also, DHT11 is the default setting in our program currently. 
In order to using DHT22, you have to edit the definition macro from `#define DHTTYPE DHT11` to `#define DHTTYPE DHT22`.

## Required Libraries
The following libraries are necessary for reading sensor data and displaying.
* LiquidCrystal
* DHT sensor library
Both are available in Arduino IDE library manager.
