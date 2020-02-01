#include<Wire.h>
#include<LiquidCrystal_I2C.h>
int counter=0;
LiquidCrystal_I2C lcd(0x27,20,4);   //lcd.setCursor(col, row)
int s1=0,s2=0,s3=0,s4=0;

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
lcd.init();
lcd.backlight();
pinMode(2, INPUT);
pinMode(3, INPUT);
pinMode(4, INPUT);
pinMode(5, INPUT);
pinMode(6, OUTPUT);
pinMode(7, OUTPUT);
pinMode(8, OUTPUT);
pinMode(9, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(2)==HIGH && s1==0 )
  {
    counter++;
    s1=1;
    digitalWrite(6, HIGH);
    
  }
  else if(digitalRead(2)==LOW && s1==1)
  {
    counter--;
    s1=0;
    
    digitalWrite(6, LOW);
  }
  if(digitalRead(3)==HIGH && s2==0)
  {
    counter++;
    s2=1;
    digitalWrite(7, HIGH);
  }
  else if(digitalRead(3)==LOW && s2==1)
  {
      counter--;
      s2=0;
      digitalWrite(7, LOW);
  }
  if(digitalRead(4)==HIGH && s3==0)
  {
    counter++;
    s3=1;
    digitalWrite(8, HIGH);
  }
  else if(digitalRead(4)==LOW && s3==1)
  {
    counter--;
    s3=0;
    digitalWrite(8, LOW);
  }
  if(digitalRead(5)==HIGH && s4==0)
  {
    counter++;
    s4=1;
    digitalWrite(9, HIGH);
  }
  else if(digitalRead(5)==LOW && s4==1)
  {
    counter--;
    s4=0;
    digitalWrite(9, LOW);
  }

  int avail=4-counter;
  if(counter<4)
  {
    lcd.setCursor(0,0);   //column,row
    lcd.print("Available : ");
    //lcd.setCursor(0,6);
    lcd.print(avail);
    lcd.setCursor(0,1);
    lcd.print("Occupied : ");
    //lcd.setCursor(1,6);
    lcd.print(counter);
    Serial.println("available :");
    Serial.println(avail);
    Serial.println("occupied");
    Serial.println(counter);
  }
  else if(counter==4)
  {
    lcd.setCursor(0,0);
    lcd.print("   Parking     ");
    Serial.println("full parking");
    lcd.setCursor(0,1);
    lcd.print("     Full     ");
  }
  delay(2000);
}
