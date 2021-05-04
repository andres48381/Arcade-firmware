const String version="0.3.0";
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// constants won't change. They're used here to set pin numbers:
const int buttonPin[]={5,6,7,8};
const char buttonCode[]={'p','c','r','m'};
const String games[]={"PANG","MARVEL","RAYMAN","MARIO"};
const int ledPin[]={25,26,11,12};
int cont_wait=0;
int cont_blink=0;
int etapa=0; //1: ESPERA CONEXION 2:READY 3:PROCESANDO 4:ACTIVO
int active_game=0;
//Battery
String level_battery = "XX"; 
String status_battery = ""; 

//Sonido
const int soundPin[]={52,53};
const char soundCode[]={'o','i'}; //o: decrease i:increase
int speakerPin = 9;
String level_sound="10";
 
int numTones = 10;
int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440};
//            mid C  C#   D    D#   E    F    F#   G    G#   A

// variables will change:
int buttonState[] = {0,0,0,0};         // variable for reading the game pushbutton status
int buttonState0[] = {0,0,0,0}; 

int soundState[] = {0,0};         // variable for reading the sound pushbutton status
int soundState0[] = {0,0}; 

int aux=1;
void setLCD(String m1,String m2)
{
  //LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(m1);
  lcd.setCursor(0,1);
  lcd.print(m2); 
}
void setLCD(String m)
{
  //LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BAT:"+level_battery+status_battery+" VOL:"+level_sound);
  lcd.setCursor(0,1);
  lcd.print(m); 
}
  
void waitingSystem()
{
  int led=cont_wait/3; //3xCiclos de espera

  if((led>=0) && (led<4))
  {
     //Reset
    clearLed();
    digitalWrite(ledPin[led],HIGH);
  }
  else
  {
    cont_wait=0; //Reset
  }

  cont_wait++;

  //Check serial port
  if(Serial.available()>0)//Si el Arduino recibe datos a través del puerto serie
  {
    String dato = Serial.readStringUntil('/'); //Los almacena en la variable "dato"
    if(dato.equals("A"))  //Si recibe una "A" 
    {
      etapa=2;
      cont_wait=0; //Reset
//      digitalWrite(13,HIGH);
      //Reset
      clearLed();
      setLCD("PULSA JUEGO"); 
    }
  }
  
}
void clearLed()
{
  for(int i=0;i<4;i++)
  {
    digitalWrite(ledPin[i],LOW);
  }
}

void blinkLed(int led)
{
  digitalWrite(led,++cont_blink%2>0?HIGH:LOW);
}

void setup() {

  for(int i=0;i<4;i++)
  {
    pinMode(buttonPin[i], INPUT);
    pinMode(ledPin[i], OUTPUT);
  }

  for(int i=0;i<2;i++)
  {
    pinMode(soundPin[i], INPUT);
  }

   pinMode(13, OUTPUT);
  //Etapa espera conexion
 // etapa_wait=true;

  //Puerto de comunicacion en 9600 baudios
  Serial.begin(9600);

  etapa=1;

 //Melodia de incializacion
  for (int i = 0; i < numTones; i++)
  {
    tone(speakerPin, tones[i]);
    delay(100);
  }
  noTone(speakerPin);

  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  setLCD("   INICIANDO","CONSOLA MARCOS"); 
}

void loop() {

  //ESPERANDO CONEXION
  if(etapa==1)
  {
    waitingSystem(); 
  }
  else if(etapa==2)
  {  
      //LECTURA DE PULSADORES JUEGO
      for(int i=0;i<4;i++)
      {
         buttonState[i] = digitalRead(buttonPin[i]); 
         
        if (buttonState[i] == HIGH) {
    
          if(buttonState0[i]==0){
            Serial.println(buttonCode[i]);
          }  
        }
           buttonState0[i]=buttonState[i];
      }   
  
      //Check game started
      //Check serial port
      if(Serial.available()>0)//Si el Arduino recibe datos a través del puerto serie
      {
        //String data = Serial.readStringUntil('/'); //Los almacena en la variable "dato"
        char id = Serial.read();

        if(id=='S')
        {
          int sound = Serial.read();
          level_sound = String(sound); 
          setLCD("PULSA JUEGO"); 
        }
        else if(id=='B')
        {
          level_battery = String(Serial.read());
          //status_battery = Serial.readStringUntil('/'); 
          setLCD("PULSA JUEGO"); 
        }
        else
        {
          char data = Serial.read();
          for(int i=0;i<4;i++)
          {            
            //char game=data;            
            if(data==buttonCode[i])
            {
              active_game=ledPin[i];
              clearLed();
              etapa=3;
              //digitalWrite(13,HIGH);
              for(int i=0;i<4;i++)
              {
                if(active_game==ledPin[i])
                {
                  setLCD("INICIANDO...",games[i]);
                }
              }
            }
          }
         }  
/*
        //Battrey info
        if(data.equals("B"))
        {
          level_battery = Serial.readStringUntil('/'); 
          status_battery = Serial.readStringUntil('/'); 
          setLCD("PULSA JUEGO"); 
        }
        //Sound info
        else if(data.equals("S"))
        {
          level_sound = Serial.readStringUntil('/'); 
          setLCD("PULSA JUEGO"); 
        }
        else
        {
          String state_game = Serial.readStringUntil('/'); 
          for(int i=0;i<4;i++)
          {            
            //char game=data;            
            if(data.equals(String(buttonCode[i])))
            {
              active_game=ledPin[i];
              clearLed();
              etapa=3;
              //digitalWrite(13,HIGH);
              for(int i=0;i<4;i++)
              {
                if(active_game==ledPin[i])
                {
                  setLCD("INICIANDO...",games[i]);
                }
              }
            }
          }
         }  */
      }

      //LECTURA DE PULSADORES SONIDO
      for(int i=0;i<2;i++)
      {
         soundState[i] = digitalRead(soundPin[i]); 
         
        if (soundState[i] == HIGH) {
    
          if(soundState0[i]== LOW){
            Serial.println(soundCode[i]);
          }  
        }
           soundState0[i]=soundState[i];
      } 
  }
  else if(etapa==3)
  { 
      //JUEGO ACTIVADO/DESACTIVADO
      blinkLed(active_game);
  
      if(Serial.available()>0)//Si el Arduino recibe datos a través del puerto serie
      {     
        String game = Serial.readStringUntil('/'); //Los almacena en la variable "dato"  

        if(game.equals("NONE"))
        {
          active_game=0;
          digitalWrite(13,LOW);
        }       
  
        etapa=4;
      }
  }
  else if(etapa==4)
  { 
    //
      if(active_game==0)
      {
        clearLed();
        setLCD("PULSA JUEGO"); 
      }
      else
      {
         digitalWrite(active_game,HIGH);

         for(int i=0;i<4;i++)
         {
            if(active_game==ledPin[i])
            {
               setLCD(games[i]);
            }
         }
      }
  
      etapa=2;
  }

    delay(50);
}
