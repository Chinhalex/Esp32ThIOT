#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27,16,2);

#define DHT11_PIN 15
#define DHTTYPE DHT11

// WiFi
const char* ssid = "LQMB";
const char* password = "2444666668888888@";

DHT dht(DHT11_PIN, DHTTYPE);
//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// Insert Firebase project API Key
#define API_KEY "AIzaSyBTJjoUePNXvpBgcCpwQyUhxBlkqI4MZnM"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://th-iot-c314c-default-rtdb.firebaseio.com/" 

// /* 4. Define the user Email and password that alreadey registerd or added in your project */
// #define USER_EMAIL "vovchinh@gmail.com"
// #define USER_PASSWORD "123456789"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

boolean signupOK = false;
String led_state ;


void initLcd()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void initDHT() {
  pinMode(DHT11_PIN, INPUT);
  dht.begin();
}

void initFirebase() {

    
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    /* Assign the api key (required) */
    config.api_key = API_KEY;

    // /* Assign the user sign in credentials */
    // auth.user.email = USER_EMAIL;
    // auth.user.password = USER_PASSWORD;

    /* Assign the RTDB URL (required) */
    config.database_url = DATABASE_URL;
    /* Sign up */
    if (Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("ok");
        signupOK = true;
    }
    else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);

    //Comment or pass false value when WiFi reconnection will control by your code or third party library
    Firebase.reconnectWiFi(true);
    //  if (Firebase.ready())
    //      Firebase.RTDB.setString(&fbdo, F("/LivingRoom/Alarm"), F("ON")) ? "ok" : fbdo.errorReason().c_str();
}

void TurnLight()
{  
     if (Firebase.ready())
      {
        int led_gpio = 2;
        pinMode(led_gpio, OUTPUT);

        String urlDevice = "/led/status";
      
        Firebase.RTDB.getString(&fbdo,urlDevice);
        led_state = fbdo.to<const char *>();
        
        if (led_state == "true") {
        Serial.println(String("ESP32-GPIO is ON"));
        digitalWrite(led_gpio, HIGH);
        }
        else if (led_state == "false") {
        Serial.println(String("ESP32-GPIO is OFF"));
        digitalWrite(led_gpio, LOW);
        }
    
        else {
            Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, F("/led")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
        }
      }
} 
void setup()
{
  Serial.begin(115200);
  
  initWiFi();
  initDHT();
  initLcd();
  initFirebase();

}

void loop()
{
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (Firebase.ready() ) {
    {
        if (Firebase.RTDB.setFloat(&fbdo, "devices/nhietdo", temp)){
          Serial.print("nhiet do: ");
          Serial.println(temp);
        } 
        if (Firebase.RTDB.setFloat(&fbdo, "devices/doam", hum)){
          Serial.print("do am: ");
          Serial.println(hum);
        }
        Serial.println();
      delay(1000);
    }
  }

  lcd.clear();
  // check whether the reading is successful or not
  if (isnan(temp) || isnan(hum)) {
    Serial.println("failed to lcd");
    lcd.setCursor(0, 0);
    lcd.print("Failed");
  } else {
    Serial.println("success to lcd");
    lcd.setCursor(0, 0);  // display position
    lcd.print("Temp: ");
    lcd.print(temp);     // display the temperature
    lcd.print("°C");

    lcd.setCursor(0, 1);  // display position
    lcd.print("Humi: ");
    lcd.print(hum);      // display the humidity
    lcd.print("%");
  }
  TurnLight();
  
}