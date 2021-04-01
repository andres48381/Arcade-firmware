const String version="0.1.1";

// constants won't change. They're used here to set pin numbers:
const int buttonPin[]={5,6,7,8};
const char buttonCode[]={'p','c','b','m'};
const String games[]={"PANG","MARVEL","BUBBLE","MARIO"};
const int ledPin[]={25,26,27,28};
int cont_wait=0;
int cont_blink=0;
int etapa=0; //1: ESPERA CONEXION 2:READY 3:PROCESANDO 4:ACTIVO
int active_game=0;

//Sonido
const int soundPin[]={52,53};
const char soundCode[]={'o','i'}; //o: decrease i:increase
int speakerPin = 9;
 
int numTones = 10;
int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440};
//            mid C  C#   D    D#   E    F    F#   G    G#   A

// variables will change:
int buttonState[] = {0,0,0,0};         // variable for reading the game pushbutton status
int buttonState0[] = {0,0,0,0}; 

int soundState[] = {0,0};         // variable for reading the sound pushbutton status
int soundState0[] = {0,0}; 

int a=35,b=36,c=37,d=38,e=39,f=40,g=41;//Pines de salida para led's
int pin_charge=50;
int aux=1;

void numero(int num)//Representar números en el DISPLAY
{
  if(num==1 || num==4)digitalWrite(a,LOW);//Números {2,3,5,6} --> Segmento "a" del display encendido
  else digitalWrite(a,HIGH);//CONTROL SEGMENTO: A    
  
  if(num==5||num==6)digitalWrite(b,LOW);
  else digitalWrite(b,HIGH);//CONTROL SEGMENTO: B  
  
  if(num==2)digitalWrite(c,LOW);
  else digitalWrite(c,HIGH);//CONTROL SEGMENTO: C  
  
  if(num==1||num==4||num==7||num==9)digitalWrite(d,LOW);
  else digitalWrite(d,HIGH);//CONTROL SEGMENTO: D  
  
  if(num==2||num==6||num==8)digitalWrite(e,HIGH);
  else digitalWrite(e,LOW);//CONTROL SEGMENTO: E  
  
  if(num==4||num==5||num==6||num==8||num==9)digitalWrite(f,HIGH);
  else digitalWrite(f,LOW);//CONTROL SEGMENTO: F  
  
  if(num==1 || num==7)digitalWrite(g,LOW);
  else digitalWrite(g,HIGH);//CONTROL SEGMENTO: G  

  if(num==10)
  {
    digitalWrite(a,LOW);//CONTROL SEGMENTO: A  
    digitalWrite(b,LOW);//CONTROL SEGMENTO: B 
    digitalWrite(c,HIGH);//CONTROL SEGMENTO: C 
    digitalWrite(d,HIGH);//CONTROL SEGMENTO: D 
    digitalWrite(e,HIGH);//CONTROL SEGMENTO: E  
    digitalWrite(f,HIGH);//CONTROL SEGMENTO: F 
    digitalWrite(g,HIGH);//CONTROL SEGMENTO: G  
  }
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

  pinMode(a,OUTPUT);//LED's del DISPLAY 7 Segmentos
  pinMode(b,OUTPUT);
  pinMode(c,OUTPUT);
  pinMode(d,OUTPUT);
  pinMode(e,OUTPUT);
  pinMode(f,OUTPUT);
  pinMode(g,OUTPUT);

  pinMode(pin_charge,OUTPUT);

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
}

void loop() {

 // numero(aux++);

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
        String data = Serial.readStringUntil('/'); //Los almacena en la variable "dato"
        
        //Battrey info
        if(data.equals("b"))
        {
          String level_battery = Serial.readStringUntil('/'); 
          String status_battery = Serial.readStringUntil('/'); 
          numero(level_battery.toInt());
          digitalWrite(pin_charge,status_battery.equals("1")?HIGH:LOW);
        }
        else
        {
          String state_game = Serial.readStringUntil('/'); 
          for(int i=0;i<4;i++)
          {            
//           char game=data;            
            if(data.equals(String(buttonCode[i])))
            {
              active_game=ledPin[i];
              clearLed();
              etapa=3;
              digitalWrite(13,HIGH);
            }
          }
         }  
      }

      //LECTURA DE PULSADORES SONIDO
      for(int i=0;i<2;i++)
      {
         soundState[i] = digitalRead(soundPin[i]); 
         
        if (soundState[i] == HIGH) {
    
          if(soundState0[i]==0){
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
      }
      else
      {
         digitalWrite(active_game,HIGH);
      }
  
      etapa=2;
  }

    delay(150);
}
