// 44kHz Underwater Piezo Beacon
// ESP32 code for TC4427 gate driver

#define PIEZO_PIN 2        // GPIO pin connected to TC4427 input
#define FREQUENCY 44000    // 44kHz frequency
#define RESOLUTION 8       // 8-bit resolution (0-255)
#define DUTY_CYCLE 128     // 50% duty cycle for maximum power

void setup() {
  Serial.begin(115200);
  Serial.println("44kHz Piezo Beacon Starting...");
  
  // Configure LEDC peripheral for 44kHz output
  ledcSetup(0, FREQUENCY, RESOLUTION);  // Channel 0, 44kHz, 8-bit
  ledcAttachPin(PIEZO_PIN, 0);          // Attach GPIO to channel 0
  
  Serial.print("Frequency: ");
  Serial.print(FREQUENCY);
  Serial.println(" Hz");
  Serial.print("Duty Cycle: ");
  Serial.print((DUTY_CYCLE * 100) / 255);
  Serial.println("%");
  Serial.print("Output Pin: GPIO");
  Serial.println(PIEZO_PIN);
  
  // Start the 44kHz signal
  ledcWrite(0, DUTY_CYCLE);
  
  Serial.println("Signal active - beacon transmitting!");
  Serial.println("Use Serial Monitor commands:");
  Serial.println("'on' - Start beacon");
  Serial.println("'off' - Stop beacon");
  Serial.println("'freq XXXXX' - Set frequency (e.g. 'freq 44000')");
  Serial.println("'duty XXX' - Set duty cycle 0-255 (e.g. 'duty 128')");
}

void loop() {
  // Check for serial commands
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    if (command == "on") {
      ledcWrite(0, DUTY_CYCLE);
      Serial.println("Beacon ON");
    }
    else if (command == "off") {
      ledcWrite(0, 0);
      Serial.println("Beacon OFF");
    }
    else if (command.startsWith("freq ")) {
      int newFreq = command.substring(5).toInt();
      if (newFreq >= 1000 && newFreq <= 80000) {
        ledcSetup(0, newFreq, RESOLUTION);
        ledcWrite(0, DUTY_CYCLE);
        Serial.print("Frequency set to: ");
        Serial.print(newFreq);
        Serial.println(" Hz");
      } else {
        Serial.println("Invalid frequency. Use 1000-80000 Hz");
      }
    }
    else if (command.startsWith("duty ")) {
      int newDuty = command.substring(5).toInt();
      if (newDuty >= 0 && newDuty <= 255) {
        ledcWrite(0, newDuty);
        Serial.print("Duty cycle set to: ");
        Serial.print((newDuty * 100) / 255);
        Serial.println("%");
      } else {
        Serial.println("Invalid duty cycle. Use 0-255");
      }
    }
    else {
      Serial.println("Unknown command");
    }
  }
  
  delay(100);  // Small delay to prevent overwhelming serial
}

// Optional: Function to sweep frequency for testing resonance
void sweepFrequency(int startFreq, int endFreq, int stepTime) {
  Serial.println("Starting frequency sweep...");
  
  for (int freq = startFreq; freq <= endFreq; freq += 100) {
    ledcSetup(0, freq, RESOLUTION);
    ledcWrite(0, DUTY_CYCLE);
    
    Serial.print("Frequency: ");
    Serial.print(freq);
    Serial.println(" Hz");
    
    delay(stepTime);
  }
  
  // Return to 44kHz
  ledcSetup(0, FREQUENCY, RESOLUTION);
  ledcWrite(0, DUTY_CYCLE);
  Serial.println("Sweep complete - returned to 44kHz");
}