# The hitchhiker's guide to the DS2781 datasheet  
Lydia Calderon-Aceituno | August 2021

The DS2781 measures voltage, temperature, and current, and estimates available capacity for rechargeable Lithium-Ion and Lithium-Ion Polymer batteries. Cell stack characteristics and application parameters used in the calculations are stored in onchip EEPROM. In theory, it is a wonderful little module that can provide great insight for LiPo-powered projects. The only problem is understanding how it actually works. This document attempts to provide a human-friendly translation of the [official Maxim documentation](https://datasheets.maximintegrated.com/en/ds/DS2781.pdf). 

## Communicating with the module 
The DS2781 communicates via 1-Wire a serial signaling protocol like SPI or I2C with the key difference that power, data, and timing can be sent over only one wire. A full review on how 1-Wire works can be found here **INSERT GITHUB LINK**  

## Getting data 
Some parameters, like voltage and current, are relatively easy to monitor by simply selecting the device or issuing a skip command, setting the communication to read mode, and then reading the data at the appropriate register. Other parameters, like those that deal with battery monitoring, are much more of a headache. 

In order to get meaningful data from the battery monitoring registers, you must first configure the cell stack characteristics and the application parameters so that your DS2781 knows what type of battery it is dealing with.

### Cell stack characteristics
The DS2781 considers cell stack performance characteristics over temperature, load current, and charge termination when making estimations about remaining battery capacity. Full and empty points are retrieved in a process which re-traces a piece-wise linear model of consisting of three model curves named Full, Active Empty, and Stand-by Empty. Each model curve is made up of 5 segments. The three junctions that join the segments ((labeled TBP12, TBP23 and TBP34) are programmable values from -128°C to +40°C. They are stored in 2's complement form. The slope or derivative for segments 1, 2, 3, and 4 are also programmable. The full curve defines how the full point of the cell stack depends on temperature for a given charge termination.  The active empty curve defines the temperature variation in the empty point of the discharge profile based on a high level load current. 

### Model construction 
Both the +40°C Full value (in mVh) and +40°C Active Empty value (expressed as a fraction of the +40°C Full value) are stored in the cell parameter EEPROM block along with the break point temperatures of each segment. An example of the data stored in this manner is shown in the table below. This data reflects the real conditions on the High Altitude Ballon Command Module 2.2: Power Board. 

| Sense resistor | Cell capacity | Charge current | Charge voltage |
|----------------| ------------|-------------| ----------------|
| 470 mΩ | 3000 mAh | 0.6 A | 8.3 V |  


| Termination current | Active empty (V,I) | Standby Empty (V,I) | 
|----------------| ------------|-------------|
| unknown | unknown | unknown | 

Segment break points:
| TBP12 | TBP23 | TBP34 | 
|----------------| ------------| -------|
| cold room? | fridge? | 23ºC (room temp)| 

### Application parameters 
In addition to the cell model, several application parameters are needed to get an accurate estimation of the battery capacity. These are:
* The value of the sense resistor 
* The charge voltage threshold 
* The charge current threshold 
* The voltage threshold of the Active Empty point 
* The  discharge current threshold used to detect the Active Empty point 
* The rated battery capacity 
* The Age Scalar, which adjusts the capacity estimation results downward to compensate for cell aging. Usually, writing AS by the host is not necessary because AS is
automatically saved to EEPROM on a periodic basis.