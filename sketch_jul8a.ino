// // #include "esp32-hal-bt.h"
// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLEUtils.h>
// #include <BLE2902.h>

// #define SERVICE_UUID "02040608-0204-0204-0204-020406081012"
// #define CHAR_UUID "02040608-0204-0204-0204-020406081013"

// #define LED_PIN 23


// class LEDCallback : public BLECharacteristicCallbacks{
//   public:
//     void onWrite(BLECharacteristic *c) override{
//       String v = String(c->getValue().c_str());
//       if (v == "1"){
//         digitalWrite(LED_PIN, 1);
//         Serial.println("on");
//       } else {
//         digitalWrite(LED_PIN, 0);
//         Serial.println("off");
//       }
//     }
// };


// class ServerCallback : public BLEServerCallbacks {
//   void onConnect(BLEServer* server) override {
//     Serial.println("Client connected");
//   }

//   void onDisconnect(BLEServer* server) override {
//     Serial.println("Client disconnected");

//     digitalWrite(LED_PIN, LOW);  // turn off LED

//     delay(100);

//     server->getAdvertising()->start();
//     Serial.println("Advertising restarted");
//   }
// };


// void setup(){
//   Serial.begin(115200);
//   delay(1000);

//   Serial.println("starting");

//   pinMode(LED_PIN, OUTPUT);
//   Serial.println("pin ready");

//   BLEDevice::init("a");
//   Serial.println("BLE initialized");

//   BLEServer *server = BLEDevice::createServer();
//   Serial.println("server created");

//   server->setCallbacks(new ServerCallback());

//   BLEService *service = server->createService(SERVICE_UUID);
//   Serial.println("service created");

//   BLECharacteristic *led = service->createCharacteristic(
//       CHAR_UUID,
//       BLECharacteristic::PROPERTY_READ |
//       BLECharacteristic::PROPERTY_WRITE
//   );
//   Serial.println("characteristic created");

//   led->setValue("0");
//   Serial.println("value set");

//   led->setCallbacks(new LEDCallback());
//   Serial.println("callback set");

//   service->start();
//   Serial.println("service started");

//   BLEAdvertising *adv = BLEDevice::getAdvertising();
//   Serial.println("advertising object");

//   adv->addServiceUUID(SERVICE_UUID);
//   adv->start();

//   Serial.println("a-ing");
// }

// void loop(){
//   delay(1000);
// }


#include <BLEDevice.h>

#define SERVICE_UUID "02040608-0204-0204-0204-020406081012"
#define CHAR_UUID    "02040608-0204-0204-0204-020406081013"

#define LED_PIN 23
#define BUTTON 22

bool connected = false;

BLEClient *client;
BLERemoteCharacteristic *ledChar;

void setup() {
    Serial.begin(115200);

    BLEDevice::init("a");
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);


    BLEScan *scan = BLEDevice::getScan();
    BLEScanResults *results = scan->start(10);
    
    for (int i = 0; i < results->getCount(); i++) {
        BLEAdvertisedDevice device = results->getDevice(i);
    
        if (device.haveServiceUUID() &&
            device.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
    
            BLEAddress address = device.getAddress();
    
            client = BLEDevice::createClient();
            if (client->connect(address)) {
                Serial.println("connected");
                connected = true;
                digitalWrite(LED_PIN, 1);
            } else {
                Serial.println("connection failed");
                return;
            }
            break;
        }
    }

    BLERemoteService *service = client->getService(SERVICE_UUID);
    if (service == nullptr) {
        Serial.println("Service not found");
        return;
    }

    ledChar = service->getCharacteristic(CHAR_UUID);
    if (ledChar == nullptr) {
        Serial.println("Characteristic not found");
        return;
    }
  
    // ledChar->writeValue("1");   // LED on
    Serial.println("on");
    delay(4000);
    // ledChar->writeValue("0");
}

bool lastButton = HIGH;

void loop() {
  bool currentButton = digitalRead(BUTTON);

  if (currentButton != lastButton && connected) {
    if (currentButton == LOW) {
      ledChar->writeValue("1");
    } else {
      ledChar->writeValue("0");
    }
  }

  lastButton = currentButton;
  delay(50);
}

