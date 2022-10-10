/*
  Copyright (c) 2014-2015 NicoHood
  See the readme for credit to other people.

  Advanced RawHID example

  Shows how to send bytes via RawHID.
  Press a button to send some example values.

  Every received data is mirrored to the host via Serial.

  See HID Project documentation for more information.
  https://github.com/NicoHood/HID/wiki/RawHID-API
*/

#include "HID-Project.h"

const int pinLed = LED_BUILTIN;
const int pinButton = 4;

// Buffer to hold RawHID data.
// If host tries to send more data than this,
// it will respond with an error.
// If the data is not read until the host sends the next data
// it will also respond with an error and the data will be lost.
uint8_t rawhidData[255];
uint8_t bttn = 0;

void setup() {
  pinMode(pinLed, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);

  //Serial.begin(115200);

  // Set the RawHID OUT report array.
  // Feature reports are also (parallel) possible, see the other example for this.
  RawHID.begin(rawhidData, sizeof(rawhidData));
}

void loop() {
  if (!digitalRead(pinButton)){
    bttn = 1;
  }
  else {
    bttn = 0;
  }
//  Serial.print("bttn=");
//  Serial.println(bttn);
  // Check if there is new data from the RawHID device
  auto bytesAvailable = RawHID.available();
  if (bytesAvailable)
  {
    digitalWrite(pinLed, HIGH);

    // Mirror data via Serial
    uint8_t buffIn[64];
    uint8_t buffOut[64];
    int i = 0;
    while (bytesAvailable--) {
      buffIn[i] = RawHID.read();
      Serial.print(buffIn[i]);
      Serial.print(" ");
      i++;
    }
    i = 0;
    Serial.println();
    if ((buffIn[2] == 255) && (buffIn[3] != 13)){
       
     switch (buffIn[3]){
      case 1:
      //тип устройства. Отвечаем, что sensor cassy-2
        buffOut[0] = 3;
        buffOut[1] = 1;
        buffOut[2] = 3;//12 - mobile cassy, 29-pocket cassy, 32 - ваттметр 
        buffOut[3] = 4; // версия D
      break;
      case 2:
        // версия по. текущая версия 1.11
        buffOut[0] = 3;
        buffOut[1] = 1;
        buffOut[2] = 0;
        buffOut[3] = 111;
      break;
      case 7:
      //установка режима. Для sensor cassy устанавливаются параметры LED. отвечаем ок
        buffOut[0] = 2;
        buffOut[1] = 1;
        buffOut[2] = 1;
      break;
    case 16:
      //команда для sensor, mobile и pocket
      //опрашивает наличие сенсор боксов в разъеме А. Если есть, овтечаем номером устройства: 0-524030, 1- 524031
      buffOut[0] = 2;
      buffOut[1] = 1;
      buffOut[2] = 255;//ничего не подкдлючено, ответ -1
      break;
    case 17:
    //команда для sensor, mobile и pocket
    //опрашивает наличие сенсор боксов в разъеме B. Если есть, овтечаем номером устройства: 0-524030, 1- 524031
      buffOut[0] = 2;
      buffOut[1] = 1;
      buffOut[2] = 44;//код для Timer S
      break;
    case 28:
      //остановка запущенного осциллографа. Отвечаем ок
      buffOut[0] = 1;
      buffOut[1] = 1;
      break;
    case 34:
      //сброс таймеров на входах A и B. Отвечаем ок
      buffOut[0] = 1;
      buffOut[1] = 1;
      break;
    case 35:
      //сброс таймеров на входах A и B. Отвечаем ок
      buffOut[0] = 1;
      buffOut[1] = 1;
      break;
    case 126:
    //команда для sensor2
    //опрашивает наличие сенсор боксов в разъеме A. Если есть, овтечаем номером устройства: 0-524030, 1- 524031
      buffOut[0] = 2;
      buffOut[1] = 1;
      buffOut[2] = 255;//устройства нет
      break;
    case 127:
    //команда для sensor2
    //опрашивает наличие сенсор боксов в разъеме B. Если есть, овтечаем номером устройства: 0-524030, 1- 524031
      buffOut[0] = 2;
      buffOut[1] = 1;
      buffOut[2] = 44;//подключен Timer S
      break;
    case 33:
    //опрос уровня входного сигнала таймеров на входе B (для входа A команда №32). 1 бит - вход E, 2 бит - вход F
      buffOut[0] = 2;
      buffOut[1] = 1;
      buffOut[2] = bttn;
      break;
    case 40:
    //получение статуса таймера (A-№39, B - №40)
      buffOut[0] = 11;
      buffOut[1] = 1;//все ок
      buffOut[2] = 3;//режим recorder
      buffOut[3] = 0;//(0-оба запущены, 1 - E ост-н, 2 - F ост-н, 3 - оба остановлены, 4 - таймер выключен)
      buffOut[4] = 0;//биты отсчета времени
      buffOut[5] = 158;//биты отсчета времени
      buffOut[6] = 223;//биты отсчета времени
      buffOut[7] = 202;//биты отсчета времени
      buffOut[8] = 0;//число счетов в массиве входа E
      buffOut[9] = 0;//число счетов в массиве входа E
      buffOut[10] = 0;//число счетов в массиве входа F
      buffOut[11] = 0;//число счетов в массиве входа F
      break;
    default:
      buffOut[0] = 1;
      buffOut[1] = 1;
      break;      
    }
    RawHID.write(buffOut, sizeof(buffOut));
      for (uint8_t i = 0; i < sizeof(buffOut); i++) {
      Serial.print(buffOut[i]);
      buffOut[i] = 0;
      Serial.print(" ");
    }
    }
    Serial.println();
    digitalWrite(pinLed, LOW);
  }
}
