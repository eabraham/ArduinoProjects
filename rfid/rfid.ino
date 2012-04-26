int RFIDResetPin = 13;

//Register your RFID tags here
char* tags[] = {"4800E4E3602F","010230F28243"};

void setup(){
  Serial.begin(9600);

  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, HIGH);

  //ONLY NEEDED IF CONTROLING THESE PINS - EG. LEDs
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  
  lightLED(4);
  lightLED(3);
  lightLED(4);
}

void loop(){

  char tagString[13];
  int index = 0;
  boolean reading = false;

  while(Serial.available()){

    int readByte = Serial.read(); //read next available byte

    if(readByte == 2) reading = true; //begining of tag
    if(readByte == 3) reading = false; //end of tag

    if(reading && readByte != 2 && readByte != 10 && readByte != 13){
      //store the tag
      Serial.println(readByte);
      tagString[index] = readByte;
      index ++;
    }
  }

  //checkTag(tagString); //Check if it is a match
  //clearTag(tagString); //Clear the char of all value
  //resetReader(); //eset the RFID reader
}

void checkTag(char tag[]){
///////////////////////////////////
//Check the read tag against known tags
///////////////////////////////////
  boolean valid=false;
  
  if (sizeof(tag)==12){
    Serial.print("Tag Pos: ");
    Serial.println(tag);
    for (int i=0;i<sizeof(tags);i++){
      if(compareTag(tag, tags[i])){
        Serial.println(tag);
        valid=true;
      }
    }
    if (valid){
      lightLED(4);
    }else{
      Serial.println("Why RED!!!");
      lightLED(2);
    }
  }else{
    lightLED(3);
  }
  
}

void lightLED(int pin){
///////////////////////////////////
//Turn on LED on pin "pin" for 250ms
///////////////////////////////////
  Serial.println(pin);

  digitalWrite(pin, HIGH);
  delay(250);
  digitalWrite(pin, LOW);
}

void resetReader(){
///////////////////////////////////
//Reset the RFID reader to read again.
///////////////////////////////////
  digitalWrite(RFIDResetPin, LOW);
  digitalWrite(RFIDResetPin, HIGH);
  delay(150);
}

boolean compareTag(char one[], char two[]){
///////////////////////////////////
//compare two value to see if same,
//strcmp not working 100% so we do this
///////////////////////////////////

  if(strlen(one) == 0) return false; //empty

  for(int i = 0; i < 12; i++){
    if(one[i] != two[i]) return false;
  }

  return true; //no mismatches
}


