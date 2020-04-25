/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const int gpio_D1 = 5;
const int gpio_D2 = 4;
const int gpio_D3 = 0;

/* Set these to your desired credentials. */
const char *ssid = "Drifter";
const char *password = "MaxPower";

ADC_MODE(ADC_TOUT);
//const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0
//String versorgungsspannung = "n/a";         //the ASCII of the integer will be stored in this char array

String measurements = "";
String measurementsComplete = "n/a";

bool updateTempString = false;
unsigned int someOneConnected = 0;
#define SIGNAL_CONNECTION_TIMEOUT 20

char HTML[] = "<html><head><title>Drifter</title><style>.button{background-color:grey;border:none;color:white;text-align:center;text-decoration:none;display:inline-block;font-size:10vw;margin:8px 2px;cursor:pointer;width:100%;}</style></head><body><a href=\"/cells\"><button class=\"button\">Zellen&uuml;berwachung</button></a><br/><!--<a href=\"/temp\"><button class=\"button\">nodeMCU-Temperatur</button></a><br/>--></body></html>";

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
  someOneConnected = SIGNAL_CONNECTION_TIMEOUT;
  server.send(200, "text/html", HTML);
}

// This function will be called whenever anyone requests 192.168.4.1/1 within the local area connection of this ESP module.
void handleCells()
{
  someOneConnected = SIGNAL_CONNECTION_TIMEOUT;
  server.send(200, "text/html", "<html><head><meta http-equiv=\"refresh\" content=\"1\"><title>Drifter - Zellen&uuml;berwachung</title><style>body{font-size:8vw;}.cells{border-collapse:collapse;width:100%;font-size:4vw;}.cells td, .cells th {border: 1px solid #ddd;padding:8px;}.cells tr:nth-child(even){background-color: #f2f2f2;}.cells tr:hover {background-color: #ddd;}.cells th{padding-top:12px;padding-bottom:12px;text-align:left;background-color:grey;color: white;}</style></head><body><pre>" + measurementsComplete + "</pre></body></html>");
}

//void handleTemp()
//{
//  someOneConnected = SIGNAL_CONNECTION_TIMEOUT;
//  server.send(200, "text/html", "<html><head><meta http-equiv=\"refresh\" content=\"1\"><title>Drifter - nodeMCU-Temperatur</title><style>body{font-size:8vw;}</style></head><body>ADC-Wert an Pin A0: " + versorgungsspannung + "</body></html>");
//}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/cells",handleCells);
//  server.on("/temp",handleTemp);
  server.begin();
  Serial.println("HTTP server started");
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(gpio_D1, OUTPUT);
  digitalWrite(gpio_D1, LOW);
  pinMode(gpio_D2, OUTPUT);
  digitalWrite(gpio_D2, LOW);
  pinMode(gpio_D3, OUTPUT);
  digitalWrite(gpio_D3, LOW);
}

unsigned int toggle = 0;

void loop() {
  server.handleClient();

  measureCells();

  if(someOneConnected > 0)
    --someOneConnected;
  if(someOneConnected)
  {
    if(++toggle%2)
    {
      digitalWrite(LED_BUILTIN, LOW);
    }
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(220);
  }
  else
  {
    if(++toggle%2)
    {
      digitalWrite(LED_BUILTIN, LOW);
    }
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(720);
  }
}

unsigned int measureCell(unsigned int cell)
{
  digitalWrite(gpio_D1, cell&0x1);
  digitalWrite(gpio_D2, cell&0x2);
  digitalWrite(gpio_D3, cell&0x4);
  delay(10);
#ifdef DEBUG
  String tmp = String(cell);
  Serial.print("measuring cell #");
  Serial.print(tmp);
  Serial.print(", gpios: ");
  tmp = String(cell&0x4);
  Serial.print(tmp);
  tmp = String(cell&0x2);
  Serial.print(tmp);
  tmp = String(cell&0x1);
  Serial.println(tmp);
#endif
  // read the analog value
  return analogRead(A0);
}

void measureCells(void)
{
  int values[8];
  for(unsigned int cell=0; cell<8; cell++)
  {
    values[cell] = measureCell(cell);
  }

  measurements = "<table class=\"cells\"><tr><th>Zelle</th><th>Spannung</th></tr>";
  for(unsigned int cell=0; cell<8; cell++)
  {
    //measurements += "<tr><td>" + cell + "</td><td>" + values[cell] + "</td></tr>";
    measurements = String(measurements + "<tr><td>" + cell + "</td><td>" + values[cell] + "</td></tr>");
  }
  measurements = String(measurements + "</table>");
#ifdef DEBUG
Serial.println(measurements);
#endif
  measurementsComplete = measurements;
//  versorgungsspannung = "";
//  String sensorwert = String(analogRead(A0), DEC);
//  versorgungsspannung = sensorwert;


}
