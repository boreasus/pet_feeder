#include <WiFi.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
Servo servo2; //Servo for tossing treats
// twelve servo objects can be created on most boards

// GPIO the servo is attached to
static const int servoPin = 13;

// Replace with your network credentials
const char* ssid     = "poyraz_wifi";
const char* password = "12345678";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Decode HTTP GET value
String valueString ="";
String valueString2 ="";
String valueString3 = "";
long x, y, z;
int pos1_gram = 0;
int pos2_gram = 0;
int pos1_time = 0;
int pos2_time = 0;
int pos1_treat = 0;
int pos2_treat = 0;
bool control= false;

// Current time
unsigned long currentTime = millis();
unsigned long Time = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (1000ms = 1s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(9600);

  myservo.attach(servoPin);  // attaches the servo on the servoPin to the servo object

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            
            client.println("$.get(\"/?valuex=\" + gram + \"&\"/?value2_=\" + time + \"*\"/?valueb=\" + bool + \"%\"); {Connection: close};}</script>");
            
            //GET /?value=180& HTTP/1.1
            if(header.indexOf("GET /?value=")>=0) {
              pos1_gram = header.indexOf('x');
              pos2_gram = header.indexOf('&');
              valueString = header.substring(pos1_gram+2, pos2_gram);
              pos1_time = header.indexOf('_');
              pos2_time = header.indexOf('*');
              valueString2 = header.substring(pos1_time+2, pos2_time);
              pos1_treat = header.indexOf('b');
              pos2_treat = header.indexOf('%');
              valueString3= header.substring(pos1_treat+2, pos2_treat);
              if (valueString3 == 't') {
                servo2.write(180);
              }
              
              //Rotate the servo
              myservo.write(valueString.toInt());
              x= Time;
              control= true;
              y= (valueString.toInt())/50*1080000; //Converted grams to microseconds for the continous servo motor
            }
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
  }
  if ((Time >= x + (valueString2.toInt()-10000) && Time <= x + (valueString2.toInt()+10000)) && control == true) {
    myservo.writeMicroseconds(y);
    control = false;
  }
}
