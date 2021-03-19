// constants won't change. They're used here to set pin numbers:
const int buttonPin[]={5,6,7,8};
const char buttonCode[]={'p','c','b','m'};
const String games[]={"PANG","MARVEL","BUBBLE","MARIO"};
const int ledPin[]={25,26,27,28};
int cont_wait=0;
int cont_blink=0;
int etapa=0; //1: ESPERA CONEXION 2:READY 3:PROCESANDO 4:ACTIVO
int active_game=0;

// variables will change:
int buttonState[] = {0,0,0,0};         // variable for reading the pushbutton status
int buttonState0[] = {0,0,0,0}; 

int a=35,b=36,c=37,d=38,e=39,f=40,g=41;//Pines de salida para led's
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
    byte dato = Serial.read(); //Los almacena en la variable "dato"
    if(dato==65)  //Si recibe una "A" (en ASCII "65")
    {
      etapa=2;
      cont_wait=0; //Reset
      //digitalWrite(13,HIGH);
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

  pinMode(a,OUTPUT);//LED's del DISPLAY 7 Segmentos
  pinMode(b,OUTPUT);
  pinMode(c,OUTPUT);
  pinMode(d,OUTPUT);
  pinMode(e,OUTPUT);
  pinMode(f,OUTPUT);
  pinMode(g,OUTPUT);



   // pinMode(13, OUTPUT);
  //Etapa espera conexion
 // etapa_wait=true;

  //Puerto de comunicacion en 9600 baudios
  Serial.begin(9600);

  etapa=1;
}

void loop() {

  numero(aux++);

  //ESPERANDO CONEXION
  if(etapa==1)
  {
    waitingSystem();  
  }
  else if(etapa==2)
  {  
      //LECTURA DE PULSADORES
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
        char game = Serial.read(); //Los almacena en la variable "dato"
        for(int i=0;i<4;i++)
        {
          if(buttonCode[i]==game)
          {
             active_game=ledPin[i];
             clearLed();
             etapa=3;
          }
        }
      }
  }
  else if(etapa==3)
  { 
      //JUEGO ACTIVADO/DESACTIVADO
      blinkLed(active_game);
  
      if(Serial.available()>0)//Si el Arduino recibe datos a través del puerto serie
      {
        String game = Serial.readString(); //Los almacena en la variable "dato"
  
        for(int i=0;i<4;i++)
        {
          if(game.equals("NONE"))
          {
            active_game=0;
          }
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

    delay(1000);
}
