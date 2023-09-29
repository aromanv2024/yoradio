/* 
 *  18650 Ion-Li battery
 *  Fork from https://github.com/danilopinotti/Battery18650Stats
 */

#ifndef Battery18650Stats_H_
#define Battery18650Stats_H_
#include "../core/options.h"

#if BATTERY_PIN!=255
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "Arduino.h"

class Battery {
 public:
  Battery();
  ~Battery();
  int getBatteryChargeLevel(bool useConversionTable = false);
  double getBatteryVolts();
  void init();
 private:
  double *_conversionTable = nullptr;
  int _mv;
  adc1_channel_t _channel;
  esp_adc_cal_characteristics_t _adc_chars;
  void _initConversionTable();
  bool _validAdcPin;
  bool _validateAdcPin();
  int _getChargeLevelFromConversionTable(double volts);
  int _calculateChargeLevel(double volts);
  int _avgAnalogRead();
  double _analogReadToVolts(int readValue);
};

#endif  // if BATTERY_PIN!=255

#endif
