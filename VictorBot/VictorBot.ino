#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "XXXXXXXXXXX";
const char* password = "XXXXXXXXXX";

// Initialize Telegram BOT
#define BOTtoken "6276768911:AAEVeSQSaddkU2_k-f6R-2AIkX0WXTCKhao"  // Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "XXXXXXXXXX" //Replace with your chatID
#define CHAT_ID2 "XXXXXXXXXX" //Replace with another ChatID

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

//Serial ports for comunication with arduino
#define RXD2 16
#define TXD2 17
//Variables received from arduino
float airTemp;
float airHum;
int groundHum;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = 2;
bool ledState = LOW;

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID && chat_id != CHAT_ID2){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Bienvenido camarada " + from_name + ".\n";
      welcome += "Usa los siguientes comandos para conquistar Ucrania:\n\n";
      welcome += "/measure mostrar el estado actual de tu planta \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    if (text == "/measure") {
      bot.sendMessage(chat_id, "Air Temperature: " + String(airTemp) + " degrees\n Air Humidity: " + String(airHum) +"%\n Ground Humidity: "+ String(groundHum) + "%", "");
    }
  }
}
/* Function that updates the values received from arduino with the information of the String passed by the parameter values.
 * The values received must follow the next patron:
 * :Air_Temperature:Air_Humidity:Ground_Humidity:
 */
void updateArduinoFields(String values){
  //Controls new set of measurements
  bool read = false;
  //word of variable size who stores one of the fields of the values string separated by ':'
  String palabra = "";
  //Counter of the field which is being readed.
  int cont = 0;

  for(int i = 0; i < values.length(); i++){
    if(values[i] == ':'){
      if(read){
        //Switchs in function of the field we are into.
        switch (cont){
          case 0:
            airTemp = palabra.toFloat();
            break;
          case 1:
            airHum = palabra.toFloat();
            break;
          case 2:
            groundHum = palabra.toInt();
            break;
        }
        cont++;
        //set the word to an empty string again
        palabra = "";
      }else{
        //Sets read to true indicating that we are reading a whole new set of values.
        read = true;
      }
    }else if (read){
      //reads the field from the parameter values
      palabra += values[i];
    }
  }

}

void setup() {
  //Initialization of serials; 1 for display, 2 for comunication with arduino 
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1,RXD2,TXD2);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  #endif

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  //Initialization of arduino variables
  airTemp = 0;
  airHum = 0;
  groundHum = 0;
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    updateArduinoFields(Serial2.readString());
    lastTimeBotRan = millis();
  }
}
