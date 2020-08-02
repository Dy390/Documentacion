#define CLK 3
#define DTA 4

String st;
String msg;

long ledActivity;

void setup()
{
  pinMode(CLK,INPUT);
  pinMode(DTA,INPUT);
  pinMode(13,OUTPUT);
  Serial.begin(9600);
  Serial.println("Start");
}

void loop()
{
    // Display led if no activity. Detect activity on bus
    if (millis() - ledActivity > 1000) digitalWrite(13,0); else digitalWrite(13,1);
  
    if (waitCLKchange(1) > 200)
    {
      // Message start detected
      
      st = "";
      msg = "";
      
      while (1)
      {
        // CLK is low. Let's wait until it goes up, max 500 usec.
        if (waitCLKchange(0) > 50) break;
        
        // CLK is high. Read a bit
        delayMicroseconds(100);
        if (digitalRead(DTA)) st += "1"; else st += "0";
        
        // Wait for CLK to drop again
        if (waitCLKchange(1) > 50) break;
      }
      
      // Get command number
      int cmd = getBinaryData(st,0,8);
      
      // Command 5 is status from panel. If we got a status, we got activity. Reset counter for inactvity for the LED
      if (cmd == 5 || cmd == 0x27) ledActivity = millis();
      
      if (cmd == 5)
      {
        int zones = getBinaryData(st,9+8+8+8,8);
        msg += "Zones " + String(zones);
      }
      
      // Interpret the TIME broadcast
      if (cmd == 165)
      {
        int year3 = getBinaryData(st,9,4);
        int year4 = getBinaryData(st,13,4);
        int month = getBinaryData(st,19,4);
        int day = getBinaryData(st,23,5);
        int hour = getBinaryData(st,28,5);
        int minute = getBinaryData(st,33,6);
        msg += "Date: 20" + String(year3) + String(year4) + " " + String(month) + " " + String(day) + " " + String(hour) + ":" + String(minute);
      }
      
      
      // Send findings to USB port
      if (cmd != 0 && st != "00000101010000001000000010000000011000111000000001100011100000000110001111")
      {
        Serial.print(st);
        Serial.print(" ");
        Serial.print(cmd);
        Serial.print(" ");
        Serial.print(msg);
  
        Serial.println("");
      }
    }
    
}


int waitCLKchange(int currentState)
{
  int c = 0;
  while (digitalRead(CLK) == currentState)
  {
    delayMicroseconds(10);
    c++;
    if (c > 1000) break;
  }
  return c;
}


int getBinaryData(String st, int offset, int length)
{
  int buf = 0;
  for (int j=0)
}
