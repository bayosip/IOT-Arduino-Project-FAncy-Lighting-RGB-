#define LED_R 11
#define LED_G 10
#define LED_B 9
#define BTTN_ONOFF 2
#define BTTN_COLOR 3
#define	NUM_COLORS	7
#define LDR A0
#define ADC 255

float colors[NUM_COLORS][3] = {{1.0,1.0,1.0},{1.0,0.0,0.0},{1.0,1.0,0.0},
							   {0.0,1.0,0.0},{0.0,1.0,1.0},
							   {0.0,0.0,1.0},{1.0,0.0,1.0}};
int currentColor=0;
volatile int ledState = 0;

// variable to hold sensor value
int sensorValue;
// variable to calibrate low value
int sensorLow = 1023;
// variable to calibrate high value
int sensorHigh = 0;
// led state, 1 = lit, 0 = dark
int fadeAmount = 5;    // how many points to fade the LED by
int brightness = ADC;    // how bright the LED is

void changeColors(void);
void calibrateLDR(void);
void fadeOff(int color);
bool buttonChangeColor (void);
void lightUpLed(int i, int br);
void turnOnOffLed (int state);
void checkButtonOnOFF (void);
int ldrControl(void);


void setup()
{
		Serial.begin(9600);
		pinMode(BTTN_ONOFF, INPUT);
		pinMode(BTTN_COLOR, INPUT);
		pinMode (LDR, INPUT);
		pinMode(LED_R, OUTPUT);
		pinMode(LED_G, OUTPUT);
		pinMode(LED_B, OUTPUT);
		calibrateLDR();
		
		// Interrupt call
		attachInterrupt(0, checkButtonOnOFF, RISING);
}

void calibrateLDR(void){
	
	// calibrate for the first five seconds after program runs
  while (millis() < 6000) {
    // record the maximum sensor value
    sensorValue = analogRead(LDR);
    if (sensorValue > sensorHigh) {
      sensorHigh = sensorValue;
    }
    // record the minimum sensor value
    if (sensorValue < sensorLow) {
      sensorLow = sensorValue;
    }
  }
  		turnOnOffLed (0);
  		Serial.print("Low Threshold: ");
  		Serial.println(sensorLow);
  		Serial.print("High Threshold: ");
		Serial.println(sensorHigh);
}

void loop(){
	if(ldrControl() == 1|| ledState == 1){
		if (buttonChangeColor())changeColors();
		
	}
	
}


void changeColors(void){
	if (buttonChangeColor()){
  		fadeOff(currentColor);
  		if (++currentColor == NUM_COLORS) currentColor = 0;
  		fadeOff(currentColor);
  		fadeOn(currentColor);    
  	// wait for 30 milliseconds to see the dimming effect    
  		delay(30); }                        
}

void fadeOn (int color){
	
	while (brightness < 255){
  			lightUpLed(color, brightness);
  			brightness = brightness + fadeAmount;

  		// reverse the direction of the fading at the ends of the fade: 
		if (brightness == 255) {
    		break;
  		}
	}
}

void fadeOff(int color){
	while (brightness >0){
  			lightUpLed(color, brightness);
  			brightness = brightness - fadeAmount;

  		// reverse the direction of the fading at the ends of the fade: 
		if (brightness == 0) {
    		break;
  		}     
		delay(30);
	}
}

bool buttonChangeColor (void){
	
	bool bttnPushed = false;
	static int buttonStateColor = 0;
	
	int tempState = digitalRead(BTTN_COLOR);
	
	 if (buttonStateColor != tempState) {
		delay(100);
		tempState = digitalRead(BTTN_COLOR);
	}
	
	if (buttonStateColor == 0 && tempState == 1) { 
		bttnPushed = true;
	}
	
	buttonStateColor = tempState;
	
	return bttnPushed;
}

void lightUpLed(int i, int br){
	
	analogWrite(LED_R, (colors[i][0])*br);
	analogWrite(LED_G, (colors[i][1])*br);
	analogWrite(LED_B, (colors[i][2])*br);
}

void turnOnOffLed (int state){
	
	digitalWrite(LED_R, state);
	digitalWrite(LED_G, state);
	digitalWrite(LED_B, state);
	
}

void checkButtonOnOFF (void){
	
	static int buttonState = 0;
	
	int tempState = digitalRead(BTTN_ONOFF);
	
	
	 if (buttonState != tempState) {
		delay(100);
		tempState = digitalRead(BTTN_ONOFF);
		Serial.print("temp state is: "); 
		Serial.println(tempState);
	}
	
	if (buttonState == 0 && tempState == 1) { 
		Serial.print("LED state changed from "); 
		Serial.print(ledState);
		ledState = !ledState;
		Serial.print(" to ");
		Serial.println(ledState);
	}
	
	buttonState = !tempState;
	turnOnOffLed (ledState);
	Serial.println(buttonState);
}

int ldrControl(void){
	
	
	//read the input from A0 and store it in a variable
  sensorValue = analogRead(LDR);
  
  if (sensorValue <=sensorLow){ledState = 1;
	analogWrite(LED_R, (colors[currentColor][0])*255);
	analogWrite(LED_G, (colors[currentColor][1])*255);
	analogWrite(LED_B, (colors[currentColor][2])*255);
	changeColors();
	}
	
	if (sensorValue >= sensorHigh){
	ledState =0;
	turnOnOffLed(ledState);}
	return ledState;
}