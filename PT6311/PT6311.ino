#include <SPI.h>
#define STB 10
//ckl 13
//dio 11
//hardware sunt pini separati pentru DI si DO

void initializare() {
  pinMode(STB, OUTPUT);

  digitalWrite(STB, HIGH);
  ///////////////////////
  SPI.begin();
  SPI.setBitOrder(LSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  SPI.setDataMode(SPI_MODE3);
  ///////////////////////
  digitalWrite(STB, LOW);
  SPI.transfer(0b01000000);//cmd2
  digitalWrite(STB, HIGH);
  //dupa fiecare comanda trebuie STB high
  digitalWrite(STB, LOW);
  SPI.transfer(0b11000000);//cmd3
  for (volatile uint8_t i = 0; i < 13; ++i) {
    //toate segmentele stinse
    SPI.transfer(0);
    SPI.transfer(0);
    SPI.transfer(0);
  }
  digitalWrite(STB, HIGH);
  delayMicroseconds(25);

  digitalWrite(STB, LOW);
  SPI.transfer(0b00000000);//cmd1
  digitalWrite(STB, HIGH);
  digitalWrite(STB, LOW);
  SPI.transfer(0b10001000);//cmd4
  digitalWrite(STB, HIGH);
}

void send_digit(uint8_t digit_pos, uint32_t data) {
  digitalWrite(STB, LOW);
  SPI.transfer(0b01000000);//cmd2
  digitalWrite(STB, HIGH);
  digitalWrite(STB, LOW);
  SPI.transfer(0b11000000 | (digit_pos * 0x03));//in RAM prima adresa a unui digit sare din 0x03 in 0x03
  SPI.transfer((uint8_t)(data      ));//de la 0 la 7
  SPI.transfer((uint8_t)(data >>  8));//de la 8 la 16
  SPI.transfer((uint8_t)(data >> 16));//de la 15 la 19. total 20bit
  digitalWrite(STB, HIGH);
  //nu se pot scrie mai multi digiti intr-un singur transfer sarind adresele

}

void send_whole(uint32_t data[8]) {
  uint8_t i = 0;
  digitalWrite(STB, LOW);
  SPI.transfer(0b01000000);//cmd2
  digitalWrite(STB, HIGH);
  digitalWrite(STB, LOW);
  SPI.transfer(0b11000000 | (0x00));
  while (i < 8) {
    SPI.transfer((uint8_t)(data[i]      ));//de la 0 la 7
    SPI.transfer((uint8_t)(data[i] >>  8));//de la 8 la 16
    SPI.transfer((uint8_t)(data[i] >> 16));//de la 15 la 19. total 20bit
    //trebuie modificat daca nu se foloseste display 8/20
    i++;
  }
  digitalWrite(STB, HIGH);
  //se poate scrie secvential tot RAMul fara a trebui STB high dupa fiecare digit
}

void wr_led(uint8_t value) {
  digitalWrite(STB, LOW);
  SPI.transfer(0b01000001);
  SPI.transfer(0b00001111 & value);
  digitalWrite(STB, HIGH);
}

uint8_t rd_sw() {
  uint8_t state;
  digitalWrite(STB, LOW);
  SPI.transfer(0b01000011);
  state = SPI.transfer(0xFF);
  digitalWrite(STB, HIGH);
  return state;
}

uint8_t rd_key() {
  uint8_t state;
  digitalWrite(STB, LOW);
  SPI.transfer(0b01000010);
  state = SPI.transfer(0xFF) & 0x0F;
  digitalWrite(STB, HIGH);
  return state;
}



void setup() {
  initializare();
  uint32_t a[8] = {0x000C7C40, 0x00000000, 0x000C3C20, 0x000C7C80, 0x00020000, 0x00044460, 0x000C4460, 0x00029100};
  delay(50);
  wr_led(0b00000001);
  send_whole(a);
  delay(1000);
}

void loop() {
  //display-ul are dig1 seg1 in dreapta
  //display 8 digiti 20 segmente
  send_digit(0, 0x000C7C40 );//A
  send_digit(1, 0x00000000 );
  send_digit(2, 0x000C3C20 );//E
  send_digit(3, 0x000C7C80 );//R
  send_digit(4, 0x00020000 );//`
  send_digit(5, 0x00044460 );//U
  send_digit(6, 0x000C4460 );//O
  send_digit(7, 0x00029100 );//Y
  delay(500);

  send_digit(0, 0x000C3C20 );//E
  send_digit(1, 0x00011100 );//I
  send_digit(2, 0x000C7C00 );//P
  send_digit(3, 0x000C3C20 );//E
  send_digit(4, 0x00011100 );//I
  send_digit(5, 0x00091100 );//T
  send_digit(6, 0x00044460 );//U
  send_digit(7, 0x000C0420 );//C
  delay(500);
}
