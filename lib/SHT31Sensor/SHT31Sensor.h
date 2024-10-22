/**
 * SHT31Sensor.h
 * 
 * Declaration of the class SHT31Sensor. It inherits a generic  
 * interface and from Adafruit_SHT31.
 * The constructors argument are the I2C address of the sensor and a
 * reference to a data struct to hold the measuremnts. 
 */
#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_SHT31.h>
#include "SensorData.h"
#include "ISensor.h"

class SHT31Sensor : public ISensor,  Adafruit_SHT31
{  
    public:
        SHT31Sensor(uint8_t i2cAddress, SensorData& sensorData) : 
            Adafruit_SHT31(), _i2cAddress(i2cAddress), _sData(sensorData)
        {}

        void  setup() override;      // initialize the i2c bus and read the sensor
        void  readSensor() override; // refresh the sensor readings
        float getCelsius() override;
        void  printData() override ; // prints the sensor readings from sensor data struct
        SensorData& getDataReference() override;  // returns a referenc to the sensor data struct which holds the sensor readings

    private:
        float _calculateDewPoint();
        uint8_t _i2cAddress;
        SensorData& _sData;
};