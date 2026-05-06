# Dokumen Spesifikasi Teknis

**Nama Proyek:** Aplikasi ESIoT Monitor & Kontrol  

**Deskripsi:**  
Aplikasi mobile (Android) untuk memantau data suhu dan kelembapan secara real-time serta memberikan fitur kontrol untuk menghidupkan atau mematikan LED dari jarak jauh.

---

## 1. Fitur Inti

### 🔐 Autentikasi Latar Belakang (Background Auth)
Aplikasi melakukan proses login secara otomatis menggunakan kredensial email dan password yang sudah tertanam di dalam variabel sistem (tanpa antarmuka formulir login) untuk memenuhi aturan keamanan Firebase.

### 📡 Pemantauan Real-time
Menampilkan data suhu dan kelembapan yang akan diperbarui secara otomatis setiap kali ada perubahan data di perangkat IoT (Firebase Realtime Database).

### 💡 Kontrol Toggle LED
Sebuah tombol interaktif yang, ketika ditekan, akan membaca status LED saat ini di database dan membaliknya:

- Jika `0` menjadi `1`
- Jika `1` menjadi `0`

---

## 2. Alur Pengguna (User Flow)

1. Pengguna membuka aplikasi.  
2. Aplikasi secara tak kasat mata melakukan autentikasi ke Firebase menggunakan variabel email dan password.  
3. Setelah autentikasi berhasil, aplikasi menghubungkan diri (`listen`) ke titik data sensor dan kontrol di Firebase.  
4. Layar utama menampilkan nilai suhu dan kelembapan terkini, serta status LED.  
5. Pengguna menekan tombol **"Ubah Status LED"**.  
6. Aplikasi mengirimkan perintah pembaruan nilai ke Firebase.  
7. Lampu (di sisi perangkat IoT keras) akan merespons, dan status di layar aplikasi pengguna juga ikut berubah.  

---

## 3. Skema Database (Firebase Realtime Database)

Struktur JSON yang digunakan di dalam database:

```json
{
  "esiot-db": {
    "kontrol": {
      "led": 0
    },
    "sensor": {
      "kelembapan": 65.5,
      "suhu": 28.4
    }
  }
}
