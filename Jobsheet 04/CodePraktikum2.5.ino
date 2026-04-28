#include <WiFi.h>

// Konfigurasi WiFi Anda
const char* ssid     = "POCO F7";
const char* password = "Afkarbaik";

// Inisialisasi Server pada Port 80
WiFiServer server(80);

// Pin LED yang digunakan (Contoh menggunakan Pin 13)
const int ledPin = 13;

void setup() {
  Serial.begin(115200);
  delay(10);

  // Menyiapkan Pin LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Proses koneksi ke WiFi
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Terhubung.");
  
  // Memulai Server
  server.begin();
  Serial.println("Server telah berjalan");

  // Menampilkan Alamat IP
  Serial.print("Alamat IP ESP32: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Mengecek apakah ada klien (browser) yang terhubung
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Menunggu data dari klien
  Serial.println("Klien Baru terdeteksi.");
  while(!client.available()){
    delay(1);
  }

  // Membaca baris pertama permintaan HTTP
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Logika Pengendalian LED sesuai isi Praktikum 2.5
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(ledPin, LOW);
    value = LOW;
  }

  // Mengirimkan Response (Halaman Web) ke Browser
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); // Baris kosong wajib setelah header
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>ESP32 Web Server</title></head>");
  client.println("<body style='text-align:center; font-family: Arial;'>");
  
  client.println("<h1>Kontrol LED Tunggal</h1>");
  
  // Menampilkan Status LED saat ini
  client.print("Status LED sekarang: ");
  if(digitalRead(ledPin) == HIGH) { 
  client.print("<strong>NYALA</strong>");
} else {
  client.print("<strong>MATI</strong>");
}

  // Membuat Tombol Kontrol
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"><button style='width:100px; height:40px;'>Turn On</button></a>");
  client.println("<a href=\"/LED=OFF\"><button style='width:100px; height:40px;'>Turn Off</button></a>");
  
  client.println("</body>");
  client.println("</html>");

  delay(1);
  Serial.println("Klien terputus.");
  Serial.println("");
}