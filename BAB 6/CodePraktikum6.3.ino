// Tambahkan library DHT di bagian atas
#include "DHT.h"

#define DHTPIN 4          // Pin data DHT terhubung ke GPIO 4
#define DHTTYPE DHT11     // Ubah menjadi DHT22 jika Anda menggunakan DHT22

// Step 1
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

// For ESP32
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// Step 2
void asyncCB(AsyncResult &aResult);
void processData(AsyncResult &aResult);

const int led1 = 12; 

#define ON HIGH
#define OFF LOW

#define WIFI_SSID "Asalole 4G"
#define WIFI_PASSWORD "21032006"

#define API_KEY "AIzaSyCG22aiF9ZStITJQs3HsarrzzSfu0BgosA"
#define USER_EMAIL "bintang@gmail.com"
#define USER_PASSWORD "12345678"
#define DATABASE_URL "https://esiot2026-556e3-default-rtdb.asia-southeast1.firebasedatabase.app"

// Step 3
UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

// Step 4
FirebaseApp app;

// Step 5
// Use two SSL clients for sync and async tasks for demonstation only.
WiFiClientSecure ssl_client1, ssl_client2;

// Step 6
// Use two AsyncClients for sync and async tasks for demonstation only.
using AsyncClient = AsyncClientClass;
AsyncClient async_client1(ssl_client1), async_client2(ssl_client2);

// Step 7
RealtimeDatabase Database;

bool onetimeTest = false;

// The Optional proxy object that provides the data/information
//  when used in async mode without callback.
AsyncResult dbResult;


DHT dht(DHTPIN, DHTTYPE);


void setup() {
    Serial.begin(115200);

    pinMode(led1, OUTPUT);
    // Memastikan semua LED mati di awal
    digitalWrite(led1, OFF);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    // The SSL client options depend on the SSL client used.

    // Skip certificate verification (Optional)
    ssl_client1.setInsecure();
    ssl_client2.setInsecure();

    // Set timeout for ESP32 core sdk v3.x. (Optional)
    ssl_client1.setConnectionTimeout(2000);
    ssl_client1.setHandshakeTimeout(10);
    ssl_client2.setConnectionTimeout(2000);
    ssl_client2.setHandshakeTimeout(10);

    // Step 8
    initializeApp(async_client1, app, getAuth(user_auth), processData, "🔐 authTask");

    // Step 9
    app.getApp<RealtimeDatabase>(Database);

    // Step 10
    Database.url(DATABASE_URL);
    
    dht.begin(); // Inisialisasi sensor DHT
}

unsigned long lastMillis = 0;

void loop() {
    app.loop();
    processData(dbResult);

    if (millis() - lastMillis > 5000) 
    {
        lastMillis = millis();

        if (app.ready())
        {
            // --- MEMBACA SENSOR ASLI ---
            float h = dht.readHumidity();
            float t = dht.readTemperature();

            // Cek jika sensor gagal terbaca
            if (isnan(h) || isnan(t)) {
                Serial.println("Gagal membaca sensor DHT!");
                return;
            }

            Serial.printf("Mengirim data asli... Suhu: %.2f°C, Kelembapan: %.2f%%\n", t, h);

            // Kirim data asli ke Firebase
            Database.set<float>(async_client1, "/esiot-db/suhu", t, processData, "suhuTask");
            Database.set<float>(async_client1, "/esiot-db/kelembapan", h, processData, "kelembapanTask");

            // --- KONTROL LED ---
            String value = Database.get<String>(async_client2, "/esiot-db/led");
            if(value == "1") digitalWrite(led1, ON);
            else digitalWrite(led1, OFF);
        }
    }
}

void processData(AsyncResult &aResult)
{
    // Exits when no result is available when calling from the loop.
    if (!aResult.isResult())
        return;

    if (aResult.isEvent())
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());

    if (aResult.isDebug())
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

    if (aResult.isError())
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

    if (aResult.available())
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
}
