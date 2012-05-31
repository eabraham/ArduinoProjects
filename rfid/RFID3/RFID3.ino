// RFID reader ID-12 for Arduino 
// Based on code by BARRAGAN <http://people.interaction-ivrea.it/h.barragan> 
// and code from HC Gilje - http://hcgilje.wordpress.com/resources/rfid_id12_tagreader/
// Modified for Arudino by djmatic
// Modified for ID-12 and checksum by Martijn The - http://www.martijnthe.nl/
//
// Use the drawings from HC Gilje to wire up the ID-12.
// Remark: disconnect the rx serial wire to the ID-12 when uploading the sketch

#include <EEPROM.h>

byte master[6]={0x00,0x00,0x00,0x00,0x00,0x00}; //remove master key for security
byte storedCard[6]; // Stores an ID read from EEPROM
byte readCard[6]; // Sotres an ID read from the RFID reader
  
void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);     
  pinMode(8, OUTPUT);
  
  lightLED(4);
  lightLED(3);
  lightLED(2);

  Serial.begin(9600);                                 // connect to the serial port
}

void loop () {
  byte i = 0;
  byte val = 0;
  byte code[6];
  byte checksum = 0;
  byte bytesread = 0;
  byte tempbyte = 0;


  if(Serial.available() > 0) {
    if((val = Serial.read()) == 2) {                  // check for header 
      bytesread = 0; 
      while (bytesread < 12) {                        // read 10 digit code + 2 digit checksum
        if( Serial.available() > 0) { 
          val = Serial.read();
          Serial.println(val);
          if((val == 0x0D)||(val == 0x0A)||(val == 0x03)||(val == 0x02)) { // if header or stop bytes before the 10 digit reading 
            break;                                    // stop reading
          }

          // Do Ascii/Hex conversion:
          if ((val >= '0') && (val <= '9')) {
            val = val - '0';
          } else if ((val >= 'A') && (val <= 'F')) {
            val = 10 + val - 'A';
          }

          // Every two hex-digits, add byte to code:
          if (bytesread & 1 == 1) {
            // make some space for this hex-digit by
            // shifting the previous hex-digit with 4 bits to the left:
            code[bytesread >> 1] = (val | (tempbyte << 4));

            if (bytesread >> 1 != 5) {                // If we're at the checksum byte,
              checksum ^= code[bytesread >> 1];       // Calculate the checksum... (XOR)
            };
          } else {
            tempbyte = val;                           // Store the first hex digit first...
          };

          bytesread++;                                // ready to read next digit
        } 
      } 

      // Output to Serial:

      if (bytesread == 12) {                          // if 12 digit read is complete
        Serial.print("5-byte code: ");
        for (i=0; i<5; i++) {
          if (code[i] < 16) Serial.print("0");
          Serial.print(code[i], HEX);
          Serial.print(" ");
        }
        Serial.println();

        Serial.print("Checksum: ");
        Serial.print(code[5], HEX);
        Serial.println(code[5] == checksum ? " -- passed." : " -- error.");
        Serial.println();
        Serial.print(code[5],DEC);
        //Card Present 
        goBuzz(5);
        //check for correct card
        if (checkMaster(code,master)==1){
          openDoor(8);
        }else{
          wrongKey();
        }
      }
      bytesread = 0;
    }
  }else
    lightLED(3);
}
boolean checkMaster(byte* code, byte* mast){
  boolean check=false;

  for (int x=0;x<5;x++){
      Serial.print("Master: ");
      Serial.println(mast[x]);
      Serial.print("Code: ");      
      Serial.println(code[x]);
   if (code[x]==mast[x])
     check=true;
   else{
     check=false;
     break;
   }
  }
  Serial.println(check);
  return check;
}
void lightLED(int pin){
///////////////////////////////////
//Turn on LED on pin "pin" for 250ms
///////////////////////////////////


  digitalWrite(pin, HIGH);
  delay(250);
  digitalWrite(pin, LOW);
}
void goBuzz(int pin){
///////////////////////////////////
//Turn on LED on pin "pin" for 250ms
///////////////////////////////////

  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
}
void wrongKey(){
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
}
void openDoor(int pin){
  digitalWrite(4, HIGH);
  digitalWrite(pin,HIGH);
  delay(5000);
  digitalWrite(pin,LOW);
  digitalWrite(4, LOW);
}
// Write an array to the EEPROM in the next available slot
void writeID( byte a[] )
{
  if ( !findID( a ) ) // Before we write to the EEPROM, check to see if we have seen this card before!
  {
  int num = EEPROM.read(0); // Get the numer of used spaces, position 0 stores the number of ID cards
  
  Serial.print("Num: ");
  Serial.print(num);
  Serial.print(" \n");
  int start = ( num * 5 ) + 1; // Figure out where the next slot starts
  num++; // Increment the counter by one
  EEPROM.write( 0, num ); // Write the new count to the counter
  for ( int j = 0; j < 5; j++ ) // Loop 5 times
  {
  EEPROM.write( start+j, a[j] ); // Write the array values to EEPROM in the right position
  Serial.print("W[");
  Serial.print(start+j);
  Serial.print("] Value [");
  Serial.print(a[j], HEX);
  Serial.print("] \n");
  
  }
    successWrite();
  }
  else
  {
    failedWrite();
  }
}
void readID( int number ) // Number = position in EEPROM to get the 5 Bytes from
{
  int start = (number * 5 ) - 4; // Figure out starting position
  //Serial.print("Start: ");
  //Serial.print(start);
  //Serial.print("\n\n");
  for ( int i = 0; i < 5; i++ ) // Loop 5 times to get the 5 Bytes
  {
    storedCard[i] = EEPROM.read(start+i); // Assign values read from EEPROM to array
    /*
    Serial.print("Read [");
    Serial.print(start+i);
    Serial.print("] [");
    Serial.print(storedCard[i], HEX);
    Serial.print("] \n");
    */
  }
}
// Find the slot number of the id to be deleted
int findIDSLOT( byte f[] )
{
  int count = EEPROM.read(0); // Read the first Byte of EEPROM that
  // Serial.print("Count: "); // stores the number of ID's in EEPROM
  // Serial.print(count);
  //Serial.print("\n");
  for ( int i = 1; i <= count; i++ ) // Loop once for each EEPROM entry
  {
    readID(i); // Read an ID from EEPROM, it is stored in storedCard[6]
    if( checkTwo( f, storedCard ) ) // Check to see if the storedCard read from EEPROM
    { 
      return i; // The slot number of the card
      break; // Stop looking we found it
    }
  }
}
boolean checkTwo ( byte a[], byte b[] )
{
  boolean match;
  if ( a[0] != NULL ) // Make sure there is something in the array first
  match = true; // Assume they match at first
  
  for ( int k = 0; k < 5; k++ ) // Loop 5 times
  {
    if ( a[k] != b[k] ) // IF a != b then set match = false, one fails, all fail
      match = false;
  }
  
  return match;
}
boolean findID( byte find[] )
{
  int count = EEPROM.read(0); // Read the first Byte of EEPROM that
  // Serial.print("Count: "); // stores the number of ID's in EEPROM
  // Serial.print(count);
  //Serial.print("\n");
  for ( int i = 1; i <= count; i++ ) // Loop once for each EEPROM entry
  {
    readID(i); // Read an ID from EEPROM, it is stored in storedCard[6]
    if( checkTwo( find, storedCard ) ) // Check to see if the storedCard read from EEPROM
    { // is the same as the find[] ID card passed
      //Serial.print("We have a matched card!!! \n");
      return true;
      break; // Stop looking we found it
    }
    else // If not, return false
    {
    //Serial.print("No Match here.... \n");
    }
  
  }
  return false;
}
void successWrite()
{
  digitalWrite(2, LOW); // Make sure green LED is on
  delay(200);
  digitalWrite(2, HIGH); // Make sure green LED is on
  delay(200);
  digitalWrite(2, LOW); // Make sure green LED is off
  delay(200);
  digitalWrite(2, HIGH); // Make sure green LED is on
  delay(200);
  digitalWrite(2, LOW); // Make sure green LED is off
  delay(200);
  digitalWrite(2, HIGH); // Make sure green LED is on
  delay(200);
}
void failedWrite()
{
  digitalWrite(4, LOW);
  delay(200);
  digitalWrite(4, HIGH);
  delay(200);
  digitalWrite(4, LOW);
  delay(200);
  digitalWrite(4, HIGH);
  delay(200);
  digitalWrite(4, LOW); 
  delay(200);
  digitalWrite(4, HIGH);
  delay(200);
}
