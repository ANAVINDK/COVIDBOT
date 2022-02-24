#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
#define rf1      4
#define rf2      5
#define rf3      6
#define rf4      7
 
#define sw1      8
#define sw2      9
#define sw3      10
#define sw4      11

#define motA     A0
#define motB     A1
#define buzzer   A2
#define tray1    2
#define tray2    3


unsigned char a[15],i,room=1,room_set_feed=0,room_set=1,stage=0,obj=0,action_en=0;

unsigned long currentMillis = 0;
unsigned long previousMillis1 = 0;
unsigned int interval1 = 1000,sec1=0,sec2=0,sec3=0;

void timer();
void forward();
void backward();
void stp();
void movement(char);
void request_check();
void action();

void setup()
{
  pinMode(motA,OUTPUT);
  pinMode(motB,OUTPUT);
  pinMode(tray1,OUTPUT);
  pinMode(tray2,OUTPUT);

  pinMode(buzzer,OUTPUT);
 
  pinMode(rf1,INPUT_PULLUP);
  pinMode(rf2,INPUT_PULLUP);
  pinMode(rf3,INPUT_PULLUP);
  pinMode(rf4,INPUT_PULLUP);
  
  pinMode(sw1,INPUT_PULLUP);
  pinMode(sw2,INPUT_PULLUP);
  pinMode(sw3,INPUT_PULLUP);
  pinMode(sw4,INPUT_PULLUP);
  lcd.begin();
  Serial.begin(9600);
  Serial.println("COVIDBOT getting ready..");
  
  digitalWrite(motA,LOW); digitalWrite(motB,LOW);
  digitalWrite(tray1,LOW); digitalWrite(tray2,LOW); 
  digitalWrite(buzzer,LOW);
    
  lcd.setCursor(0, 0);lcd.print("    COVIDBOT    ");    
  lcd.setCursor(0, 1);lcd.print("                ");   
  delay(2000);
  lcd.setCursor(0, 0);lcd.print("Room1     Vh-STP");
  lcd.setCursor(0, 1);lcd.print("         -IDEAL-");
 
}
void loop()
{ 
  timer();
  
  if(Serial.available()>0)
  {
    a[i]= Serial.read();delay(25);
    i++; 
  }
  
  if(i>1)     
  { 
    if     (a[0]=='*'&&a[1]=='A'){room=1; }        //*A --- card 1
    else if(a[0]=='*'&&a[1]=='B'){room=2; }        //*B --- card 2
    else if(a[0]=='*'&&a[1]=='C'){room=3; }        //*C --- card 3
    else if(a[0]=='*'&&a[1]=='D'){room=4; }        //*D --- card 4
    
    lcd.setCursor(4, 0);lcd.print(room,DEC); 
   
    i=0;
  }

   
    if     ((digitalRead(rf1)==0||digitalRead(sw1)==0||room_set_feed==1)&&stage==0){room_set=1;room_set_feed=0;stage=1;}
    else if((digitalRead(rf2)==0||digitalRead(sw2)==0||room_set_feed==2)&&stage==0){room_set=2;room_set_feed=0;stage=1;}
    else if((digitalRead(rf3)==0||digitalRead(sw3)==0||room_set_feed==3)&&stage==0){room_set=3;room_set_feed=0;stage=1;}
    else if((digitalRead(rf4)==0||digitalRead(sw4)==0||room_set_feed==4)&&stage==0){room_set=4;room_set_feed=0;stage=1;}
  
  
  if(stage==1)
  { 
    request_check();
    
    if     (room_set>room) {movement('F'); Serial.println("FORWARD");delay(100);}
    else if(room_set<room) {movement('B'); Serial.println("BACKWARD");delay(100);}
    else if(room_set==room)
    {
      stp();Serial.println("STOP");
      lcd.setCursor(9, 1);lcd.print("-IDEAL-");
      if(action_en>0){action();action_en=0;} 
      stage=2;
    }
  }
  else if(stage==2)
  {
    sec3=0;
    while(sec3<5){request_check();timer();}stage=0;       //waiting time after one cycle
  }
}

void request_check()
{
  if     (digitalRead(rf1)==0||digitalRead(sw1)==0){room_set_feed=1; }
  else if(digitalRead(rf2)==0||digitalRead(sw2)==0){room_set_feed=2; }
  else if(digitalRead(rf3)==0||digitalRead(sw3)==0){room_set_feed=3; }
  else if(digitalRead(rf4)==0||digitalRead(sw4)==0){room_set_feed=4; }
}

void stp()    
{
  digitalWrite(motA,LOW); digitalWrite(motB,LOW); 
  lcd.setCursor(13, 0);lcd.print("STP");
  
}
void forward() 
{
  action_en=1;
    digitalWrite(buzzer,LOW);
    digitalWrite(motA,HIGH); digitalWrite(motB,LOW); 
    lcd.setCursor(13, 0);lcd.print("FWD");

}
void backward()
{
  action_en=1;

    digitalWrite(buzzer,LOW);
    digitalWrite(motA,LOW); digitalWrite(motB,HIGH); 
    lcd.setCursor(13, 0);lcd.print("BKW");

}


void movement(char move )
{
    lcd.setCursor(9,1); lcd.print(">Room-"); 
    lcd.setCursor(15,1);lcd.print(room_set,DEC); 
    if(move=='F'){ forward(); }
    if(move=='B'){ backward();}
}
void action()
{
  digitalWrite(buzzer,HIGH);
  digitalWrite(tray1,LOW); digitalWrite(tray2,HIGH);
  delay(3000);digitalWrite(buzzer,LOW);
  digitalWrite(tray1,LOW); digitalWrite(tray2,LOW);
  delay(5000);
  digitalWrite(buzzer,HIGH);
  digitalWrite(tray1,HIGH); digitalWrite(tray2,LOW);
  delay(3000);digitalWrite(buzzer,LOW);
  digitalWrite(tray1,LOW); digitalWrite(tray2,LOW);
  
  }

void timer()
{
  currentMillis = millis();
  if ((unsigned long)(currentMillis - previousMillis1) >= interval1)   // check for rollover delay 1
  {
    sec1++;sec2++;sec3++;
    previousMillis1 = currentMillis;
  } 
}
