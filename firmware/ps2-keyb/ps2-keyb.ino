// https://github.com/Harvie/ps2dev/
// https://codeandlife.com/2013/06/28/minimal-ps2-keyboard-on-attiny2313/

#include <avr/io.h> //подключаем библиотеку аппаратных описаний
#include <util/delay.h> //поключаем библиотеку задержек

//#define KEYMAPSIZE 256

//Битовый массив нажатых клавиш. Если клавиша нажата, то в слове по номеру строки бит по номеру столбца будет установлен
word pr[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//Для использования 1 чипа 165 оставить значение 8
//Для двух чипов - 16
#define CHIP_J_COUNT 8

//Массив скан-кодов клавиш
const unsigned char keymap[] PROGMEM =
{
  0x76, 0x26, 0x4E, 0x2D, 0x5B, 0x33, 0x22, 0x4A,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x05, 0x25, 0x55, 0x2C, 0x5A, 0x3B, 0x21, 0x59,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x06, 0x2E, 0x5D, 0x35, 0x58, 0x42, 0x2A, 0x14,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x04, 0x36, 0x66, 0x3C, 0x1C, 0x4B, 0x32, 0x11,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x0C, 0x3D, 0x0D, 0x43, 0x1B, 0x4C, 0x31, 0x29,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x0E, 0x3E, 0x15, 0x44, 0x23, 0x41, 0x3A, 0xE0,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x16, 0x46, 0x1D, 0x4D, 0x2B, 0x12, 0x41, 0xE1,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x1E, 0x45, 0x24, 0x54, 0x34, 0x1A, 0x49, 0xE2,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,

  0x03, 0xE3, 0xE4, 0x29, 0x29, 0x29, 0x29, 0x29,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x0B, 0xE5, 0xE6, 0x29, 0x29, 0x29, 0x29, 0x29,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x83, 0xE7, 0xE8, 0x29, 0x29, 0x29, 0x29, 0x29,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x0A, 0xE9, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x01, 0xEA, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x09, 0xEB, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x78, 0xEC, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x07, 0xED, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,   0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29
};

//Длина максимальной последовательности расширенных кодов для клавиши
#define EXT_K_SIZE 8

//Массив клавиш с расширенными кодами.
//Если в массиве основных кодов выше указан код >= E0, то будет выбрана последовательность
//из данного массива с номером (Номер - E0), т.е., код E0 - первая, E1 - вторая, и т.д.
//Коды 0x00 будут игнорироваться
const unsigned char extcodes[]/*14][EXT_K_SIZE]*/ PROGMEM = {
  0xE0, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //R ALT  (E0)
  0xE0, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //R CTRL (E1)
  0xE1, 0x14, 0x77, 0xE1, 0xF0, 0x14, 0xE0, 0x77,   //PAUSE  (E2)
  0xE0, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //UP     (E3)
  0xE0, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //END    (E4)
  0xE0, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //DOWN   (E5)
  0xE0, 0x7D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //PG UP  (E6)
  0xE0, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //LEFT   (E7)
  0xE0, 0x12, 0xE0, 0x7C, 0x00, 0x00, 0x00, 0x00,   //PT SCR (E8)
  0xE0, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //RIGHT  (E9)
  0xE0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //INS    (EA)
  0xE0, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //HOME   (EB)
  0xE0, 0x7A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //PG DN  (EC)
  0xE0, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    //DEL    (ED)
};

//То же самое для кодов отпускания клавиш
const unsigned char extcanscodes[]/*14][EXT_K_SIZE]*/ PROGMEM = {
  0xE0, 0xF0, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00,   //R ALT  (E0)
  0xE0, 0xF0, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00,   //R CTRL (E1)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //PAUSE  (E2)
  0xE0, 0xF0, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00,   //UP     (E3)
  0xE0, 0xF0, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00,   //END    (E4)
  0xE0, 0xF0, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00,   //DOWN   (E5)
  0xE0, 0xF0, 0x7D, 0x00, 0x00, 0x00, 0x00, 0x00,   //PG UP  (E6)
  0xE0, 0xF0, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x00,   //LEFT   (E7)
  0xE0, 0xF0, 0x7C, 0xE0, 0xF0, 0x12, 0x00, 0x00,   //PT SCR (E8)
  0xE0, 0xF0, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00,   //RIGHT  (E9)
  0xE0, 0xF0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,   //INS    (EA)
  0xE0, 0xF0, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00,   //HOME   (EB)
  0xE0, 0xF0, 0x7A, 0x00, 0x00, 0x00, 0x00, 0x00,   //PG DN  (EC)
  0xE0, 0xF0, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00    //DEL    (ED)
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
#define CLK_FULL 10 // 40+40 us for 12.5 kHz clock
#define CLK_HALF 5
//Таймаут проверки состояния пинов PS/2 при чтении
#define TIMEOUT 50

//Ожидание в мс перед и после отправки байта в порт
#define BYTEWAIT 500


//Инициализация порта PS/2
void init_ps2() {

  DDRD = (1 << RDATA_PIN) | (1 << RCLOCK_PIN) | (1 << RLATCH_PIN) | (1 << LED1_PIN) | (1 << LED2_PIN);

  //Установим оба диода в LOW
  LED1_PORT &= ~(1 << LED1_PIN);
  LED2_PORT &= ~(1 << LED2_PIN);

  //установим выходы регистра 27HC165
  DDRB |= (1 << ICLOCK_PIN) | (1 << ILATCH_PIN);
  DDRB &= ~(1 << IDATA_PIN);

  //Установим выводы PS/2 CLOCK и DATA в HIGH
  clockHigh();
  dataHigh();


  //Отправим по PS/2 код 0xAA, означающий готовность клавиатуры к работе
  while (keyb_write(0xAA) != 0);
  //  keyb_write(0xAA);
  _delay_us(10);


  //return;
}


//Отдельная процедура - Сделать строб на выводе PS/2 CLOCK
void do_clock_lo_hi() {
  _delay_us(CLK_HALF);
  clockLow(); // start bit
  _delay_us(CLK_FULL);
  clockHigh();
  _delay_us(CLK_HALF);
  //return;
}

//Проверка состояния выводов PS/2 CLOCK и DATA
int keyb_check() {
  return (!readClockPin() || !readDataPin());
}

//Универсальный ответ клавиатуры - подтверждение об успешном приеме
void ack() {
  while (keyb_write(0xFA));
  //keyb_write(0xFA);
  //_delay_us(CLK_HALF);
}



//Считывание с регистра 74HC165
uint16_t shiftIn165()
{
  uint16_t value = 0;

  //Включим-выключим защелку
  ILATCH_PORT &= ~(1 << ILATCH_PIN); //LOW
  ILATCH_PORT |= (1 << ILATCH_PIN);  //HIGH

  //Считаем побитно содержимое регистра
  for (uint8_t i = 0; i < CHIP_J_COUNT; ++i)
  {
    ICLOCK_PORT &= ~(1 << ICLOCK_PIN); //LOW
    value |= bitRead(PINB, IDATA_PIN) << ((CHIP_J_COUNT - 1) - i); //digitalRead(IDATA) << (15 - i);
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

  // parity bit
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
  _delay_us(BYTEWAIT);

  unsigned char val;
  unsigned char received_parity = 1;

  if (/*digitalRead(ps2clock) == LOW*/ !readClockPin()) {
    return -1;
  }

  if (/*digitalRead(ps2data) == LOW*/ !readDataPin()) {
    return -2;
  }

  dataLow();

  do_clock_lo_hi();

  for (val = 0; val < 8; val++)
  {
    if (data & 0x01)
    {
      dataHigh();
    } else {
      dataLow();
    }

    do_clock_lo_hi();

    received_parity = (!(received_parity % 2)); //received_parity ^ (data & 0x01);
    data = data >> 1;
  }
  // parity bit
  if (received_parity)
  {
    dataHigh();
  } else {
    dataLow();
  }
  do_clock_lo_hi();

  // stop bit
  dataHigh();
  do_clock_lo_hi();

  _delay_us(BYTEWAIT);

  return 0;
}

// грязный хак для выполнения ресета контроллера в функции вызова ресета клавиатуры
void (*reset)(void) = (void*)0;




int keyboard_reply(unsigned char cmd, unsigned char *leds) {

  unsigned char val;

// ряд не особо нужных кодов закомментирован и не используется
// в следствие нехватки памяти для компилированного кода.
// Параметры таймингов клавиатуры приходится изменять буквально
// перекомпиляцие и перепрошивкой, увы.

  switch (cmd) {
    case 0xFF: //reset
      ack();
      reset();
      break; //Не знаю, как тут изящно завершить
    case 0xFE: //resend
    //      ack();
    //      break;
    //case 0xFA: //Set all keys to typematic/autorepeat
    //      ack();
    //      break;
    //case 0xF9: //Set all keys to make only
    //      ack();
    //      break;
    //case 0xF8:
    //case 0xF7:
    //case 0xF6: //set defaults
    //      ack();
    //      break;
    //case 0xF5: //disable data reporting
    //FM
    //enabled = 0;
    //      ack();
    //      break;
    //case 0xF4: //enable data reporting
    //FM
    //enabled = 1;
    //ack();
    //break;
    case 0xF3: //set typematic rate
      ack();
      /*if (!keyboard_read(&val)) {
        ack(); //do nothing with the rate
        } else {
        if(bitRead(val,5)==1){
          if(bitRead(val,6)==1){
            waittime = 1000;
          } else {
            waittime = 750;
          }
        } else {
          if(bitRead(val,6)==1){
            waittime = 500;
          } else {
            waittime = 250;
          }
        }
        val = (val << 4) >> 4;

        }*/
      break;
    case 0xF2: //Сообщить device id
      ack();
      //0xAB83 - идентификатор оборудования стандартной клавиатуры ps/2
      while (keyb_write(0xAB) != 0);

      //_delay_us(CLK_HALF);
      while (keyb_write(0x83) != 0);
      //digitalWrite(LED1, HIGH);
      //_delay_us(80);
      //digitalWrite(LED1, LOW);
      break;
    case 0xF0: //set scan code set
      ack();
      if (!keyboard_read(&val)) ack(); //do nothing with the rate
      break;
    case 0xEE: //echo
      //ack();
      keyb_write(0xEE);
      break;
    case 0xED: //set/reset LEDs
      ack();
      if (!keyboard_read(leds)) ack(); //do nothing with the rate
      return 1;
      break;
      /* default:
         LED2_PORT |= (1 << LED2_PIN);
         _delay_us(100);
         LED2_PORT &= ~(1 << LED2_PIN);*/
  }
  return 0;
}




int keyboard_read_check(unsigned char *leds) {
  unsigned char c;

  if ( keyb_check() ) {
    if (!keyboard_read(&c)) return keyboard_reply(c, leds);
  }
  return 0;
}


int main() {
  //Установим выходы регистра 27HC595 и диодов
  //DDRD = (1 << RDATA_PIN) | (1 << RCLOCK_PIN) | (1 << RLATCH_PIN) | (1 << LED1_PIN) | (1 << LED2_PIN);

  //Установим оба диода в LOW
  //LED1_PORT &= ~(1 << LED1_PIN);
  //LED2_PORT &= ~(1 << LED2_PIN);
  //bool mc = 1;
  unsigned char km;
  //_delay_us(1000);
  init_ps2();
  //Инициализируем пины PS/2
  

  //установим выходы регистра 27HC165
  //DDRB |= (1 << ICLOCK_PIN) | (1 << ILATCH_PIN);
  //DDRB &= ~(1 << IDATA_PIN);


  //Главный бесконечный цикл
  while (1) {

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

    //Цикл опроса регистра 595
    for (byte i = 0; i < 16; i++) {
      //сформируем адрес строки опроса клавиш в 16битном сдвиговом регистре
      uint16_t ww = (uint16_t)1 << i;
      //И запишем его в порт 595
      RLATCH_PORT &= ~(1 << RLATCH_PIN);  //LOW
      shiftOut(RDATA, RCLOCK, MSBFIRST, highByte(ww));
      shiftOut(RDATA, RCLOCK, MSBFIRST, lowByte(ww));
      RLATCH_PORT |= (1 << RLATCH_PIN);  //HIGH

      //Считаем значение с регистра 165
      uint16_t b = shiftIn165();

      //Проверяем по строкам от 0 до 7
      for (byte j = 0; j < CHIP_J_COUNT; j++) {
        if (bitRead(b, j) == 1) {  // Проверяем на нажатие
          if (bitRead(pr[i], j) == 0) { //Если клавиша еще не нажата, то отправим ее код
            //LED1_PORT |= (1 << LED1_PIN);  //HIGH
            km = pgm_read_byte(&keymap[j + 16 * i]);
            //Проверим, не должна ли клавиша сообщать расширенный код
            if (km >= 0xE0) {
              //Если код из таблицы кодов >= E0, то читаем последовательность из массива
              //с номером (Код - E0)
              for (int k = 0; k < EXT_K_SIZE; k++) {
                unsigned char ekm = pgm_read_byte(&extcodes[k + EXT_K_SIZE * (km - 0xE0)]);
                if (ekm > 0x00) {
                  keyb_write(ekm);
                }
              }
            } else {
              //Если не расширенный код, отправляем просто код клавиши
              keyb_write(km);
            }
            bitWrite(pr[i], j, 1);
            LED2_PORT |= (1 << LED2_PIN);
          }
        } else {  //Проверяем на отпускание
          if (bitRead(pr[i], j) == 1) { //Если клавиша была нажата, то, отправим код отпускания
            km = pgm_read_byte(&keymap[j + 16 * i]);
            if (km >= 0xE0) { // По вышеописанному принципу - если код клавиши должен быть расширенным,
              //то выдаем расширенную последовательность из массива extcanscodes
              for (int k = 0; k < EXT_K_SIZE; k++) {
                unsigned char ekm = pgm_read_byte(&extcanscodes[k + EXT_K_SIZE * (km - 0xE0)]);
                if (ekm > 0x00) {
                  keyb_write(ekm);
                  //LED2_PORT &= ~(1 << LED2_PIN);
                }
              }
            } else { // Или выдаем просто один код из массива keymap
              keyb_write(0xF0);
              keyb_write(km);
            }
            bitWrite(pr[i], j, 0);
            LED2_PORT &= ~(1 << LED2_PIN);
          }

        }


      }  //j

    }

    
    
  }  //конец главного цикла

  return 1;
}
