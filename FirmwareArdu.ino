// constants won't change. They're used here to set pin numbers:
const int buttonPin[]={5,6,7,8};
const char buttonCode[]={'p','c','b','m'};
const int ledPin=13;

// variables will change:
int buttonState[] = {0,0,0,0};         // variable for reading the pushbutton status
int buttonState0[] = {0,0,0,0}; 

void setup() {
  // initialize the LED pin as an output:
  for(int i=0;i<4;i++)
  {
    pinMode(buttonPin[i], INPUT);
  }
  // initialize the pushbutton pin as an input:
  pinMode(ledPin, OUTPUT);

    //Puerto de comunicacion en 9600 baudios
  Serial.begin(9600);
}

void loop() {
  // read the state of the pushbutton value:
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

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
 /* if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    if(buttonState0==0){
      Serial.println("p");
    }
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }

  buttonState0=buttonState;*/
}
