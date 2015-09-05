// **********************************************************************
// USB Switch
// **********************************************************************

// User switch S1
int switchPin = D0;  // Input
int usbEnableAPin = D1; // Output - Not used.
int usbFlagAPin = D2;  // Input - Not used.
int usbFlagBPin = D3;  // Input
int usbEnableBPin = D4; // Output

int VUsbOutPin = A0;
int ISensePin = A1;
int VUsbInPin = A5;
// Pin 3/4 Tx/Rx for serial avaiable as test points.


int vin = 0;
int vout = 0;
int current = 0;
int zeroCurrent = 0;
int state = 0;
int faultCode = 0;

int on(String command);
int off(String command);
int zero(String command);

void setup() {
    // Take control of the spark LED and make it dimmer as it's very
    // bright by default.
    RGB.control(true);
    RGB.brightness(10);
    RGB.color(0, 255, 0);
    
    pinMode(switchPin, INPUT);
    pinMode(usbFlagAPin, INPUT);
    pinMode(usbFlagBPin, INPUT);
    
    pinMode(usbEnableAPin, OUTPUT);
    pinMode(usbEnableBPin, OUTPUT);
    
    // LM3526M-L is active low, set HIGH to switch off.
    digitalWrite(usbEnableAPin, HIGH);
    digitalWrite(usbEnableBPin, HIGH);
    
    // Let the output settle and read the current level
    // with the output switched off. This is the baseline
    delay(1000);
    zeroCurrent = analogRead(ISensePin);
      
    Particle.variable("vin", &vin, INT);
    Particle.variable("vout", &vout, INT);
    Particle.variable("current", &current, INT);
    Particle.variable("zeroCurrent", &zeroCurrent, INT);
    Particle.variable("state", &state, INT);
    Particle.variable("faultCode", &faultCode, INT);

    // register the cloud function
    Particle.function("on", on);
    Particle.function("off", off);
    Particle.function("zero", zero);
    
    
    //delay(1000);
    Particle.publish("Status", "IoT Usb Switch Setup complete.");
    RGB.color(0, 0, 0);
}

void loop() {
    //setFan(4);
    //digitalWrite(ledSampling, HIGH);
    RGB.color(0, 0, 255);
    
    readLevels();
    
    // TODO: Read Current
    // Read VIn
    // Read VOut
    // Check error state of USB
    
    // Reset the fault code before measuring.
    faultCode = 0;

    // Small delay before re-reading to see if this helps resolve the errors.
    delay(1000);
    //readTemperature();
    
    RGB.color(0, 0, 0);

    // Do not use Sleep as it breaks Photon WiFi!
    // Can we put the Core into low power mode?
    // Photon:
    //System.sleep(2);
    // Core:
    // Spark.sleep(1);
    // Sleep for x seconds.
    delay(1000);
} 

int on(String command)
{
    Particle.publish("Status", "Switching on.");
    digitalWrite(usbEnableBPin, LOW);
    state = 1;
    readLevels();
    return 1;
}

// Particle Method
int off(String command)
{
    Particle.publish("Status", "Switching off.");
    digitalWrite(usbEnableBPin, HIGH);
    state = 0;
    readLevels();
    return 0;
}

// Particle Method
int zero(String command)
{
    int currentRead = analogRead(ISensePin);
    zeroCurrent = currentRead;
    Particle.publish("Status", "Zering current measurement at " + String(currentRead));
    return currentRead;
}

// Particle Method
int reset(String args) {
    // Photon only?
    Particle.publish("Status", "I'm going to reset in 2 seconds. Reason: " + args);
    delay(2000);
    System.reset();
    return 0;
}

void readLevels() {
    // read Voltage and current levels
    vout = analogRead(VUsbOutPin);
    vin = analogRead(VUsbInPin);
    
    // Sensor works + and - so zero current gives 5v/2 for zero current.
    // Current should only flow in one direction. Take the diff here and use that.
    int currentRead = analogRead(ISensePin);
    current = zeroCurrent - currentRead;
}