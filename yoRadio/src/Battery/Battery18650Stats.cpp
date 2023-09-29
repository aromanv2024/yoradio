#include "Battery18650Stats.h"

#if BATTERY_PIN!=255

const uint8_t unit1pins[] PROGMEM = { 36,37,38,39,32,33,34,35 };
adc1_channel_t unit1channels[] PROGMEM = { ADC1_CHANNEL_0,ADC1_CHANNEL_1,ADC1_CHANNEL_2,ADC1_CHANNEL_3,ADC1_CHANNEL_4,ADC1_CHANNEL_5,ADC1_CHANNEL_6,ADC1_CHANNEL_7 };

Battery::Battery() {
  
}

Battery::~Battery() {
  free(_conversionTable);
  delete this->_conversionTable;
}

void Battery::init(){
  if(!_validateAdcPin()) {
    Serial.printf("##[ERROR]#\tGPIO%d is not valid ADC1 pin\n", BATTERY_PIN);
    return;
  }
  adc1_config_width((adc_bits_width_t)ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(_channel, ADC_ATTEN_DB_11);
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, (adc_bits_width_t)ADC_WIDTH_BIT_12, 1100, &_adc_chars);
  getBatteryChargeLevel(true);
}

bool Battery::_validateAdcPin(){
  _validAdcPin = false;
  uint8_t len = sizeof(unit1pins)/sizeof(unit1pins[0]);
  for(uint8_t i=0; i<len; i++){
    if(BATTERY_PIN==unit1pins[i]){
      _channel=unit1channels[i];
      _validAdcPin = true;
      return _validAdcPin;
    }
  }
  return _validAdcPin;
}

double Battery::getBatteryVolts() {
  //int readValue = _avgAnalogRead();
  return _analogReadToVolts(_mv);
}

int Battery::getBatteryChargeLevel(bool useConversionTable) {
  _mv = _avgAnalogRead();
  double volts = _analogReadToVolts(_mv);

  if (volts >= 4.2) return 100;
  if (volts <= 3.2) return 0;

  return useConversionTable ? _getChargeLevelFromConversionTable(volts) : _calculateChargeLevel(volts);
}

void Battery::_initConversionTable() {
  _conversionTable = (double*)malloc(sizeof(double)*101);
  _conversionTable[0] = 3.200;
  _conversionTable[1] = 3.250;  _conversionTable[2] = 3.300; _conversionTable[3] = 3.350; _conversionTable[4] = 3.400; _conversionTable[5] = 3.450;
  _conversionTable[6] = 3.500; _conversionTable[7] = 3.550; _conversionTable[8] = 3.600; _conversionTable[9] = 3.650; _conversionTable[10] = 3.700;
  _conversionTable[11] = 3.703; _conversionTable[12] = 3.706; _conversionTable[13] = 3.710; _conversionTable[14] = 3.713; _conversionTable[15] = 3.716;
  _conversionTable[16] = 3.719; _conversionTable[17] = 3.723; _conversionTable[18] = 3.726; _conversionTable[19] = 3.729; _conversionTable[20] = 3.732;
  _conversionTable[21] = 3.735; _conversionTable[22] = 3.739; _conversionTable[23] = 3.742; _conversionTable[24] = 3.745; _conversionTable[25] = 3.748;
  _conversionTable[26] = 3.752; _conversionTable[27] = 3.755; _conversionTable[28] = 3.758; _conversionTable[29] = 3.761; _conversionTable[30] = 3.765;
  _conversionTable[31] = 3.768; _conversionTable[32] = 3.771; _conversionTable[33] = 3.774; _conversionTable[34] = 3.777; _conversionTable[35] = 3.781;
  _conversionTable[36] = 3.784; _conversionTable[37] = 3.787; _conversionTable[38] = 3.790; _conversionTable[39] = 3.794; _conversionTable[40] = 3.797;
  _conversionTable[41] = 3.800; _conversionTable[42] = 3.805; _conversionTable[43] = 3.811; _conversionTable[44] = 3.816; _conversionTable[45] = 3.821;
  _conversionTable[46] = 3.826; _conversionTable[47] = 3.832; _conversionTable[48] = 3.837; _conversionTable[49] = 3.842; _conversionTable[50] = 3.847;
  _conversionTable[51] = 3.853; _conversionTable[52] = 3.858; _conversionTable[53] = 3.863; _conversionTable[54] = 3.868; _conversionTable[55] = 3.874;
  _conversionTable[56] = 3.879; _conversionTable[57] = 3.884; _conversionTable[58] = 3.889; _conversionTable[59] = 3.895; _conversionTable[60] = 3.900;
  _conversionTable[61] = 3.906; _conversionTable[62] = 3.911; _conversionTable[63] = 3.917; _conversionTable[64] = 3.922; _conversionTable[65] = 3.928;
  _conversionTable[66] = 3.933; _conversionTable[67] = 3.939; _conversionTable[68] = 3.944; _conversionTable[69] = 3.950; _conversionTable[70] = 3.956;
  _conversionTable[71] = 3.961; _conversionTable[72] = 3.967; _conversionTable[73] = 3.972; _conversionTable[74] = 3.978; _conversionTable[75] = 3.983;
  _conversionTable[76] = 3.989; _conversionTable[77] = 3.994; _conversionTable[78] = 4.000; _conversionTable[79] = 4.008; _conversionTable[80] = 4.015;
  _conversionTable[81] = 4.023; _conversionTable[82] = 4.031; _conversionTable[83] = 4.038; _conversionTable[84] = 4.046; _conversionTable[85] = 4.054;
  _conversionTable[86] = 4.062; _conversionTable[87] = 4.069; _conversionTable[88] = 4.077; _conversionTable[89] = 4.085; _conversionTable[90] = 4.092;
  _conversionTable[91] = 4.100; _conversionTable[92] = 4.111; _conversionTable[93] = 4.122; _conversionTable[94] = 4.133; _conversionTable[95] = 4.144;
  _conversionTable[96] = 4.156; _conversionTable[97] = 4.167; _conversionTable[98] = 4.178; _conversionTable[99] = 4.189; _conversionTable[100] = 4.200;
}

int Battery::_avgAnalogRead() {
  if(!_validAdcPin) {
    Serial.printf("##[ERROR]#\tGPIO%d is not valid ADC1 pin\n", BATTERY_PIN);
    return 0;
  }
  int totalValue = 0;
  for (int i = 0; i < BATTERY_READS; i++) {
    totalValue += esp_adc_cal_raw_to_voltage(adc1_get_raw(_channel), &_adc_chars);
  }
  return (int) (totalValue / BATTERY_READS);
}

int Battery::_calculateChargeLevel(double volts) {
  if (volts <= 3.700) return (20 * volts) - 64;
  int chargeLevel = round((-233.82 * volts * volts) + (2021.3 * volts) - 4266);
  return ((volts > 3.755 && volts <= 3.870) || volts >= 3.940)
    ? chargeLevel + 1
    : chargeLevel;
}

int Battery::_getChargeLevelFromConversionTable(double volts) {
  if (_conversionTable == nullptr) _initConversionTable();
  int index = 50;
  int previousIndex = 0;
  int half = 0;
  while(previousIndex != index) {
    half = abs(index - previousIndex) / 2;
    previousIndex = index;
    if (_conversionTable[index] == volts) return index;
    index = (volts >= _conversionTable[index]) ? index + half : index - half;
  }
  return index;
}

double Battery::_analogReadToVolts(int readValue) {
  return readValue * BATTERY_CONVERSION_FACTOR / 1000;
}

#endif  // if BATTERY_PIN!=255
