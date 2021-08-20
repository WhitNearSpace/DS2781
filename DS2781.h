
#ifndef MBED_DS2781_H
#define MBED_DS2781_H

#include <mbed.h>
#include "OneWire.h"

#define DS2781_FAMILY_CODE 0x3D

/***
 *  The structure to be returned from debug functions 
 * **/
struct debug_return{
    float data;
    bool success;
};

class DS2781{
    public:
        /**
         * Constructor 
         * **/
        DS2781(OneWire *onewire, char ROM[8] = 0x00);

        /**
         * Overwrite the virtual void function in OneWire
         * 
         * Because this class comes with a set family code, there
         * should be no need for the user to access or set it 
         * **/
        void set_family_code(char family_code);

        /**
         * Return the family code of the device 
        **/
        char read_family_code();

        /**
         * Return the percentage remaining in the battery 
         * */
        uint8_t percentageRemaining();

        /**
         * Read the input voltage of the battery 
         * **/
        float readVoltage();

        /**
         * Read battery temperature 
         * **/
        float readTemp();

        /**
         * Read current 
         * **/
        float readCurrent();

        /***
         *  Writes out the match ROM command and the specific ROM ID
         * 
         * NOTE: DOES NOT SEND OUT A RESET COMMAND 
         * **/
        void matchROM();

        /***
         * Sets data for cell charactersitics 
         * @param data the data to be set 
         * @param reg the register the data is to be written to 
         * **/
        void setData(char data, char reg);

        //////////
        // Debug functions 
        // Read data from the DS2781 and return both the read data
        // and a boolean indicating success/failure of communication
        //////////

        /***
         *  Read voltage from DS2781 
         * @return debug_return structure containing the voltage
         * data and a debug boolean 
         * **/
        debug_return readVoltage_debugger();

        /***
         *  Read temperature from DS2781 
         * @return debug_return structure containing the temperature
         * data and a debug boolean 
         * **/
        debug_return readTemp_debugger();

        /***
         *  Read current from DS2781 
         * @return debug_return structure containing the current
         * data and a debug boolean 
         * **/
        debug_return readCurrent_debugger();

    private:
        char _ROM[8];
        char _family_code = DS2781_FAMILY_CODE; 
        OneWire* _onewire; 
    protected: 
        AnalogIn bus_monitor3V3(p20); // Monitors the 3.3V bus on the power board to determine if the board is being powered by the 1-Wire pin 
        // If the bus is being powered by the 1-Wire line, it drops from ~3.3V to ~1.3V
        // This can be used to indicate that the battery has died 
};

#endif