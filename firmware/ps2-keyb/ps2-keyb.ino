// https://github.com/Harvie/ps2dev/
// https://codeandlife.com/2013/06/28/minimal-ps2-keyboard-on-attiny2313/

#include <avr/io.h> //подключаем библиотеку аппаратных описаний
//#define F_CPU 8000000UL //выставляем частоту МК
#include <util/delay.h> //поключаем библиотеку задержек

#define KEYMAPSIZE 256

//Битовый массив нажатых клавиш. Если клавиша нажата, то в слове по номеру строки бит по номеру столбца будет установлен
word pr[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//Массив скан-кодов клавиш
const unsigned char keymap[] PROGMEM =
{
  0x41, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x31, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,

  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29
};

//Зададим пины светодиода Св1
#define LED1 7
#define LED1_PORT PORTD
#define LED1_PIN 5

//Зададим пины светодиода Св2
#define LED2 8
#define LED2_PORT PORTD
#define LED2_PIN 6

//Задаем пины для управления PS/2
#define PS2_CLOCK_DDR DDRB
#define PS2_CLOCK_PORT PORTB
#define PS2_CLOCK_PIN 0
#define PS2_CLOCK_INPUT PINB
#define ps2clock 9

#define PS2_DATA_DDR DDRB
#define PS2_DATA_PORT PORTB
#define PS2_DATA_PIN 1
#define PS2_DATA_INPUT PINB
#define ps2data 10

//Задаем пины 74HC595
#define RPORT PORTD
#define RDATA_PORT PORTD
#define RDATA_PIN 0
#define RCLOCK_PORT PORTD
#define RCLOCK_PIN 1
#define RLATCH_PORT PORTD
#define RLATCH_PIN 4
#define RDATA 0
#define RCLOCK 1
#define RLATCH 6

//Задаем пины 74HC165
#define IPORT PORTB
#define IDATA_DDR DDRB
#define IDATA_PORT PORTB
#define IDATA_PIN 4
#define ICLOCK_PORT PORTB
#define ICLOCK_PIN 2
#define ILATCH_PORT PORTB
#define ILATCH_PIN 3
#define IDATA 13
#define ICLOCK 11
#define ILATCH 12

//байт состояния светодиодов
unsigned char leds;

//Установить вывод PS/2 CLOCK в HIGH
static void clockHigh(void) {
  PS2_CLOCK_DDR &= ~_BV(PS2_CLOCK_PIN); // set as input
  PS2_CLOCK_PORT |= _BV(PS2_CLOCK_PIN); // set pullup
}

//Установить вывод PS/2 CLOCK в LOW
static void clockLow(void) {
  PS2_CLOCK_PORT &= ~_BV(PS2_CLOCK_PIN); // zero output value
  PS2_CLOCK_DDR |= _BV(PS2_CLOCK_PIN); // set as output
}

//Установить вывод PS/2 DATA в HIGH
static void dataHigh(void) {
  PS2_DATA_DDR &= ~_BV(PS2_DATA_PIN); // set as input
  PS2_DATA_PORT |= _BV(PS2_DATA_PIN); // set pullup
}

//Установить вывод PS/2 DATA в LOW
static void dataLow(void) {
  PS2_DATA_PORT &= ~_BV(PS2_DATA_PIN); // zero output value
  PS2_DATA_DDR |= _BV(PS2_DATA_PIN); // set as output
}

//Ускоренные процедуры чтения пинов PS/2
#define readClockPin() (PS2_CLOCK_INPUT & (1 << PS2_CLOCK_PIN))
#define readDataPin() (PS2_DATA_INPUT & (1 << PS2_DATA_PIN))
#define readReg165Pin() (PINB & (1 << IDATA_PIN))

//Тайминги ожидания при установке выводов PS/2
#define CLK_FULL 40 // 40+40 us for 12.5 kHz clock
#define CLK_HALF 20
//Таймаут ожидания перед и после процедуры чтения с порта PS/2
#define BYTEWAIT 1000
//Таймаут проверки состояния пинов PS/2 при чтении
#define TIMEOUT 30

//Инициализация порта PS/2
void init_ps2() {
  //Установим выводы PS/2 CLOCK и DATA в HIGH
  clockHigh();
  dataHigh();

  //Отправим по PS/2 код 0xAA, означающий готовность клавиатуры к работе
  /*ps2write*/keyb_write(0xAA);
  _delay_us(10);
}


//Отдельная процедура - Сделать строб на выводе PS/2 CLOCK
void do_clock_lo_hi() {
  _delay_us(CLK_HALF);
  clockLow(); // start bit
  _delay_us(CLK_FULL);
  clockHigh();
  _delay_us(CLK_HALF);
}

/*int keyb_available() {
  return (  (digitalRead(ps2data) == LOW) || (digitalRead(ps2clock) == LOW)  );
  }*/

//Проверка состояния выводов PS/2 CLOCK и DATA
int keyb_check() {
  return (!readClockPin() || !readDataPin());
}

//Универсальный ответ клавиатуры - подтверждение об успешном приеме
void ack() {
  /*ps2write*/keyb_write(0xFA);
  //_delay_us(CLK_HALF);
}


/*
//Запись байта в PS/2
int ps2write(unsigned char data) {
  unsigned char i;
  unsigned char parity = 0;
  //keyb_check()
  if (!readClockPin() || !readDataPin())  return -1; // PS/2 CLOCK или DATA LOW

  //Вывод PS/2 DATA в LOW
  dataLow();
  //Сделать строб по выводу PS/2 CLOCK
  do_clock_lo_hi();

  //Последовательно записать биты из байта data, одновременно считая четность
  for (i = 0; i < 8; i++) {
    if (data & 1) {
      dataHigh();
      parity++;
    } else
      dataLow();

    do_clock_lo_hi();

    data = data >> 1;
  }

  //Согласно посчитанной четности выставить на выводе PS/2 DATA состояние LOW или HIGH
  if (parity & 1)
    dataLow();
  else
    dataHigh();

  //Сделать строб по выводу PS/2 CLOCK
  do_clock_lo_hi();

  // Послать последний стоповый бит
  dataHigh();
  do_clock_lo_hi();

  //Немного подождать
  _delay_us(CLK_FULL);

  return 0;
}
*/

//Считывание с регистра 74HC165
uint16_t shiftIn165()
{
  uint16_t value = 0;

  //Включим-выключим защелку
  ILATCH_PORT &= ~(1 << ILATCH_PIN); //LOW
  ILATCH_PORT |= (1 << ILATCH_PIN);  //HIGH

  //Считаем побитно содержимое регистра
  for (uint8_t i = 0; i < 16; ++i)
  {
    ICLOCK_PORT &= ~(1 << ICLOCK_PIN); //LOW
    value |= bitRead(PINB, IDATA_PIN) << (15 - i); //digitalRead(IDATA) << (15 - i);
    ICLOCK_PORT |= (1 << ICLOCK_PIN);  //HIGH
  }

  return value;
}

//Функция чтения данных с порта PS/2
int keyboard_read(unsigned char * value) {
  unsigned int data = 0x00;
  unsigned int bit = 0x01;

  unsigned char calculated_parity = 1;
  unsigned char received_parity = 0;

  unsigned long waiting_since = millis();
  while ((readDataPin()) || (!readClockPin()) /*(digitalRead(ps2data) != LOW) || (digitalRead(ps2clock) != HIGH)*/ ) {
    if ((millis() - waiting_since) > TIMEOUT) return -1;
  }

  do_clock_lo_hi();

  while (bit < 0x0100) {
    if (readDataPin()/*digitalRead(ps2data) == HIGH*/) {
      data = data | bit;
      calculated_parity = calculated_parity ^ 1;
    } else {
      calculated_parity = calculated_parity ^ 0;
    }
    bit = bit << 1;
    do_clock_lo_hi();
  }

  if (readDataPin()/*digitalRead(ps2data) == HIGH*/) {
    received_parity = 1;
  }

  do_clock_lo_hi();

  _delay_us(CLK_HALF);
  dataLow();
  clockLow();
  _delay_us(CLK_FULL);
  clockHigh();
  _delay_us(CLK_HALF);
  dataHigh();

  *value = data & 0x00FF;

  if (received_parity == calculated_parity) {
    return 0;
  } else {
    return -2;
  }
}



//Запись байта в PS/2
int keyb_write(unsigned char data)
{
  _delay_us(1000);

  unsigned char i;
  unsigned char parity = 1;

  if (/*digitalRead(ps2clock) == LOW*/!readClockPin()) {
    return -1;
  }

  if (/*digitalRead(ps2data) == LOW*/!readDataPin()) {
    return -2;
  }

  dataLow();

  do_clock_lo_hi();

  for (i = 0; i < 8; i++)
  {
    if (data & 0x01)
    {
      dataHigh();
    } else {
      dataLow();
    }

    do_clock_lo_hi();

    parity = parity ^ (data & 0x01);
    data = data >> 1;
  }
  // parity bit
  if (parity)
  {
    dataHigh();
  } else {
    dataLow();
  }
  do_clock_lo_hi();

  // stop bit
  dataHigh();
  do_clock_lo_hi();

  _delay_us(1000);

  return 0;
}



int keyboard_reply(unsigned char cmd, unsigned char *leds) {

  unsigned char val;
  //unsigned char enabled;

  switch (cmd) {
    case 0xFF: //reset
      ack();
      //the while loop lets us wait for the host to be ready
      /*ps2write*/keyb_write(0xAA);
      break;
    case 0xFE: //resend
      ack();
      break;
    case 0xF6: //set defaults
      //enter stream mode
      ack();
      break;
    case 0xF5: //disable data reporting
      //FM
      //enabled = 0;
      ack();
      break;
    case 0xF4: //enable data reporting
      //FM
      //enabled = 1;
      ack();
      break;
    case 0xF2: //Сообщить device id
      ack();
      //0xAB83 - идентификатор оборудования стандартной клавиатуры ps/2
      /*ps2write*/keyb_write(0xAB);
      //_delay_us(CLK_HALF);
      /*ps2write*/keyb_write(0x83);
      /*digitalWrite(LED1, HIGH);
        _delay_us(80);
        digitalWrite(LED1, LOW);*/
      break;
    case 0xF0: //set scan code set
      ack();
      if (!keyboard_read(&val)) ack(); //do nothing with the rate
      break;
    case 0xEE: //echo
      //ack();
      /*ps2write*/keyb_write(0xEE);
      break;
    case 0xED: //set/reset LEDs
      ack();
      if (!keyboard_read(leds)) ack(); //do nothing with the rate
      return 1;
      break;
  }
  return 0;
}


int keyboard_read_check(unsigned char *leds) {
  unsigned char c;
  if (keyb_check()/*keyb_available()*/) {
    if (!keyboard_read(&c)) return keyboard_reply(c, leds);
  }
  return 0;
}


int main() {
  //Установим выходы регистра 27HC595 и диодов
  DDRD = (1 << RDATA_PIN) | (1 << RCLOCK_PIN) | (1 << RLATCH_PIN) | (1 << LED1_PIN) | (1 << LED2_PIN);

  //Установим оба диода в LOW
  LED1_PORT &= ~(1 << LED1_PIN);
  LED2_PORT &= ~(1 << LED2_PIN);

  //Инициализируем пины PS/2
  init_ps2();

  //установим выходы регистра 27HC165
  DDRB |= (1 << ICLOCK_PIN) | (1 << ILATCH_PIN);
  DDRB &= ~(1 << IDATA_PIN);

  //Главный бесконечный цикл
  while (1) {
    //Цикл опроса регистра 595
    for (byte i = 0; i < 16; i++) {
      //сформируем адрес строки опроса клавиш в 16битном сдвиговом регистре
      word ww = (word)1 << i;
      //И запишем его в порт 595
      RLATCH_PORT &= ~(1 << RLATCH_PIN);  //LOW
      shiftOut(RDATA, RCLOCK, MSBFIRST, highByte(ww));
      shiftOut(RDATA, RCLOCK, MSBFIRST, lowByte(ww));
      RLATCH_PORT |= (1 << RLATCH_PIN);  //HIGH

      //Считаем значение с регистра 165
      word b = shiftIn165();

      //Проверяем по строкам от 0 до 7
      //digitalWrite(PD2, LOW);
      for (byte j = 0; j < 16; j++) {
        //byte j = 0;
        if ((i == 0) && (j == 8)) {/// temporary check
          if (bitRead(b, j) == 1) {
            if (bitRead(pr[i], j) == 0) {
              //LED1_PORT |= (1 << LED1_PIN);  //HIGH
              unsigned char km = pgm_read_byte(&keymap[j + KEYMAPSIZE * i]);
              //digitalWrite(LED1, HIGH);
              /*ps2write*/keyb_write(km);
              bitWrite(pr[i], j, 1);
              _delay_ms(3);
            }
          } else {
            if (bitRead(pr[i], j) == 1) {
              /*ps2write*/keyb_write(0xF0);
              _delay_ms(3);
              /*ps2write*/keyb_write(pgm_read_byte(&keymap[j + KEYMAPSIZE * i]));
              bitWrite(pr[i], j, 0);
            }

          }

        } else {/// temporary check
          if (bitRead(b, j) == 1) {

          } else {

          }
          //digitalWrite(LED1, LOW);
          //LED1_PORT &= ~(1 << LED1_PIN);
        }
      }  //j
      //_delay_ms(50);
    }

    //Опрос входящих данных с порта PS/2 и проверка байта состояния светодиодов
    if (keyboard_read_check(&leds)) {
      //Если СAPS включен, зажжем светодиод Св1
      //digitalWrite(LED1, leds);
      if (bitRead(leds, 2) == 1) {
        //digitalWrite(LED1, HIGH);
        LED1_PORT |= (1 << LED1_PIN);
      } else {
        //digitalWrite(LED1, LOW);
        LED1_PORT &= ~(1 << LED1_PIN);
      }
    }


  }  //конец главного цикла

  return 1;
}
