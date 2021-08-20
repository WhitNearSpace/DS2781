#include "DS2781.h"

/**
 *  Device-specific bytes that deal with cell stack characteristics 
 * **/
#define FULL_SLOPE_4 0x6C // Full segment 4 slope
#define FULL_SLOPE_3 0x6D // Full segment 3 slope
#define FULL_SLOPE_2 0x6E // Full segment 2 slope 
#define FULL_SLOPE_1 0x6F // Full segment 1 slope 
#define AE_SLOPE_4 0x70 // Active empty segment 4 slope 
#define AE_SLOPE_3 0x71 // Active empty segment 3 slope 
#define AE_SLOPE_2 0x72 // Active empty segment 2 slope 
#define AE_SLOPE_1 0x73 // Active empty segment 1 slope
#define TBP34 0x7C // Temperature breakpoint 3
#define TBP23 0x7D // Temperature breakpoint 2
#define TBP12 0x7E // Temperature breakpoint 1

/**
 *  Device-specific bytes that deal with application parameters 
 ***/
#define STORE_SENSE_RESISTOR_VAL 0x69 // Officially called RSNSP, this location in the EEPROM block stores the value of the sense resistor for use in computing the absolute capacity results
#define STORE_CHARGE_VOLTAGE 0x64 // Officially called VCHG, this location in EEPROM block stores the charge voltage threshold used to detect a fully charged state 
#define STORE_MIN_CHARGE_CURRENT 0x65 // Officially called IMIN, this location stores charge current threshold 
#define ACTIVE_EMPTY_VOLTAGE 0x66 // Officially called VAE, this location stores the voltage threshold used to detect the Active Empty point 
#define ACTIVE_EMPTY_CURRENT 0x67 // Officially called IAE, this location stores the discharge current threshold used to detect the Active Empty point 
#define AGING_CAPACITY_MSB 0x62 // Officially called AC, this location stores the rated battery capacity used in estimating the decrease in battery capacity that occurs in normal use  
#define AGING_CAPACITY_LSB 0x63 // The LSB of the aging capacity storage location 
#define AGE_SCALAR // Officially called AS, this location adjusts the capacity estimation results downwared to compensate for cell aging 

/**
 *  Device-specific bytes that deal with data registers  
 * **/
#define VOLT_MSB 0x0C // Voltage register MSB
#define TEMP_MSB 0x0A // Temperature register MSB
#define CURRENT_MSB 0x0E // Current register MSB
#define RARC 0x06 
#define RSRC 0x07

AnalogIn bus_monitor3V3(p20); // Monitors the 3.3V bus on the power board to determine if the board is being powered by the 1-Wire pin 
// If the bus is being powered by the 1-Wire line, it drops from ~3.3V to ~1.3V
// This can be used to indicate that the battery has died 


DS2781::DS2781(OneWire *onewire, char ROM[8]){
    for(int i = 0; i < 8; i++){ // Copy ROM ID
        _ROM[i] = ROM[i];
    }
    _onewire = onewire; // Set onewire pointer 
}

void DS2781::set_family_code(char family_code){ 
    // Do nothing for now, maybe display error 
}

char DS2781::read_family_code(){
    return _family_code; 
}

uint8_t DS2781::percentageRemaining(){
    uint8_t result = 0;
    if(_onewire->reset())
    {       
        _onewire->writeByte(SKIP_ROM);
        _onewire->writeByte(READ_DATA); // Put in read mode 
        _onewire->writeByte(RARC);  // Register
        result  = _onewire->readByte(); // Read byte       
    }
    return result;
}


float DS2781::readVoltage(){
    int16_t result = 0;
    if(_onewire->reset()) // Issue reset command 
    {       
        matchROM();
        _onewire->writeByte(READ_DATA); // Put in read mode 
        _onewire->writeByte(VOLT_MSB);  // Voltage register MSB 
        result  = _onewire->readByte()  << 8;     // Read MSB
        result = result | _onewire->readByte();  // Read LSB
    }
    return (result >> 5)*0.00967; // Get rid of the last 5 reserved bits and account for 9.67 mV resolution 
}

float DS2781::readTemp(){
    int16_t result = 0; 
    if(_onewire->reset()) // Issue reset command 
    {       
        matchROM();
        _onewire->writeByte(READ_DATA); // Set to read mode 
        _onewire->writeByte(TEMP_MSB);  // Temperature register MSB
        result  = (int16_t)_onewire->readByte()  << 8;     // Read MSB   
        result = result | (int16_t)_onewire->readByte() ;  // Read LSB
    }
    return (result >> 5)*0.125; // Get rid of the last 5 reserved bits and account for 0.125 C resolution  
}

float DS2781::readCurrent(){
    uint16_t result = 0; 
    if(_onewire->reset()){
        matchROM(); // Select specific device 
        _onewire->writeByte(READ_DATA); // Set to read mode 
        _onewire->writeByte(CURRENT_MSB); // Current register MSB
        result  = (uint16_t)_onewire->readByte()  << 8;     // Read MSB   
        result = result | (uint16_t)_onewire->readByte() ;  // Read LSB
        if (result & 0x8000) { // Since the reading is in 2's compliment form, if the first bit is 1, the number is negative 
                    result = 0-((result ^ 0xffff) + 1); // Convert from 2's compliment to signed integer
        }
        result = result + 0.0; // Convert to floating point and assign to the return variable
    }
    return result*0.00000332446809; // Return result and account for the resolution (1.5625 uV)/(sense resistance)
}


void DS2781::matchROM(){
    _onewire->writeByte(MATCH_ROM); // Send match command
    // Read out the ROM byte by byte 
    for(int i = 0; i < 8; i++){
        _onewire->writeByte(_ROM[i]);
    }
}

void DS2781::setData(char data, char reg){
    if(_onewire->reset()){ // Issue reset command 
        matchROM(); // Select specific device
        _onewire->writeByte(WRITE_DATA); // Set in write mode 
        _onewire->writeByte(reg); // Select the register  
        _onewire->writeByte(data); // Write out the data byte 
    }
}

debug_return DS2781::readVoltage_debugger(){
    int16_t result = 0;
    debug_return return_vals; // Structure to be returned 
    if(bus_monitor3V3.read() > 0.6) // Check if the battery has died
    {       
        _onewire->reset();
        matchROM();
        _onewire->writeByte(READ_DATA); // Put in read mode 
        _onewire->writeByte(VOLT_MSB);  // Voltage register MSB 
        result  = _onewire->readByte()  << 8;     // Read MSB
        result = result | _onewire->readByte();  // Read LSB
        // Set the return values in the structure 
        return_vals.data = (result >> 5)*0.00967; // Get rid of the last 5 reserved bits and account for 9.67 mV resolution 
        return_vals.success = true; // Set success boolean to TRUE since data was successfully read
    }else{
        return_vals.data = 9999; // Wipe the data value 
        return_vals.success = false; // Set success boolean to FALSE since data was NOT successfully read
    }
    return return_vals;
}

debug_return DS2781::readTemp_debugger(){
    int16_t result = 0; 
    debug_return return_vals; // Structure to be returned 
    if(bus_monitor3V3.read() > 0.6) // Check if the battery has died
    {       
        _onewire->reset();
        matchROM();
        _onewire->writeByte(READ_DATA); // Set to read mode 
        _onewire->writeByte(TEMP_MSB);  // Temperature register MSB
        result  = (int16_t)_onewire->readByte()  << 8;     // Read MSB   
        result = result | (int16_t)_onewire->readByte() ;  // Read LSB
        // Set the return values in the structure 
        return_vals.data = (result >> 5)*0.125; // Get rid of the last 5 reserved bits and account for 0.125 C resolution  
        return_vals.success = true; // Set success boolean to TRUE since data was successfully read
    }else{
        return_vals.data = 0; // Wipe the data value 
        return_vals.success = false; // Set success boolean to FALSE since data was NOT successfully read
    }
    return return_vals;
}

debug_return DS2781::readCurrent_debugger(){
    uint16_t result = 0; 
    debug_return return_vals; 
    if(bus_monitor3V3.read() > 0.6){ // Check if the battery has died
        _onewire->reset();
        matchROM(); // Select specific device 
        _onewire->writeByte(READ_DATA); // Set to read mode 
        _onewire->writeByte(CURRENT_MSB); // Current register MSB
        result  = (uint16_t)_onewire->readByte()  << 8;     // Read MSB   
        result = result | (uint16_t)_onewire->readByte() ;  // Read LSB
        if (result & 0x8000) { // Since the reading is in 2's compliment form, if the first bit is 1, the number is negative 
                    result = 0-((result ^ 0xffff) + 1); // Convert from 2's compliment to signed integer
        }
        result = result + 0.0; // Convert to floating point and assign to the return variable
        // Set the return values in the structure 
        return_vals.data = result*0.00000332446809; // Return result and account for the resolution (1.5625 uV)/(sense resistance)
        return_vals.success = true; // Set success boolean to TRUE since data was successfully read
    }else{
        return_vals.data = 0; // Wipe the data value 
        return_vals.success = false; // Set success boolean to FALSE since data was NOT successfully read
    }
    return return_vals;
}