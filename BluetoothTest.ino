#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define LED 2
#define SERVICE_UUID "84c82c43-dead-beef-9eb9-2dcac4c36ee3"
#define CHARACTERISTICS_UUID_TX "5657fb55-49ba-4cee-80da-654dcdf89af0"
#define CHARACTERISTICS_UUID_RX "5657fb55-49ba-4cee-80da-654dcdf89af0"

BLEService* pService;
BLEServer* pServer;
BLECharacteristic* pTxCharacteristic;
BLECharacteristic* pRxCharacteristic;
BLEAdvertising *pAdvertising;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      blinkWhenConnected();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};


void setup() {
  Serial.begin(115200);
  BLEDevice::init("recStarter");
  pinMode(LED, OUTPUT);
  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pRxCharacteristic = pService->createCharacteristic(CHARACTERISTICS_UUID,
                                                    BLECharacteristic::PROPERTY_WRITE);
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTICS_UUID,
                                                    BLECharacteristic::PROPERTY_NOTIFY);
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Hello World says Neil");
  pTxCharacteristic->addDescriptor(new BLE2902());
  pService->start();
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void blinkWhenConnected(){
  for (int i = 0; i < 3; i++){
    delay(100);
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
  }
}

void loop() {
    if (deviceConnected) {
        pTxCharacteristic->setValue(&txValue, 1);
        pTxCharacteristic->notify();
        txValue++;
		delay(10); // bluetooth stack will go into congestion, if too many packets are sent
	}

      // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        digitalWrite(LED, HIGH);
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
        digitalWrite(LED, LOW);
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
		// do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
