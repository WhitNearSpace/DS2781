#include <mbed.h>
#include "DS2781.h"
#include "OneWire.h"
 
 // ROM ID of the battery monitor
 // Note this ID is bit-sequential with the LSB coming first 
char battROM[8] = {0x3D, 0xD0, 0x1B, 0xD7, 0x00, 0x00, 0x00, 0x63};

OneWire onewire(p9);
DS2781 ds27(&onewire, battROM);


int main() {
    float voltage, current, temp; // Initialize data variables  
    while(1){
        // Collect data
        ThisThread::sleep_for(59700ms); // 59.7 second delay 
        voltage = ds27.readVoltage();
        ThisThread::sleep_for(100ms); // Time buffer
        current = ds27.readCurrent();
        ThisThread::sleep_for(100ms); // Time buffer
        temp = ds27.readTemp();
        ThisThread::sleep_for(100ms); // Time buffer

        // Write data to the log.txt file 
        // fprintf has the same format string options as printf 
        printf("Temp: %.3f C\n", temp);
        printf("Voltage: %.3f V\n", voltage);
        printf("Current: %.6f A\n", current);
        printf("\n");
    }
}