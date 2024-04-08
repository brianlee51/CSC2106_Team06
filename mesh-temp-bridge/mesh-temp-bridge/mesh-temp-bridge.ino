#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "painlessMesh.h"
#include "SSD1306Wire.h"
#include "images.h"
#include "boards.h"

// uncomment out the mode at least one mode (required)
bool FACTORY_MODE = true;
bool CUSTOMER_MODE = !FACTORY_MODE;

#define SHOW_STATS true

#define RH_MESH_MAX_MESSAGE_LEN 255

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

#define   MOVING_AVG_BUFF_SIZE 32

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// In this small artifical network of 4 nodes,
#define BRIDGE_ADDRESS 1 // address of the bridge ( we send our data to, hopefully the bridge knows what to do with our data )

// lilygo T3 v2.1.6
// lora SX1276/8
#define LLG_SCK 5
#define LLG_MISO 19
#define LLG_MOSI 27
#define LLG_CS 18
#define LLG_RST 23
#define LLG_DI0 26
#define LLG_DI1 33
#define LLG_DI2 32

#define LLG_LED_GRN 25

// oled
#define LLG_OLED_SDA 21
#define LLG_OLED_SCL 22

// tfcard
#define LLG_SD_CS 13
#define LLG_SD_MISO 2
#define LLG_SD_MOSI 15
#define LLG_SD_SCK 14

#define LOG_PATH "/sensors.log"

#define RXTIMEOUT 3000 // it is roughly the delay between successive transmissions

#define CLEAR_ROUTING_TABLE_INTERVAL_MS 30000

#ifdef __cplusplus
extern "C"
{
#endif
  uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

const char *ssid = "Pixel_8072";
const char *password = "adafbb6699";

const char *rootCACertificate =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIEyDCCA7CgAwIBAgIQDPW9BitWAvR6uFAsI8zwZjANBgkqhkiG9w0BAQsFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"
    "MjAeFw0yMTAzMzAwMDAwMDBaFw0zMTAzMjkyMzU5NTlaMFkxCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxMzAxBgNVBAMTKkRpZ2lDZXJ0IEdsb2Jh\n"
    "bCBHMiBUTFMgUlNBIFNIQTI1NiAyMDIwIENBMTCCASIwDQYJKoZIhvcNAQEBBQAD\n"
    "ggEPADCCAQoCggEBAMz3EGJPprtjb+2QUlbFbSd7ehJWivH0+dbn4Y+9lavyYEEV\n"
    "cNsSAPonCrVXOFt9slGTcZUOakGUWzUb+nv6u8W+JDD+Vu/E832X4xT1FE3LpxDy\n"
    "FuqrIvAxIhFhaZAmunjZlx/jfWardUSVc8is/+9dCopZQ+GssjoP80j812s3wWPc\n"
    "3kbW20X+fSP9kOhRBx5Ro1/tSUZUfyyIxfQTnJcVPAPooTncaQwywa8WV0yUR0J8\n"
    "osicfebUTVSvQpmowQTCd5zWSOTOEeAqgJnwQ3DPP3Zr0UxJqyRewg2C/Uaoq2yT\n"
    "zGJSQnWS+Jr6Xl6ysGHlHx+5fwmY6D36g39HaaECAwEAAaOCAYIwggF+MBIGA1Ud\n"
    "EwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFHSFgMBmx9833s+9KTeqAx2+7c0XMB8G\n"
    "A1UdIwQYMBaAFE4iVCAYlebjbuYP+vq5Eu0GF485MA4GA1UdDwEB/wQEAwIBhjAd\n"
    "BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwdgYIKwYBBQUHAQEEajBoMCQG\n"
    "CCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wQAYIKwYBBQUHMAKG\n"
    "NGh0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RH\n"
    "Mi5jcnQwQgYDVR0fBDswOTA3oDWgM4YxaHR0cDovL2NybDMuZGlnaWNlcnQuY29t\n"
    "L0RpZ2lDZXJ0R2xvYmFsUm9vdEcyLmNybDA9BgNVHSAENjA0MAsGCWCGSAGG/WwC\n"
    "ATAHBgVngQwBATAIBgZngQwBAgEwCAYGZ4EMAQICMAgGBmeBDAECAzANBgkqhkiG\n"
    "9w0BAQsFAAOCAQEAkPFwyyiXaZd8dP3A+iZ7U6utzWX9upwGnIrXWkOH7U1MVl+t\n"
    "wcW1BSAuWdH/SvWgKtiwla3JLko716f2b4gp/DA/JIS7w7d7kwcsr4drdjPtAFVS\n"
    "slme5LnQ89/nD/7d+MS5EHKBCQRfz5eeLjJ1js+aWNJXMX43AYGyZm0pGrFmCW3R\n"
    "bpD0ufovARTFXFZkAdl9h6g4U5+LXUZtXMYnhIHUfoyMo5tS58aI7Dd8KvvwVVo4\n"
    "chDYABPPTHPbqjc1qCmBaZx2vN4Ye5DUys/vZwP9BFohFrH/6j/f3IL16/RZkiMN\n"
    "JCqVJUzKoZHm1Lesh3Sz8W2jmdv51b2EQJ8HmA==\n"
    "-----END CERTIFICATE-----\n";

unsigned long lastTimeSentApi = 0;
unsigned long clearRoutingTableTime = 0;
bool sendingApi = false;
// Set timer to 10 seconds (10000)
unsigned long timerDelay = 10000;

// Domain name with URL path or IP address with path
const char *serverName = "https://coruscating-starburst-2f8691.netlify.app/.netlify/functions/webhook";

const char device_id[9] = "efd1b9dc";

uint8_t acknowledgeMessage[] = "Hello back from bridge";
// Dont put this on the stack:
uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];
uint8_t res;

File writeLog;

// Singleton instance of the radio driver
RH_RF95 rf95(LLG_CS, LLG_DI0); // slave select pin and interrupt pin, [heltec|ttgo] ESP32 Lora OLED with sx1276/8

// Class to manage message delivery and receipt, using the driver declared above
RHMesh manager(rf95, BRIDGE_ADDRESS);

SSD1306Wire display(0x3c, I2C_SDA, I2C_SCL);
Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
JsonDocument hashmap;
SPIClass sd_spi(HSPI);

int counter = 0;

// User stub
void readTemperature();
void printTemperature();
void printScreen();
void sendToApi();
void writeToSdCard();
void showTopology();
void showStats();

Task taskReadTemp(TASK_SECOND * 1, TASK_FOREVER, &readTemperature);
Task taskPrintTemp(TASK_SECOND * 1, TASK_FOREVER, &printTemperature);
Task taskPrintScreen(TASK_SECOND * 1, TASK_FOREVER, &printScreen);
Task taskSendToApi(TASK_SECOND * 1, TASK_FOREVER, &sendToApi);
Task taskLogToSdCard(TASK_SECOND * 1, TASK_FOREVER, &writeToSdCard);
Task taskShowTopo(TASK_SECOND * 1, TASK_FOREVER, &showTopology);
Task taskShowStats(TASK_SECOND * 1, TASK_FOREVER, &showStats);

void showStats()
{
  Serial.printf("Total heap: %u\n", ESP.getHeapSize());
  Serial.printf("Free heap: %u\n", ESP.getFreeHeap());
  Serial.printf("Total PSRAM: %u\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
}

void showTopology()
{
  if (millis() > clearRoutingTableTime)
  {
    manager.clearRoutingTable();
    clearRoutingTableTime = millis() + CLEAR_ROUTING_TABLE_INTERVAL_MS;
  }

  if (CUSTOMER_MODE)
  {
    String topologyJson = mesh.subConnectionJson();
    Serial.print("::: painlessMesh :::");
    Serial.print(topologyJson);
    Serial.println("::: painlessMesh :::");
  }

  if (FACTORY_MODE)
  {
    // TODO:: show lora topology
    Serial.println("::: RadioHead :::");
    manager.printRoutingTable();
    Serial.println("::: RadioHead :::");
  }
  taskShowTopo.setInterval(TASK_SECOND * 1);
}


void setupSdCard()
{
  // SD Card
  sd_spi.begin(LLG_SD_SCK, LLG_SD_MISO, LLG_SD_MOSI, LLG_SD_CS);

  if (!SD.begin(LLG_SD_CS, sd_spi))
    Serial.println("SD Card: mounting failed.");
  else {
    Serial.println("SD Card: mounted.");
    writeLog = SD.open(LOG_PATH, FILE_WRITE);
    writeLog.close();
  }
}

void writeToSdCard()
{
  writeLog = SD.open(LOG_PATH, FILE_APPEND);
  if (writeLog)
  {
    serializeJson(hashmap, writeLog);
    writeLog.printf("\n");
    writeLog.close();
  }
  else
  {
    Serial.println("Unable to open log file");
  }
}

void sendToApi()
{
  sendingApi = true;
  // Send an HTTP POST request every 5 sec
  if ((millis() - lastTimeSentApi) > timerDelay)
  {
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {
      WiFiClientSecure *client = new WiFiClientSecure;

      if (client)
      {
        client->setCACert(rootCACertificate);
        {
          HTTPClient http;

          http.begin(*client, serverName);

          // to send from jsondocument
          String json;
          serializeJson(hashmap, json);

          http.addHeader("Content-Type", "application/json");
          http.addHeader("Content-Length", String(json.length()));
          http.addHeader("Host", "coruscating-starburst-2f8691.netlify.app");
          Serial.print("To Send:: ");
          Serial.println(json);
          int httpResponseCode = http.POST(json);

          if (httpResponseCode > 0)
          {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String response = http.getString();
            Serial.println(response);
            // clear the jsondocument
            Serial.println("Clear the JsonDocument");
            hashmap.clear();
          }
          else
          {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
            String response = http.getString();
            Serial.println(response);
          }
          // Free resources
          http.end();
        }
      }
      delete client;
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    lastTimeSentApi = millis();
    taskSendToApi.setInterval(TASK_SECOND * 10);
  }
  sendingApi = false;
}

void loraSetup()
{
  Serial.print(F("initializing node "));
  Serial.print(BRIDGE_ADDRESS);
  SPI.begin(LLG_SCK, LLG_MISO, LLG_MOSI, LLG_CS);
  if (!manager.init())
  {
    Serial.println(" init failed");
  }
  else
  {
    Serial.println(" done");
  } // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36

  rf95.setTxPower(2, false); // with false output is on PA_BOOST, power from 2 to 20 dBm, use this setting for high power demos/real usage
  // rf95.setTxPower(1, true); // true output is on RFO, power from 0 to 15 dBm, use this setting for low power demos ( does not work on lilygo lora32 )
  rf95.setFrequency(868.0);
  rf95.setCADTimeout(500);

  // long range configuration requires for on-air time
  boolean longRange = false;
  if (longRange)
  {
    // custom configuration
    RH_RF95::ModemConfig modem_config = {
        0x78, // Reg 0x1D: BW=125kHz, Coding=4/8, Header=explicit
        0xC4, // Reg 0x1E: Spread=4096chips/symbol, CRC=enable
        0x08  // Reg 0x26: LowDataRate=On, Agc=Off.  0x0C is LowDataRate=ON, ACG=ON
    };
    rf95.setModemRegisters(&modem_config);
  }
  else
  {
    // Predefined configurations( bandwidth, coding rate, spread factor ):
    // Bw125Cr45Sf128     Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range
    // Bw500Cr45Sf128     Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range
    // Bw31_25Cr48Sf512   Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range
    // Bw125Cr48Sf4096    Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, low data rate, CRC on. Slow+long range
    // Bw125Cr45Sf2048    Bw = 125 kHz, Cr = 4/5, Sf = 2048chips/symbol, CRC on. Slow+long range
    if (!rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128))
    {
      Serial.println(F("set config failed"));
    }
  }
  Serial.println("RF95 ready");
}

void loraMeshLoop()
{
  uint8_t len = sizeof(buf);
  uint8_t from;
  if (manager.recvfromAck(buf, &len, &from))
  {
    Serial.print("request from node n.");
    Serial.print(from);
    Serial.print(": ");
    Serial.print((char *)buf);
    Serial.print(" rssi: ");
    Serial.println(rf95.lastRssi());


    // we want to decode the message, and parse into jsondocument
    JsonDocument deserialize;
    DeserializationError error = deserializeJson(deserialize, buf);
    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    for (auto p : deserialize.as<JsonObject>())
    {
      hashmap[p.key()] = p.value();
    }

    // Send a reply back to the originator client
    res = manager.sendtoWait(acknowledgeMessage, sizeof(acknowledgeMessage), from);
    if (res != RH_ROUTER_ERROR_NONE)
    {
      Serial.print("sendtoWait failed:");
      Serial.println(res);
    }
  }
}

void printScreen()
{
  // Clear the display
  display.clear();
  display.setFont(ArialMT_Plain_10);

  // Print table header
  display.drawString(0, 0, "+----------+------+-------------------+");
  display.drawString(0, 10, "| Device ID| TempC| BattV |");
  display.drawString(0, 20, "+----------+------+------------------+");

  JsonObject root = hashmap.as<JsonObject>();
  int y = 30; // Initial Y position for data rows
  for (JsonPair kv : root)
  {
    JsonObject devices = kv.value().as<JsonObject>();
    const char *device_id = kv.key().c_str();
    float temperature = devices["temperature"];
    float battery = devices["battery"];

    // Print data row
    char tempBuffer[7];    // Buffer for temperature
    char batteryBuffer[6]; // Buffer for battery percentage
    snprintf(tempBuffer, sizeof(tempBuffer), "%.2f", temperature);
    snprintf(batteryBuffer, sizeof(batteryBuffer), "%.2f", battery);
    display.drawString(0, y, "|");
    display.drawString(5, y, device_id);
    display.drawString(52, y, " |");
    display.drawString(60, y, tempBuffer);
    display.drawString(90, y, " | ");
    display.drawString(100, y, batteryBuffer);
    display.drawString(120, y, " |");
    y += 10; // Increment Y position for next row
  }
  // Print bottom border
  display.drawString(0, y, "+--------------+---------+------------+");
  display.display(); // Update display
  taskPrintScreen.setInterval(TASK_SECOND * 1);
}

void printTemperature()
{
  Serial.println("+------------+-----------------+---------------+");
  Serial.println("| Device ID  | Temperature (C) | Battery (V)   |");
  Serial.println("+------------+-----------------+---------------+");

  JsonObject root = hashmap.as<JsonObject>();
  for (JsonPair kv : root)
  {
    JsonObject devices = kv.value().as<JsonObject>();
    const char* device_id = kv.key().c_str();
    float temperature = devices["temperature"];
    float battery = devices["battery"];
    Serial.printf("| %10s | %15.2f | %13.2f |\n", device_id, temperature, battery);
  }

  Serial.println("+------------+-----------------+---------------+");
  taskPrintTemp.setInterval(TASK_SECOND * 1);
}

/*
    compute a moving average of temperature data
    and then broadcast the moving average value to all nodes
*/

void readTemperature()
{
  // static type to make the variables persist across runs.
  // data buffer will be set in a circular fashion with index, e.g.
  // 0,1,2...9,0,1,2...9
  static float data[MOVING_AVG_BUFF_SIZE] = {0};
  static int index = 0;
  static int count = 0;

  float sum = 0;
  float moving_avg_celsius = 0;
  // float result = random(0, 3000) / 100.0; // Generate a random integer between 0 and 3000, then divide by 100.0 to get a float between 0 and 30
  float result = (temprature_sens_read() - 32) / 1.8;
  float battery = 0;

  // Map integer to float between 4.0 and 5.0
  battery = map(random(0, 1000), 0, 1000, 400, 500) / 100.0;

  sum -= data[index];
  data[index] = result;
  sum += data[index];

  // index can be from 0 to MOVING_AVG_BUFF_SIZE - 1
  index = (index + 1) % MOVING_AVG_BUFF_SIZE;

  // once count reaches (for example), 10, it wont decrease, since it will store the last 10 data points.
  if (count < MOVING_AVG_BUFF_SIZE)
  {
    count++;
  }

  moving_avg_celsius = sum / count;

  // we want to update the jsondocument with for this device.
  hashmap[device_id]["temperature"] = result;
  hashmap[device_id]["battery"] = battery;

  String output;
  serializeJson(hashmap, output);
  mesh.sendBroadcast(output);
  taskReadTemp.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
}

// Needed for painless library
// other nodes will be broadcasting their json data structure back to us
// we need to deserialize and store it into some array of structs.
// it should potentially be able to store inside eeprom
void receivedCallback(uint32_t from, String &msg)
{
  Serial.print("receivedCallback:: ");
  Serial.println(msg);
  JsonDocument deserialize;
  DeserializationError error = deserializeJson(deserialize, msg);
  // Test if parsing succeeds.
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  for (auto p : deserialize.as<JsonObject>()) 
  {
    hashmap[p.key()] = p.value();
  }
}

// Whenever there is a new node detected the node will broadcast the jsondocument
void newConnectionCallback(uint32_t nodeId) {
  String output;
  serializeJson(hashmap, output);
  mesh.sendBroadcast(output);
  // Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  // Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  // Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}


void setup() {
  initBoard();

  Serial.begin(115200);

  // setup sd card
  setupSdCard();
  
  randomSeed(analogRead(0)); // Seed the random number generator

  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  userScheduler.addTask( taskReadTemp );
  userScheduler.addTask( taskPrintTemp );
  userScheduler.addTask( taskPrintScreen );
  userScheduler.addTask( taskLogToSdCard );
  userScheduler.addTask( taskShowTopo );
  
  taskReadTemp.enable();
  taskPrintTemp.enable();
  taskPrintScreen.enable();
  taskLogToSdCard.enable();
  taskShowTopo.enable();

  mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  // mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );

  if (CUSTOMER_MODE)
  {
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  }

  if (FACTORY_MODE)
  {
    loraSetup();
    userScheduler.addTask( taskSendToApi );
    taskSendToApi.enable();
  }

#ifdef SHOW_STATS
  userScheduler.addTask(taskShowStats);
  taskShowStats.enable();
#endif
}

void loop() {
  // maybe have a toggle to check when to update the mesh
  // it will run the user scheduler as well
  if (!sendingApi) 
  {
    mesh.update();
  }

  if (FACTORY_MODE)
  {
    loraMeshLoop();
  }
}
