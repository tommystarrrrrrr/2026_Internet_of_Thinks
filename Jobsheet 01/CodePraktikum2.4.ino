#include <WiFi.h>

// Konfigurasi WiFi sesuai data Anda
const char* ssid     = "realme 10";
const char* password = "qwerty45678";

// Inisialisasi Server pada Port 80
WiFiServer server(80);

// Definisi Pin (Menggunakan GPIO 12, 13, 14, 27)
const int lampu1 = 12;
const int lampu2 = 13;
const int lampu3 = 14;
const int lampu4 = 27;

void setup() {
  Serial.begin(115200);
  
  // Mengatur semua pin sebagai OUTPUT
  pinMode(lampu1, OUTPUT);
  pinMode(lampu2, OUTPUT);
  pinMode(lampu3, OUTPUT);
  pinMode(lampu4, OUTPUT);
  
  // Pastikan kondisi awal semua lampu mati (LOW)
  digitalWrite(lampu1, LOW);
  digitalWrite(lampu2, LOW);
  digitalWrite(lampu3, LOW);
  digitalWrite(lampu4, LOW);

  // Proses menyambungkan ke WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  
  // Memulai Server
  server.begin();
  Serial.println("Server started");

  // Menampilkan IP Address
  Serial.print("Gunakan URL ini: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  // Mengecek apakah ada klien yang terhubung
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Menunggu data dari klien tersedia
  Serial.println("New Client");
  while(!client.available()){
    delay(1);
  }

  // Membaca baris pertama dari permintaan (request)
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Logika kontrol berdasarkan permintaan dari browser
  if (request.indexOf("/light1on") != -1)  { digitalWrite(lampu1, HIGH); }
  if (request.indexOf("/light1off") != -1) { digitalWrite(lampu1, LOW); }
  if (request.indexOf("/light2on") != -1)  { digitalWrite(lampu2, HIGH); }
  if (request.indexOf("/light2off") != -1) { digitalWrite(lampu2, LOW); }
  if (request.indexOf("/light3on") != -1)  { digitalWrite(lampu3, HIGH); }
  if (request.indexOf("/light3off") != -1) { digitalWrite(lampu3, LOW); }
  if (request.indexOf("/light4on") != -1)  { digitalWrite(lampu4, HIGH); }
  if (request.indexOf("/light4off") != -1) { digitalWrite(lampu4, LOW); }

  // Mengirimkan Response HTML ke browser
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>Kontrol LED WiFi</title></head>");
  client.println("<body style='text-align:center;'>");
  client.println("<h1>Praktikum 2.4: Kontrol 4 LED</h1>");
  
  // Membuat tabel kontrol sesuai format praktikum
  client.println("<table border='1' align='center' cellpadding='10'>");
  client.println("<tr><th>Perangkat</th><th>Aksi</th></tr>");
  
  // Baris Lampu 1
  client.println("<tr><td>Lampu 1</td>");
  client.println("<td><a href='/light1on'><button>Turn On</button></a> ");
  client.println("<a href='/light1off'><button>Turn Off</button></a></td></tr>");
  
  // Baris Lampu 2
  client.println("<tr><td>Lampu 2</td>");
  client.println("<td><a href='/light2on'><button>Turn On</button></a> ");
  client.println("<a href='/light2off'><button>Turn Off</button></a></td></tr>");
  
  // Baris Lampu 3
  client.println("<tr><td>Lampu 3</td>");
  client.println("<td><a href='/light3on'><button>Turn On</button></a> ");
  client.println("<a href='/light3off'><button>Turn Off</button></a></td></tr>");
  
  // Baris Lampu 4
  client.println("<tr><td>Lampu 4</td>");
  client.println("<td><a href='/light4on'><button>Turn On</button></a> ");
  client.println("<a href='/light4off'><button>Turn Off</button></a></td></tr>");
  
  client.println("</table>");
  client.println("</body>");
  client.println("</html>");

  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}