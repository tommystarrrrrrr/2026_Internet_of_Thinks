// Definisi nomor pin
const int lamp1 = 12;


void setup() {
  // Mengatur semua pin sebagai OUTPUT
  pinMode(lamp1, OUTPUT);

}

void loop() {
  // Menyalakan semua lampu
  digitalWrite(lamp1, HIGH);
  
  
  // Tunggu selama 1 detik
  delay(1000);

  // Mematikan semua lampu
  digitalWrite(lamp1, LOW);

  
  // Tunggu selama 1 detik
  delay(1000);
}