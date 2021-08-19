# The hitchhiker's guide to the DS2781 datasheet  
Lydia Calderon-Aceituno | August 2021

The DS2781 measures voltage, temperature, and current, and estimates available capacity for rechargeable Lithium-Ion and Lithium-Ion Polymer batteries. Cell stack characteristics and application parameters used in the calculations are stored in onchip EEPROM. In theory, it is a wonderful little module that can provide great insight for LiPo-powered projects. The only problem is understanding how it actually works. This document attempts to provide a human-friendly translation of the [official Maxim documentation](https://datasheets.maximintegrated.com/en/ds/DS2781.pdf). 

## Communicating with the module 
The DS2781 communicates via 1-Wire a serial signaling protocol like SPI or I2C with the key difference that power, data, and timing can be sent over only one wire. A full review on how 1-Wire works can be found here **INSERT GITHUB LINK**  

## Getting data 
Some parameters, like voltage and current, are relatively easy to monitor by simply selecting the device or issuing a skip command, setting the communication to read mode, and then reading the data at the appropriate register. Other parameters, like those that deal with battery monitoring, are much more of a headache. 

In order to get meaningful data from the battery monitoring registers, you must first configure the cell stack characteristics and the application parameters so that your DS2781 knows what type of battery it is dealing with.

### Cell stack characteristics
The DS2781 considers cell stack performance characteristics over temperature, load current, and charge termination when making estimations about remaining battery capacity. Full and empty points are retrieved in a process which re-traces a piece-wise linear model of consisting of three model curves named Full, Active Empty, and Stand-by Empty. Each model curve is made up of 5 segments. The three junctions that join the segments ((labeled TBP12, TBP23 and TBP34) are programmable values from -128°C to +40°C. They are stored in 2's complement form. 

