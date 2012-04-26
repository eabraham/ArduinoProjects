void setup()
{
  Serial.begin(9600);
}

void loop()
{
    Serial.print("Input: ");
    Serial.print(millis());
    Serial.print(" : ");
    Serial.println(analogRead(A5));
    delay(300);
}
