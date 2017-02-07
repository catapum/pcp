
#define MAX7219_DIN 5
#define MAX7219_CS  3
#define MAX7219_CLK 2

#define trigPin 12
#define echoPin 13

const int wait = 100;


unsigned long timeElapsed = millis();
boolean foundHug = false;

void initialise()
{
  digitalWrite(MAX7219_CS, HIGH);
  pinMode(MAX7219_DIN, OUTPUT);
  pinMode(MAX7219_CS, OUTPUT);
  pinMode(MAX7219_CLK, OUTPUT);
}

void output(byte address, byte data)
{
  digitalWrite(MAX7219_CS, LOW);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, address);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, data);
  digitalWrite(MAX7219_CS, HIGH);
}

void setTestMode(boolean on)
{
  output(0x0f, on ? 0x01 : 0x00);
}

void setShutdown(boolean off)
{
  output(0x0c, off ? 0x00 : 0x01); //shutdown register - normal operation
}

void showDigits(byte numDigits)
{
  output(0x0b, numDigits-1); //scan limit register
}

void setBrightness(byte brightness)
{
  output(0x0a, brightness); //intensity register - max brightness
}

void putByte(byte data) {
  byte i = 8;
  byte mask;
  while(i > 0) {
    mask = 0x01 << (i - 1);           // get bitmask
    digitalWrite(MAX7219_CLK, LOW);   // tick
    if (data & mask){                 // choose bit
      digitalWrite(MAX7219_DIN, HIGH);// send 1
    }else{
      digitalWrite(MAX7219_DIN, LOW); // send 0
    }
    digitalWrite(MAX7219_CLK, HIGH);  // tock
    --i;                              // move to lesser bit
  }
}

void maxSingle(byte reg, byte col) {
  digitalWrite(MAX7219_CS, LOW);       // CS has to transition from LOW to HIGH    
  putByte(reg);                        // specify register
  putByte(col);                        // put data  
  digitalWrite(MAX7219_CS, LOW);       // Load by switching CS HIGH
  digitalWrite(MAX7219_CS, HIGH);
}

void write8x8(byte a, byte b, byte c, byte d, byte e, byte f, byte g, byte h){
   maxSingle(1,a);
   maxSingle(2,b);
   maxSingle(3,c);
   maxSingle(4,d);
   maxSingle(5,e);
   maxSingle(6,f);
   maxSingle(7,g);
   maxSingle(8,h);
  delay(wait);
}

void noHug(){
  write8x8(0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0);
}

void maxHug(){
  for(int i = 0; i<8; i++){
    write8x8(0x38,0x7c,0x7e,0x3f,0x3f,0x7e,0x7c,0x38);
    write8x8(0x38,0x44,0x42,0x21,0x21,0x42,0x44,0x38);
    write8x8(0xc7,0x83,0x81,0xc0,0xc0,0x81,0x83,0xc7);
    write8x8(0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0);
    delay(wait);
  }
  

}

void medHug(){
  for(int i = 0; i<6; i++){
    write8x8(0x38,0x7c,0x7e,0x3f,0x3f,0x7e,0x7c,0x38);
    delay(wait);
    write8x8(0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0);
    delay(wait);
  }
}

void minHug(){
  for(int i = 0; i<6; i++){
    write8x8(0x38,0x44,0x42,0x21,0x21,0x42,0x44,0x38);
    delay(wait);
    write8x8(0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0);
    delay(wait);
  }
}

int measureDistance(){
  long duration, distance;

  // Trigger the ultrasonic echo
  digitalWrite(trigPin, LOW); // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);

  // Calculate echo duration
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance based on echo
  distance = (duration/2) / 29.1;
  
  delay(200);

  return distance;
}

void loop() {
  
  int distance = measureDistance();
  if (distance <= 7 || distance >= 2500){
    //hug happening
    foundHug = true;
  }
  else if (foundHug){
    //hug finished
    timeElapsed = millis() - timeElapsed;
    Serial.print("Hug length in miliseconds: ");
    Serial.println(timeElapsed);
    
    int seconds = timeElapsed/1000;

    if (seconds < 2 ){
      minHug();
      
    }else if (seconds < 4){
      medHug();
    }else{
      maxHug();
    }

    timeElapsed = millis();
    foundHug = false;
    
  }
  else {
    //no hug detected
    noHug();
    Serial.print(distance);
    Serial.println(" cm, No hug");
    timeElapsed = millis();
  }
  
  
  
  
}

void setup() {
  //led matrix code
  initialise();
  setTestMode(false);
  setShutdown(false);
  setBrightness(1); // Brightness range 1..0x0f
  showDigits(8);    // Make sure all digits are visible
  output(0x09, 0);  // using an led matrix (not digits)

  //Proximity sensor code
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  
}
