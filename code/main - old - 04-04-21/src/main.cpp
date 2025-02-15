#include <arduino.h>
#include <stdio.h>
//#include <ESP8266WiFi.h>
//#include <ESPAsyncTCP.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <FS.h>
#include <Wire.h>
#include <iostream>
#include <string>
#include <keypad.h>


// Replace with your network credentials

const char *ssid = "ASUS1424";
const char *password = "MaJaNe14245.";

//const char *ssid = "Tesla IoT";
//const char *password = "fsL6HgjN";

//const char *ssid = "LaptopieVanSander";
//const char *password = "KrijgsheerSander";

//const char *ssid = "lenovolaptop";
//const char *password = "jarno123";

//--------- Setup keypad----
const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
//byte rowPins[ROWS] = {26, 25, 33, 32}; 
//byte colPins[COLS] = {35, 34, 39, 36}; 
byte rowPins[ROWS] = {23, 22, 3, 21}; 
byte colPins[COLS] = {19, 18, 16, 17};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
//----------End keypad setUp

int page;

using namespace std;  

//dummy variables:
String dummyPasscode = "7777";
String dummyTempPasscode;
String x;
char g;
const int tempBtn = 4;
// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//Variables
// variables to return to the GUI website
char navigationKey;
bool abortCheck = false;
bool rfidCheck = false;
int passcodeCheck = 0;
String lastName = "Vuijk";
bool withdrawCheck = false;
bool balanceCheck = false;


//--------------------------------------String return functions for the webserver to switch pages

String getAbortCheck(){
	bool tempAbortCheck = abortCheck;
	abortCheck = false;
	return String(tempAbortCheck);
}
String getNavigation(){
	/*

	String tempNavigationKey;
	switch (navigationKey)
	{
	case 1:
		tempNavigationKey = 1;
		break;
	case 4:
		tempNavigationKey = 4;
		break;
	case 7:
		tempNavigationKey = 7;
		break;
	case '*':
		tempNavigationKey = 10;
		break;
	case 'A':
		tempNavigationKey = 11;
		break;
	case 'B':
		tempNavigationKey = 12;
		break;
	case 'C':
		tempNavigationKey = 13;
		break;
	default:
		break;
	}
	navigationKey = '0';
	Serial.println(tempNavigationKey);
	if(tempNavigationKey == NULL){
		tempNavigationKey = 0;
	}
	*/
	int tempNavigationKey = 0;

	return String(tempNavigationKey);
}
String getRfidCheck(){
	bool tempAbortCheck = abortCheck;
	abortCheck = false;
	return String(tempAbortCheck);
	
}
String getPasscodeCheck(){
	Serial.println("loc2");
	int tempPasscodeCheck = passcodeCheck;
	passcodeCheck = 0;
	return String(tempPasscodeCheck);
}
String getLastName(){
	return lastName;
}
String getWithdrawCheck(){
	bool tempWithdrawCheck = withdrawCheck;
	withdrawCheck = false;
	return String(tempWithdrawCheck);
}
String getBalanceCheck(){
	bool tempBalanceCheck = balanceCheck;
	balanceCheck = false;
	return String(tempBalanceCheck);
}

/*
// ONLY FOR START-UP I THINK
String processor(const String &var){
	Serial.println(var);
	if (var == "STATE")
	{
		if (digitalRead(ledPin))
		{
			ledState = "ON";
		}
		else
		{
			ledState = "OFF";
		}
		Serial.print(ledState);
		return ledState;
	}

}*/

void setup(){
	// Serial port for debugging purposes
	Serial.begin(115200);
	pinMode(ledPin, OUTPUT);
	pinMode(tempBtn, OUTPUT);

	// Initialize SPIFFS
	if (!SPIFFS.begin())
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}

	// Connect to Wi-Fi
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.println("Connecting to WiFi..");
	}

	// Print ESP32 Local IP Address
	Serial.println(WiFi.localIP());

	server.serveStatic("/index", SPIFFS, "index.html");
	server.serveStatic("/passcode", SPIFFS, "passcode.html");
	server.serveStatic("/mainMenu", SPIFFS, "mainMenu.html");
	server.serveStatic("/balance", SPIFFS, "balance.html");
	server.serveStatic("/withdraw", SPIFFS, "withdraw.html");

	// Route for root / web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/index.html", String(), false);
	});
	// Route to load style.css file
	server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/style.css", "text/css");
	});
	server.on("/navigation", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getNavigation().c_str());
		page = 2;
	});
	// route to abort the transaction processes and return to index
	server.on("/abort", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getAbortCheck().c_str());
	});
	server.on("/rfidCheck", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getRfidCheck().c_str());		
	});
	server.on("/passcodeCheck", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getPasscodeCheck().c_str());
		page = 1;
	});
	server.on("/witdrawCheck", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getBalanceCheck().c_str());
	});
	server.on("/balanceCheck", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getWithdrawCheck().c_str());
	});
	/*
	server.on("/passcodeLenght", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getPasscodeLenght().c_str());
	});
	
	server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getHumidity().c_str());
	});

	server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getPressure().c_str());
	});
	*/
	// Start server
	
	server.begin();
}

void loop(){
	/**/
	char customKey = customKeypad.getKey();
  	if (customKey){
			switch (page)
		  	{
		  	case 1:
				if(dummyTempPasscode.length() < 4){
					dummyTempPasscode += customKey;
					Serial.println(dummyTempPasscode);
				}if(dummyTempPasscode == dummyPasscode){
					passcodeCheck = 1;
					Serial.println("loc1");
				}
			  	break;
		  	case 2:
				navigationKey = customKey;
			  	break;
		  	}
	
  	}


	switch (customKey)
	{
	case 'A':
		/* code */
		break;
	
	default:
		break;
	}

	
}