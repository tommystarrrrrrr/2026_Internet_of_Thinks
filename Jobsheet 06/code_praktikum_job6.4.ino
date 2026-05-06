// =================================================================
// 1. SAKLAR FITUR (WAJIB DI PALING ATAS, SEBELUM INCLUDE)
// =================================================================
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

// =================================================================
// 2. LIBRARY UTAMA
// =================================================================
#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>
#include "DHT.h"

// =================================================================
// 3. KONFIGURASI PIN & KONEKSI
// =================================================================
#define DHTPIN 4           // DHT11 ke GPIO 4
#define DHTTYPE DHT11 
const int led1 = 12;       // LED ke GPIO 12

#define WIFI_SSID "Asalole 4G"
#define WIFI_PASSWORD "21032006"

// API Key & URL dari Firebase Console
#define API_KEY "AIzaSyCG22aiF9ZStITJQs3HsarrzzSfu0BgosA"
#define USER_EMAIL "bintang@gmail.com"
#define USER_PASSWORD "12345678"
#define DATABASE_URL "https://esiot2026-556e3-default-rtdb.asia-southeast1.firebasedatabase.app/"

// =================================================================
// 4. OBJEK & VARIABLE GLOBAL
// =================================================================
void processData(AsyncResult &aResult);
UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);
FirebaseApp app;
WiFiClientSecure ssl_client1, ssl_client2;
using AsyncClient = AsyncClientClass;
AsyncClient async_client1(ssl_client1), async_client2(ssl_client2);
RealtimeDatabase Database;
AsyncResult dbResult; // Menampung hasil baca dari Firebase
DHT dht(DHTPIN, DHTTYPE);

unsigned long sensorMillis = 0;
unsigned long ledMillis = 0;

// =================================================================
// 5. SETUP
// =================================================================
void setup() {
    Serial.begin(115200);
    pinMode(led1, OUTPUT);
    digitalWrite(led1, LOW); 

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println("\nConnected with IP: " + WiFi.localIP().toString());

    ssl_client1.setInsecure();
    ssl_client2.setInsecure();

    // Inisialisasi Firebase
    initializeApp(async_client1, app, getAuth(user_auth), processData, "authTask");
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);
    
    dht.begin();
    Serial.println(">>> Sistem IoT Siap Tempur!");
}

// =================================================================
// 6. LOOP UTAMA
// =================================================================
void loop() {
    app.loop(); // Menjaga koneksi tetap hidup

    // --- BAGIAN KONTROL LED (Cek setiap 0.5 detik) ---
    if (millis() - ledMillis > 500) {
        ledMillis = millis();
        if (app.ready()) {
            // Ambil data status led dari path: /esiot-db/led
            Database.get(async_client2, "/esiot-db/led", dbResult);
        }
    }

    // Cek apakah ada data baru datang dari Firebase
    if (dbResult.available()) {
        String payload = dbResult.c_str();
        payload.replace("\"", ""); // Hapus tanda petik jika ada
        
        Serial.print("Status LED di Firebase: ");
        Serial.println(payload);

        if (payload == "1") {
            digitalWrite(led1, HIGH);
        } else {
            digitalWrite(led1, LOW);
        }
    }

    // --- BAGIAN MONITORING SENSOR (Cek setiap 5 detik) ---
    if (millis() - sensorMillis > 5000) {
        sensorMillis = millis();
        if (app.ready()) {
            float h = dht.readHumidity();
            float t = dht.readTemperature();

            if (!isnan(h) && !isnan(t)) {
                Serial.printf("Update Cloud -> T: %.2f, H: %.2f\n", t, h);
                Database.set<float>(async_client1, "/esiot-db/suhu", t, processData, "suhuTask");
                Database.set<float>(async_client1, "/esiot-db/kelembapan", h, processData, "humTask");
            } else {
                Serial.println("Gagal baca sensor!");
            }
        }
    }
}

// =================================================================
// 7. FUNGSI LOGGING (Untuk Debugging)
// =================================================================
void processData(AsyncResult &aResult) {
    if (!aResult.isResult()) return;
    if (aResult.isError()) {
        Serial.printf("Error: %s (Code: %d)\n", aResult.error().message().c_str(), aResult.error().code());
    }
}