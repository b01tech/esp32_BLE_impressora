#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEClient.h>

#define SCAN_TIME 5 // Tempo de escaneamento em segundos

// UUIDs da impressora (devem ser substituídos pelos valores específicos da sua impressora)
const char *printerServiceUUID = "UUID_DO_SERVICO_DA_IMPRESSORA";
const char *printerCharacteristicUUID = "UUID_DA_CARACTERISTICA_DA_IMPRESSORA";

// Ponteiro global para o cliente BLE
BLEClient *pClient;

void connectToPrinter(BLEAdvertisedDevice advertisedDevice) {
    Serial.println("Conectando à impressora...");
    pClient = BLEDevice::createClient();
    pClient->connect(advertisedDevice);

    if (pClient->isConnected()) {
        Serial.println("Conectado à impressora!");

        // Obter serviço da impressora
        BLERemoteService *pRemoteService = pClient->getService(printerServiceUUID);
        if (pRemoteService != nullptr) {
            // Obter característica de impressão
            BLERemoteCharacteristic *pRemoteCharacteristic = pRemoteService->getCharacteristic(printerCharacteristicUUID);
            if (pRemoteCharacteristic != nullptr) {
                Serial.println("Característica encontrada! Enviando dados...");

                // Enviar dados de impressão (por exemplo, texto)
                std::string printData = "Hello, BLE Printer!";
                pRemoteCharacteristic->writeValue(printData);
                Serial.println("Dados enviados com sucesso!");
            } else {
                Serial.println("Característica não encontrada.");
            }
        } else {
            Serial.println("Serviço não encontrado.");
        }
    } else {
        Serial.println("Falha na conexão com a impressora.");
    }

    pClient->disconnect();
    Serial.println("Desconectado.");
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        Serial.print("Dispositivo encontrado: ");
        Serial.println(advertisedDevice.toString().c_str());

        // Verificar se é a impressora que queremos (pelo nome ou UUID)
        if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(printerServiceUUID))) {
            BLEDevice::getScan()->stop(); // Parar escaneamento
            connectToPrinter(advertisedDevice);
        }
    }
}

void setup() {
    Serial.begin(115200);
    BLEDevice::init("ESP32_BLE_Client");

    // Configuração do escaneamento BLE
    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(SCAN_TIME, false);
}

void loop() {
    
}
