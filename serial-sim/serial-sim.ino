void setup() {
  // put your setup code here, to run once:
  SerialUSB.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t data[10] = {0xc0, 0x00, 3, 4, 5, 6, 7, 8, 9, 0xc0};
  SerialUSB.write(data, 10);
  delay(5000);
}
