#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <Wire.h> // Only needed for Arduino 1.6.5 and earlier
#include <ArduinoJson.h>
#include "painlessMesh.h"
#include "SSD1306Wire.h"
#include "images.h"
#include "boards.h"

// Lora Mesh Defines
// In this small artifical network of 4 nodes,
// address of the bridge ( we send our data to, hopefully the bridge knows what to do with our data )
#define BRIDGE_ADDRESS 1
// address of this node
#define NODE_ADDRESS 2

// Wifi Mesh Defines
#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

#define MOVING_AVG_BUFF_SIZE 32

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

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

#define TXINTERVAL 10000 // delay between successive transmissions

#ifdef __cplusplus
extern "C"
{
#endif
    uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

uint8_t temprature_sens_read();

#if NODE_ADDRESS == 3
    const char device_id[9] = "52b6abfd";
#endif

#if NODE_ADDRESS == 2
    const char device_id[9] = "c04d1ae3";
#endif

// uint8_t data[] = "Hello World!";
// Dont put this on the stack:
uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];
uint8_t res;


unsigned long nextTxTime;

// Singleton instance of the radio driver
RH_RF95 rf95(LLG_CS, LLG_DI0); // slave select pin and interrupt pin, [heltec|ttgo] ESP32 Lora OLED with sx1276/8

// Class to manage message delivery and receipt, using the driver declared above
RHMesh manager(rf95, NODE_ADDRESS);


SSD1306Wire display(0x3c, I2C_SDA, I2C_SCL);
Scheduler userScheduler; // to control your personal task
painlessMesh mesh;
JsonDocument hashmap;

int counter = 0;

// User stub
void readTemperature();
void printTemperature();
void printScreen();

Task taskReadTemp(TASK_SECOND * 1, TASK_FOREVER, &readTemperature);
Task taskPrintTemp(TASK_SECOND * 1, TASK_FOREVER, &printTemperature);
Task taskPrintScreen(TASK_SECOND * 1, TASK_FOREVER, &printScreen);

void loraMeshSetup()
{
    Serial.print(F("initializing node "));
    Serial.print(NODE_ADDRESS);
    SPI.begin(LLG_SCK, LLG_MISO, LLG_MOSI, LLG_CS);
    if (!manager.init())
    {
        Serial.println(" init failed");
    }
    else
    {
        Serial.println(" done");
    } // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36

    rf95.setTxPower(10, false); // with false output is on PA_BOOST, power from 2 to 20 dBm, use this setting for high power demos/real usage
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
    nextTxTime = millis();
}

void loraMeshLoop()
{
    // send message every TXINTERVAL millisecs
    if (millis() > nextTxTime)
    {
        String jsonString;
        serializeJson(hashmap, jsonString);
        nextTxTime += TXINTERVAL;
        Serial.print("Sending to bridge n.");
        Serial.print(BRIDGE_ADDRESS);
        Serial.print(" res=");

        uint8_t* data = (uint8_t*)jsonString.c_str();
        // Send a message to a rf95_mesh_server
        // A route to the destination will be automatically discovered.
        // We should send the serialized jsondocument to the bridge node
        res = manager.sendtoWait(data, strlen(jsonString.c_str()), BRIDGE_ADDRESS);

        Serial.println(res);
        if (res == RH_ROUTER_ERROR_NONE)
        {
            // Data has been reliably delivered to the next bridge address.
            // we should dequeue the jsondocument (or just clear it)
            hashmap.clear();
        }
        else
        {
            // Data not delivered to the next node.
            Serial.println("sendtoWait failed. Are the bridge/intermediate mesh nodes running?");
        }
    }

    // radio needs to stay always in receive mode ( to process/forward messages )
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
        Serial.print("message from node n.");
        Serial.print(from);
        Serial.print(": ");
        Serial.print((char *)buf);
        Serial.print(" rssi: ");
        Serial.println(rf95.lastRssi());
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
        const char *device_id = kv.key().c_str();
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
    float result = random(0, 3000) / 100.0; // Generate a random integer between 0 and 3000, then divide by 100.0 to get a float between 0 and 30
    // float result = (temprature_sens_read() - 32) / 1.8;

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
void newConnectionCallback(uint32_t nodeId)
{
    String output;
    serializeJson(hashmap, output);
    mesh.sendBroadcast(output);
    // Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
    // Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
    // Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup()
{
    initBoard();

    Serial.begin(115200);

    loraMeshSetup();

    randomSeed(analogRead(0)); // Seed the random number generator

    // Initialising the UI will init the display too.
    display.init();

    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);

    mesh.setDebugMsgTypes(ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE); // all types on
    // mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    userScheduler.addTask(taskReadTemp);
    userScheduler.addTask(taskPrintTemp);
    userScheduler.addTask(taskPrintScreen);
    taskReadTemp.enable();
    taskPrintTemp.enable();
    taskPrintScreen.enable();
}

void loop()
{
    // it will run the user scheduler as well
    mesh.update();
    loraMeshLoop();
}
