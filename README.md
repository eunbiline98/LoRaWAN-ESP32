# LoRa Gateway-ESP32
Project Gateway LoRaWAN using chip SX278 &amp; MCU ESP32

# What is the meaning of LoRa Gateway?
Dalam jaringan LoRaWAN, LoRa gateway berperan sebagai jembatan penghubung antara perangkat IoT (Internet of Things) dan server jaringan.  Bentuknya bisa dibilang mirip seperti router WiFi, namun fungsinya lebih spesifik untuk jaringan LoRaWAN.

### Tugas utama LoRa gateway :

- Menerima data yang dikirimkan oleh perangkat IoT (disebut end node) menggunakan teknologi modulasi LoRa.
- Melakukan filtering dan forward (meneruskan) data tersebut ke server jaringan melalui koneksi internet atau jaringan lainnya.
- Menerima instruksi dari server jaringan dan mengirimkannya kembali ke perangkat IoT yang relevan.

### Kemampuan LoRa gateway :

- Jangkauan: Mampu menerima sinyal dari perangkat IoT dalam radius tertentu, tergantung dari kondisi lingkungan.
- Konektivitas: Biasanya terhubung ke internet menggunakan kabel LAN atau koneksi nirkabel lainnya.
- Kapasitas: Dapat menangani komunikasi dengan banyak perangkat IoT secara bersamaan.


# Topology
![image](https://github.com/eunbiline98/LoRaWAN-ESP32/assets/50385294/929ed9bb-872a-44ea-914c-850998752baa)

Dapat dilihat pada gambar di topologi diatas, dimana LoRaWAN gateway berfungsi sebagai coordinator menerima sebuah data dari masing - masing node, dimana setiap node memiliki ID unique yang berfungsi sebagai identitas.
Data - data yang diterima oleh gateway LoRaWAN tersebut dikirimkan ke server menggunakan protocol MQTT ataupun protocol lain nya seperti HTTP Request, Rest API, dll sesuai dengan arsitektur sistem yang dibuat

# Data Format
![image](https://github.com/eunbiline98/LoRaWAN-ESP32/assets/50385294/dc7eda40-4646-4de6-8bb7-d02b294352d5)

Data yang diterima oleh gateway LoRaWAN diubah menjadi format JSON, memiliki beberapa alasan sebagai berikut:

- JSON adalah format data yang ringan dan efisien. JSON menggunakan karakter ASCII, yang berarti dapat ditransmisikan dengan mudah melalui jaringan dengan bandwidth terbatas.
- JSON adalah format data yang mudah dibaca dan ditulis. JSON menggunakan struktur yang sederhana dan mudah dipahami, sehingga dapat dengan mudah dibaca dan ditulis oleh manusia maupun mesin.
- JSON adalah format data yang bersifat umum. JSON dapat digunakan untuk mewakili berbagai jenis data, mulai dari data sederhana hingga data kompleks.

# Schematic 
![image](https://github.com/eunbiline98/LoRaWAN-ESP32/assets/50385294/7b0b3e53-0666-4f32-86a6-bc238aa447ea)


