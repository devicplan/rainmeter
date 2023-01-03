# Rainmeter with LoRaWAN network

For quite some time I had planned to measure the domestic amount of precipitation. There are simple rain gauges with a collecting vessel. However, the rainfall must be read, emptied and documented regularly. There are not many electronic rain gauges with radio technology and often a gateway with software is necessary here in addition to the rain gauge itself. The mechanical self-construction is tedious and very costly. Therefore, I decided to acquire the rain gauge and convert it for my purposes.

## Target:
"-" Battery operation with at least 1 year battery life
"-" Possibility of a universal connection for almost all home visualizations (ioBroker, FHEM, Mosquito ...)

## Implementation / result:
- Battery operation with an expected battery life of 2 years.
- data transmission via an energy saving LoRaWAN wireless connection to the TTS network
- from the TTS network the data is sent to Thingspeak via MQTT for logging and visualization
- the above mentioned home automations can process the data from Thingspeak or also directly from the TTS network via MQTT
- also a Raspberry or an ESP can fetch data from the cloud of Thingspeak for further processing with a few lines of code 

# here you can read more  
https://icplan.de/seite40/

# Regenmesser mit LoRaWAN Netzwerk

Schon längere Zeit hatte ich vor, die heimische Niederschlagsmenge zu messen. Es gibt einfache Regenmesser mit einem Sammelgefäß. Die Regenmengen müssen aber regelmäßig abgelesen, geleert und dokumentiert werden. Elektronische Regenmesser mit Funktechnologie gibt es nicht viele und oft ist hier neben dem Regenmesser selbst auch noch ein Gateway mit Software notwendig. Der mechanische Selbstbau ist mühsam und sehr aufwändig. Daher entschied ich mich den Regenmesser zu erwerben und diesen für meine Zwecke umzubauen.

## Ziel:
– Batteriebetrieb mit mindestens 1 Jahr Batterielaufzeit
– Möglichkeit einer universellen Anbindung für nahezu alle Home-Visualisierungen (ioBroker, FHEM, Mosquito …)

## Umsetzung / Ergebnis:
– Batteriebetrieb mit einer erwarteten Batterielaufzeit von 2 Jahren
– Datenversand über eine energiesparende LoRaWAN Funkverbindung zum TTS Netzwerk
– aus dem TTS  Netzwerk werden die Daten per MQTT für den Log und eine Visualisierung zu Thingspeak gesendet
– die oben genannten Homeautomatisierungen können dabei die Daten von Thingspeak oder auch direkt aus dem TTS Netzwerk per MQTT verarbeiten
– auch ein Raspberry oder ein ESP kann sich mit wenigen Programmzeilen-Code Daten aus der Cloud von Thingspeak für eine Weiterverarbeitung holen 

# hier kannst Du weiterlesen 
https://icplan.de/seite40/
