/**
 * Program      A thermostat with a sensor interface wich enables the use of different sensors
 * 
 * Author       2024-10-19 Charles Geiser
 * 
 * Purpose      The program shows how a SHT31 temperature and humidity sensor is queried.  
 *              It also implements the functionality of a thermostat. That means the method
 *              turnHeatingOff() is called when temperature exceeds the set high tempereature 
 *              limit or the method turnHeatingOn() is called when tempereature falls below 
 *              the set low temperature limit. 
 * 
 *              From the measured temperature and humidity we calculate the dewpoint. This is  
 *              the temperature at which the air becomes saturated and it "starts to rain". 
 *   
 *              The sensor updates the measured values ​​every msRefresh milliseconds and the 
 *              user supplied method processData() is called.
 *  
 * 
 * Board        ESP32 DoIt DevKit V1 
 * 
 * Wiring                                                                                  Solid State Relay
 *                                                                                            .---------.      
 *                                                                        5V(Vin of ESP32) ---|+       ~|------------- L 
 *                         -----.                   .----------.                              |   SSR   |    .-.        ~230V 
 *             SHT31 Sensor     |-- Vcc --> 3.3V  --|  ESP32   |-- 2 Heartbeat           .----|-       ~|---( L )----- N
 *             I2C              |-- GND --> GND   --|          |                         |    `---------´    `-´
 *             Temperature      |-- SCL --> 22    --|          |                     |¦--'                  Load
 *             rel Humidity     |-- SAA --> 21      |          |                     |¦<-.  2N7000   
 *                         -----´                   |          |--  4 --> heating ---|¦--|  N-CH MOSFET        
 *                                                  |          |                         | 
 *                                                  ´----------´                   GND --+--  
 * 
 *              The solid state relay should switch on at a voltage of 3V, but direct control with an output of the ESP32 
 *              does not work. Therefore the relay is connected to Vin (5 V) and switched on with the N-Channel MOSFET against GND 
 *                .---.
 *              /_____/|
 *              | 2N  ||
 *              | 7000||
 *              |_____|/
 *               | | |
 *               S G D
 * 
 * Remarks      
 *              
 * Credits      Thanks to Stéphane Calderoni, for sharing his knowledge with the readers
 *              of the Random Nerd Tutorial Lab forum.          
 * 
 * References   
 */

#include <Arduino.h>
#include "Thermostat.h"

#define PIN_THERMOSTAT  GPIO_NUM_4   // pin to turn on/off the heating
#define PIN_HEARTBEAT   LED_BUILTIN  // indicates normal operation with 1 beat/sec or error state with 5 beats / sec
#define SHT31_I2C_ADDR  0x44


extern void heartbeat(uint8_t pin, uint8_t nBeats, uint8_t t, uint8_t duty);
extern void doMenu();
extern void setLowerLimit();
extern void setUpperLimit();
extern void setTempDelta();
extern void setInterval();
extern void toggleThermostat();
extern void showValues();
extern void showMenu();

bool heatingIsOn = false; 

SensorData  sensorData; // holds measured and calculated sensor values (see Isensor.h)
SHT31Sensor sensor(SHT31_I2C_ADDR, sensorData); // sensor used for thermostat

// Forward declaration of the handler functions for the thermostat
void processData();
void turnHeatingOn();
void turnHeatingOff();

Thermostat thermostat(sensor, processData, turnHeatingOn, turnHeatingOff);
//const BME280Data& sDataRef = myThermostat.getSensorDataRef();  // const makes access by reference readonly

// Called when refresh intervall expires
void processData()
{
  sensor.readSensor();
  sensor.printData();
  thermostat.printSettings(); 
}

// Called as onLowTemp() when the temperature falls below the set limit
void turnHeatingOn()
{
  if (! heatingIsOn)
  {
    log_i("===> switch on heating, it is now: %s", heatingIsOn ? "on" : "off");
    digitalWrite(PIN_THERMOSTAT, HIGH);
    heatingIsOn = true;
  }
}

// Called as onHighTemp() when the temperature rises above the set limit
void turnHeatingOff()
{
  if (heatingIsOn)
  {
  log_i("===> switch off heating, it is now: %s", heatingIsOn ? "on" : "off");
  digitalWrite(PIN_THERMOSTAT, LOW);
  heatingIsOn = false;
  }
}


void initOutputPins()
{
  pinMode(PIN_HEARTBEAT, OUTPUT);
  pinMode(PIN_THERMOSTAT, OUTPUT);
  digitalWrite(PIN_THERMOSTAT, LOW);
  log_i("==> done");  
}


void initThermostat()
{
  thermostat.setup();
  thermostat.enable();
  log_i("==> done");
}


void setup() 
{
  Serial.begin(115200);

  initOutputPins(); 
  initThermostat();
  showMenu();
}

void loop() 
{
  if(Serial.available()) doMenu();
  thermostat.loop();
  heartbeat(PIN_HEARTBEAT, 1, 1, 5);
}