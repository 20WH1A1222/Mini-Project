#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

LiquidCrystal lcd(13,12,14,27,26,25);


// Network credentials
const char *ssid = "dell";
const char *password = "12345678";

// Initialize Telegram BOT Token and Chat ID
#define BOTtoken "5982198371:AAEL3cD2U6IyORf9zlOD_sl5-X85MXyhKbc"  

// #define CHAT_ID "969506549"
#define CHAT_ID "2129129685"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int LDR0 = 15;     
const int LDR1 = 22;     
const int LDR2 = 5; 

const int Rain = 18;     

const int relay0 = 19; 
const int relay1 = 21; 

int LDRState0 = 0; 
int LDRState1 = 0; 
int LDRState2 = 0; 
int rainState = 0; 

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

long randNumber;

void setup() {

   Serial.begin(115200);
   sensors.begin();

   pinMode(LDR0, INPUT_PULLDOWN);
   pinMode(LDR1, INPUT_PULLDOWN);
   pinMode(LDR2, INPUT_PULLDOWN);

   pinMode(Rain, INPUT_PULLUP);
   
   pinMode(relay0, OUTPUT);
   pinMode(relay1, OUTPUT);
   
   
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);
   
   // Add root certificate for api.telegram.org
   client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 

   // put your setup code here, to run once:
  lcd.begin(16, 2);
  lcd.print("AUTO LIGHT INTEN");
  lcd.setCursor(0, 1);
  lcd.print("BASED ON WEATHER");
 
  // while (WiFi.status() != WL_CONNECTED) 
  // {
  //  WiFi.begin(ssid, password);
  //  Serial.print("WiFi Connecting.");
  //  delay(500);
  //  }
   
  bot.sendMessage(CHAT_ID, "Project started up", "");

  }

void loop() {
  
  // put your main code here, to run repeatedly:
  LDRState0     = digitalRead(LDR0);
  LDRState1     = digitalRead(LDR1);
  LDRState2     = digitalRead(LDR2);

  rainState     = digitalRead(Rain);
  
  if (millis() > lastTimeBotRan + botRequestDelay)  {
     int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
     while(numNewMessages) {
     Serial.println("got response");
     handleNewMessages(numNewMessages);
     numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
   } 
    
  
if(LDRState0 == HIGH)
 { 
  lcd.clear();
  lcd.print("LDR1 LOW     ");
  digitalWrite(relay0,HIGH);
  randNumber = random(8, 12);
  String message = "LDR1 Trigger";
  message =message +'\n' +"V:"+ String(randNumber);
  bot.sendMessage(CHAT_ID, message, "");
  
  LDRState1  = digitalRead(LDR1);

 if(LDRState1 == HIGH)
 { 
  bot.sendMessage(CHAT_ID, "REPAIR REQUIRED!!!!!!", "");
 } 
 else
 { 
  bot.sendMessage(CHAT_ID, "LIGHT ACTIVE", "");
 } 
}

  if(LDRState2 == HIGH)
 { 
  Serial.print("LDR2 Trigger");
  lcd.clear();
  lcd.print("LDR2 LOW     ");
  digitalWrite(relay1,HIGH);
  randNumber = random(11, 12);
  String message = "LDR2 Trigger";
  message =message +'\n' +"V:"+ String(randNumber);
  bot.sendMessage(CHAT_ID, message, "");
 }

else if(rainState == LOW)
 { 
  Serial.print("Rain Detected   ");
  lcd.clear();
  lcd.print("Rain Detected   ");
  digitalWrite(relay0,HIGH);
  bot.sendMessage(CHAT_ID, "It's Raining", "");
 }

else 
{
  lcd.setCursor(0, 0); 
  lcd.print("AUTO LIGHT INTEN");
  lcd.setCursor(0, 1);
  lcd.print("TEMP:");
  sensors.requestTemperatures(); 
  sensors.getTempCByIndex(0);
  lcd.print(sensors.getTempCByIndex(0));
  lcd.print("              ");
  digitalWrite(relay0,LOW);
  digitalWrite(relay1,LOW);
}  
  
}


void handleNewMessages(int numNewMessages) {
  
   for (int i=0; i<numNewMessages; i++) {
   
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
     
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);
 
    String from_name = bot.messages[i].from_name;
 
    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands\n\n";
      welcome += "/Light1on' to turn LIGHT ON \n";
      welcome += "/Light1off' to turn LIGHT OFF \n";
      welcome += "/Light2on' to turn LIGHT ON \n";
      welcome += "/Light2off' to turn LIGHT OFF \n";
      bot.sendMessage(chat_id, welcome, "");
    }
 
    if (text == "Light 1on") {bot.sendMessage(chat_id, "Light1 state set to ON", "");
                             digitalWrite(relay0, HIGH);                             
                             }
    if (text == "/Light1on") {bot.sendMessage(chat_id, "Light1 state set to ON", "");
                             digitalWrite(relay0, HIGH);}
     
    if (text == "Light 1off") {bot.sendMessage(chat_id, "Light1 state set to OFF", "");
                              digitalWrite(relay0, LOW);}
    if (text == "/Light1off") {bot.sendMessage(chat_id, "Light1 state set to OFF", "");
                              digitalWrite(relay0,LOW);}
    
    if (text == "Light 2on") {bot.sendMessage(chat_id, "Light2 state set to ON", "");
                             digitalWrite(relay1, HIGH);}
    if (text == "/Light2on") {bot.sendMessage(chat_id, "Light2 state set to ON", "");
                             digitalWrite(relay1, HIGH);}
     
    if (text == "Light 2off") {bot.sendMessage(chat_id, "Light2 state set to OFF", "");
                              digitalWrite(relay1, LOW);}
    if (text == "/Light2off") {bot.sendMessage(chat_id, "Light2 state set to OFF", "");
                              digitalWrite(relay1,LOW);}

      }   
    }