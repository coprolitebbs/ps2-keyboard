# ps2-keyboard
Самодельная клавиатура с интерфейсом ps/2.
Собрана на Attiny2313A и сдвиговых регистрах 74HC595 и 74HC165.

Прошивка Attiny базируется на библиотеке [ps2dev](https://github.com/Harvie/ps2dev)

Простое описание данных для протокола [PS/2](https://marsohod.org/11-blog/57-ps2proto).

Прошивка компилируется средой Arduino IDE,

Прошивается с помощью progisp172 программатором через порт "РПрог" для программирования на плате клавиатуры,

Fuse-биты для Attiny2313A: 
Low: E2, High: DF, Ext: FF, Lock: FF
