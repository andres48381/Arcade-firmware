const String version="0.3.0";
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const int buttonPin[]={5,6,7,8};
const char buttonCode[]={'p','c','r','m'};
const String games[]={"PANG","MARVEL","RAYMAN","SUPER MARIO"};
const int ledPin[]={25,26,11,12};
int cont_wait=0;
int cont_blink=0;
int state=0; //1: ESPERA CONEXION 2:READY 3:PROCESANDO 4:ACTIVO
int activedGame=0;
//Battery
String level_battery = "XX"; 
char status_battery='x'; 

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

/**
  * @brief
  * @param 
  * @return 
  */
void setLCD(String m1,String m2)
{
  //LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(m1);
  lcd.setCursor(0,1);
  lcd.print(m2); 
}
/**
  * @brief 
  * @return 
  */
void setLCD(String m)
{
  //LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BAT:"+level_battery+status_battery+" VOL:"+level_sound);
  lcd.setCursor(0,1);
  lcd.print(m); 
}
  /**
  * @brief 
  * @return 
  */
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
      state=2;
      cont_wait=0; //Reset
      clearLed();
      setLCD("PULSA JUEGO"); 
    }
  }
}
/**
  * @brief 
  * @return 
  */
void clearLed()
{
  for(int i=0;i<4;i++)
  {
    digitalWrite(ledPin[i],LOW);
  }
}
/**
  * @brief 
  * @return 
  */
void blinkLed(int led)
{
  digitalWrite(led,++cont_blink%2>0?HIGH:LOW);
}
/**
  * @brief 
  * @return 
  */
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

  //Puerto de comunicacion en 9600 baudios
  Serial.begin(9600);

  state=1;

 //Melodia de incializacion
  for (int i = 0; i < numTones; i++)
  {
    tone(speakerPin, tones[i]);
    delay(100);
  }
  noTone(speakerPin);

  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  setLCD("   INICIANDO"," MARCOS ARCADE"); 
}
/**
  * @brief 
  * @return 
  */
void loop() {

  //1: ESPERA CONEXION
  if(state==1)
  {
    waitingSystem(); 
  }
  //2:READY
  else if(state==2)
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
      if(Serial.available()>0)
      {
        char id = Serial.read();
        String mess="  PULSA JUEGO";
        
        if(id=='S')
        {
          int sound = Serial.read();
          if(sound>=0 && sound<=100) 
          {
            level_sound = String(sound); 
          }

          for(int i=0;i<4;i++)
          {
            if(activedGame==ledPin[i])
            {
               mess=games[i];
            }
          }
          setLCD(mess); 
        }
        else if(id=='B')
        {         
          int i_level_battery = Serial.read();
          char c_status_battery = Serial.read();

          if(i_level_battery>0 && i_level_battery<=100)
          {
            level_battery = String(i_level_battery);
          }

          if(c_status_battery=='-' || c_status_battery=='+')
          {
            status_battery = c_status_battery;
          }
          
          for(int i=0;i<4;i++)
          {
            if(activedGame==ledPin[i])
            {
               mess=games[i];
            }
          }
          setLCD(mess);
        }
        else
        {
          char data = Serial.read();
          for(int i=0;i<4;i++)
          {            
            //char game=data;            
            if(data==buttonCode[i])
            {
              activedGame=ledPin[i];
              clearLed();
              state=3;
             
              for(int i=0;i<4;i++)
              {
                if(activedGame==ledPin[i])
                {
                  setLCD("CONFIGURANDO...",games[i]);
                }
              }
            }
          }
         }  
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
  //3:PROCESANDO
  else if(state==3)
  { 
      //JUEGO ACTIVADO/DESACTIVADO
      blinkLed(activedGame);
  
      if(Serial.available()>0)//Si el Arduino recibe datos a través del puerto serie
      {     
        String game = Serial.readStringUntil('/'); //Los almacena en la variable "dato"  

        if(game.equals("NONE"))
        {
          activedGame=0;
        }       
  
        state=4;
      }
  }
  //4:ACTIVO
  else if(state==4)
  { 
      if(activedGame==0)
      {
        clearLed();
        setLCD("  PULSA JUEGO"); 
      }
      else
      {
         digitalWrite(activedGame,HIGH);

         for(int i=0;i<4;i++)
         {
            if(activedGame==ledPin[i])
            {
               setLCD(games[i]);
            }
         }
      }
  
      state=2;
  }

    delay(150);
}
