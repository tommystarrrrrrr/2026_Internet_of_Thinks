import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_database/firebase_database.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp();
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'IoT Monitor',
      theme: ThemeData(
        brightness: Brightness.dark,
        primaryColor: Colors.blue,
        scaffoldBackgroundColor: const Color(0xFF0F172A),
      ),
      home: const HomePage(),
    );
  }
}

class HomePage extends StatefulWidget {
  const HomePage({super.key});

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  final dbRef = FirebaseDatabase.instance.ref("esiot-db");

  double suhu = 0;
  double kelembapan = 0;
  bool ledStatus = false;

  @override
  void initState() {
    super.initState();
    listenData();
  }

  void listenData() {
    dbRef.onValue.listen((event) {
      final data = event.snapshot.value as Map?;

      if (data != null) {
        setState(() {
          suhu = (data['suhu'] ?? 0).toDouble();
          kelembapan = (data['kelembapan'] ?? 0).toDouble();
          ledStatus = (data['led'] == "1");
        });
      }
    });
  }

  void toggleLED(bool value) {
    dbRef.child("led").set(value ? "1" : "0");
  }

  Widget buildCard(String title, String value, IconData icon) {
    return Container(
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        color: const Color(0xFF1E293B),
        borderRadius: BorderRadius.circular(20),
      ),
      child: Column(
        children: [
          Icon(icon, size: 40, color: Colors.blueAccent),
          const SizedBox(height: 10),
          Text(
            value,
            style: const TextStyle(fontSize: 28, fontWeight: FontWeight.bold),
          ),
          Text(title, style: const TextStyle(color: Colors.grey)),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("IoT Monitoring"),
        centerTitle: true,
        backgroundColor: Colors.transparent,
        elevation: 0,
      ),
      body: Padding(
        padding: const EdgeInsets.all(20),
        child: Column(
          children: [
            // DATA SENSOR
            Row(
              children: [
                Expanded(
                  child: buildCard(
                    "Suhu",
                    "${suhu.toStringAsFixed(1)} °C",
                    Icons.thermostat,
                  ),
                ),
                const SizedBox(width: 15),
                Expanded(
                  child: buildCard(
                    "Kelembapan",
                    "${kelembapan.toStringAsFixed(1)} %",
                    Icons.water_drop,
                  ),
                ),
              ],
            ),

            const SizedBox(height: 40),

            // CONTROL LED
            Container(
              padding: const EdgeInsets.all(20),
              decoration: BoxDecoration(
                color: const Color(0xFF1E293B),
                borderRadius: BorderRadius.circular(20),
              ),
              child: Column(
                children: [
                  const Text("Kontrol Lampu", style: TextStyle(fontSize: 18)),
                  const SizedBox(height: 20),
                  Switch(
                    value: ledStatus,
                    onChanged: (value) {
                      toggleLED(value);
                    },
                    activeColor: Colors.greenAccent,
                  ),
                  Text(
                    ledStatus ? "Lampu ON" : "Lampu OFF",
                    style: TextStyle(
                      color: ledStatus ? Colors.green : Colors.red,
                      fontSize: 16,
                    ),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}
