/**
 * Class        SHT31Sensor
 * Author       2024-10-19 Charles Geiser
 * 
 * Purpose      Implements the class SHT31Sensor to measure temperature and relative  
 *              humidity by means of a SHT31 sensor. It also calculates the dew point  
 * 
 * Board        ESP32 DoIt DevKit V1
 * Remarks
 * References   
 */ 
#include "SHT31sensor.h"

/** Initializes the sensor and reads the measurement data 
 * or gives an error message if the sensor was not found 
 * and stops the program
*/
void SHT31Sensor::setup()
{
    int tries = 5;
    while (! begin(_i2cAddress) && (tries > 0))
    {
        delay(1000);
        tries--;
    }
    if(tries <= 0)
    {
        Serial.printf("SHT31 not found at i2c address %#x", _i2cAddress);
        while(true) delay(10000UL);
    }
    readSensor();
    log_i("==> done");
}


/**
 * Read the sensor and calculate local normal pressure
 * and dew point
 */
void SHT31Sensor::readSensor()
{
    _sData.relHumidity = readHumidity();
    _sData.tCelsius    = readTemperature();
    _sData.tFahrenheit = _sData.tCelsius * 9.0 / 5.0 + 32.0;
    _sData.tKelvin     = _sData.tCelsius + 273.15;
    _sData.dewPoint    = _calculateDewPoint();      
}


float SHT31Sensor::getCelsius()
{
    return _sData.tCelsius;
}

/**
 * Calculate dew point in °C from temperature and humidity
 */
float SHT31Sensor::_calculateDewPoint() 
{
    float k;
    k = log(_sData.relHumidity/100) + (17.62 * _sData.tCelsius) / (243.12 + _sData.tCelsius);
    return 243.12 * k / (17.62 - k);
}


/**
 * Returns a reference to the sensor data struct
 */
SensorData&  SHT31Sensor::getDataReference()
{
    return(_sData);
}


void SHT31Sensor::printData()
{
    readSensor();

    Serial.printf(R"(---   Sensor Readings   ---
Tc               %6.1f °C
Tf               %6.1f °F
Tk               %6.1f °K
Dewpoint         %6.1f °C
Humidity         %6.1f %%rH

)", _sData.tCelsius, _sData.tFahrenheit, _sData.tKelvin, _sData.dewPoint, _sData.relHumidity);
}
