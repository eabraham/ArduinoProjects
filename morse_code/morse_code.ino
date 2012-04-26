
int buzzPin=12;
int inByte = 0;         // incoming serial byte
String morseCodeArr[26]={"*-","-***","-*-*","-**","*","**-*","--*","****","**","*---","-*-","*-**","--","-*","---","*--*","--*-","*-*","***","-","**-","***-","*--","-**-","-*--","--**"};
String morseCode="";
int morsePnt=0;
int asciiStart=65;
const int dotDelay=300;
const int dashDelay=dotDelay*3;

void setup()
{
  Serial.begin(9600);
  pinMode(buzzPin, OUTPUT);
}

void loop()
{
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    inByte = Serial.read();
    if (inByte>=96 && inByte<=122)
      playMorse(inByte-32); //shift lower case to uppercase
    else if (inByte==32)
      space();
    else if (inByte>=65 && inByte<=91)
      playMorse(inByte);
    else
      Serial.println("Invalid Character");
      
    Serial.println(inByte);
  }
}

void playMorse(int asciiCode) {
    morsePnt=asciiCode-asciiStart;
    morseCode=morseCodeArr[morsePnt];
    Serial.println(morsePnt);
    Serial.println(morseCode);
    betLetters();
    for (int y=0;y<morseCode.length();y++){
      betPartLetter();
      if(morseCode.substring(y,y+1).equals("*")){
        Serial.println(morseCode.substring(y,y+1));
        dot();
      }
      else{
        Serial.println(morseCode.substring(y,y+1));
        dash();
      }
    }
}
//higher level helper
void space(){
  //space equ. to 7 times dot
  delay(dotDelay*7);
}
void betLetters(){
  //space between letters equ. to 3 times dot
  delay(dotDelay*3); 
}
void betPartLetter(){
  //space between letters equ. to 3 times dot
  delay(dotDelay); 
}


//low level
void dot(){
  digitalWrite(buzzPin,HIGH);
  delay(dotDelay);
  digitalWrite(buzzPin,LOW); 
}
void dash(){
  digitalWrite(buzzPin,HIGH); 
  delay(dashDelay);
  digitalWrite(buzzPin,LOW);
}
