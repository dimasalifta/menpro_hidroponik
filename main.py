from dotenv import load_dotenv
import os
load_dotenv()
import paho.mqtt.client as mqtt
import asyncio
import multiprocessing
import json

MQTT_BROKER = os.getenv("MQTT_BROKER")
MQTT_PORT = os.getenv("MQTT_PORT")
# MQTT_USERNAME = os.getenv("MQTT_USERNAME")
# MQTT_PASSWORD = os.getenv("MQTT_PASSWORD")
MQTT_TOPIC = os.getenv("MQTT_TOPIC")
class SistemFuzzy:
    def __init__(self, kelembaban, amoniak):
        self.kelembaban = kelembaban
        self.amoniak = amoniak
        self.basah = 0
        self.lembab = 0
        self.kering = 0
        self.normal = 0
        self.sedang = 0
        self.tinggi = 0
        self.hasil = ""

    def kelembaban_basah(self):
        if self.kelembaban <= 65:
            self.basah = 0
        elif 65 < self.kelembaban < 85:
            self.basah = (self.kelembaban-65) / 20
        elif self.kelembaban >= 85:
            self.basah = 1
        return self.basah

    def kelembaban_lembab(self):
        if self.kelembaban <= 25:
            self.lembab = 0
        elif 25 <= self.kelembaban <= 45:
            self.lembab = (self.kelembaban - 25) / 20
        elif 65 < self.kelembaban <= 85:
            self.lembab = (85 - self.kelembaban) / 20
        elif 45 <= self.kelembaban <= 65:
            self.lembab = 1
        return self.lembab

    def kelembaban_kering(self):
        if self.kelembaban <= 25:
            self.kering = 1
        elif 25 < self.kelembaban < 45:
            self.kering = (45-self.kelembaban) / 20
        elif self.kelembaban >= 45:
            self.kering = 0
        return self.kering

    def amoniak_normal(self):
        if self.amoniak <= 20:
            self.normal = 1
        elif 20 < self.amoniak < 30:
            self.normal = (30-self.amoniak) / 10
        elif self.amoniak >= 30:
            self.normal = 0
        return self.normal

    def amoniak_sedang(self):
        if self.amoniak <= 20 or self.amoniak >= 60:
            self.sedang = 0
        elif 20 <= self.amoniak <= 30:
            self.sedang = (self.amoniak - 20) / 10
        elif 50 < self.amoniak <= 60:
            self.sedang = (60 - self.amoniak) / 10
        elif 30 <= self.amoniak <= 50:
            self.sedang = 1
        return self.sedang

    def amoniak_tinggi(self):
        if self.amoniak >= 60:
            self.tinggi = 1
        elif 50 < self.amoniak < 60:
            self.tinggi = (self.amoniak - 50) / 10
        elif self.amoniak <= 50:
            self.tinggi = 0
        return self.tinggi

    def fuzzifikasi(self):
        self.kelembaban_basah()
        self.kelembaban_lembab()
        self.kelembaban_kering()
        self.amoniak_normal()
        self.amoniak_sedang()
        self.amoniak_tinggi()
        
        fuzzyfikasi_data = {
            "kering":self.kering,
            "lembab":self.lembab,
            "basah":self.basah,
            "normal":self.normal,
            "sedang":self.sedang,
            "tinggi":self.tinggi,
        }
        fuzzyfikasi_data = json.dumps(fuzzyfikasi_data,indent=4)
        # print(f"basah: {self.basah}\tlembab: {self.lembab}\tkering: {self.kering}")
        # print(f"Normal: {self.normal}\tsedang: {self.sedang}\tTinggi: {self.tinggi}")
        return fuzzyfikasi_data
    def inference(self):
        if self.normal >= 0.5 and self.kering >= 0.5:
            self.hasil = "AMAN"
        elif self.normal >= 0.5 and self.lembab >= 0.5:
            self.hasil = "AMAN"
        elif self.sedang >= 0.5 and self.kering >= 0.5:
            self.hasil = "AMAN"
        elif self.normal >= 0.5 and self.basah >= 0.5:
            self.hasil = "WASPADA"
        elif self.sedang >= 0.5 and self.lembab >= 0.5:
            self.hasil = "WASPADA"
        elif self.tinggi >= 0.5 and self.kering >= 0.5:
            self.hasil = "WASPADA"
        elif self.sedang >= 0.5 and self.basah >= 0.5:
            self.hasil = "BAHAYA"
        elif self.tinggi >= 0.5 and self.lembab >= 0.5:
            self.hasil = "BAHAYA"
        elif self.tinggi >= 0.5 and self.basah >= 0.5:
            self.hasil = "BAHAYA"
        else:
            self.hasil = "TIDAK TERDEFINISI"
        status = {
            "STATUS":self.hasil
        }
        
        status_json = json.dumps(status, indent=4)
        return status_json
        # print(f"HASIL: {status_json}")
    def defuzzifikasi(self):
        centroid_kering = 10  # Centroid untuk himpunan kering (berdasarkan kelembaban)
        centroid_lembab = 55  # Centroid untuk himpunan lembab (berdasarkan kelembaban)
        centroid_basah = 92.5  # Centroid untuk himpunan basah (berdasarkan kelembaban)

        centroid_normal = 10  # Centroid untuk himpunan normal (berdasarkan amoniak)
        centroid_sedang = 35  # Centroid untuk himpunan sedang (berdasarkan amoniak)
        centroid_tinggi = 75  # Centroid untuk himpunan tinggi (berdasarkan amoniak)
        # print(self.basah + self.lembab + self.kering)
        # Hitung nilai crisp dengan metode centroid
        nilai_crisp_kelembaban = (self.basah * centroid_basah + self.lembab * centroid_lembab + self.kering * centroid_kering) / (
                    self.basah + self.lembab + self.kering)
        nilai_crisp_amoniak = (self.normal * centroid_normal + self.sedang * centroid_sedang + self.tinggi * centroid_tinggi) / (
                    self.normal + self.sedang + self.tinggi)

        return nilai_crisp_kelembaban,nilai_crisp_amoniak   
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to broker")
        client.publish("AIS","Launched...!!!")  # Subscribe ke topik "topic/test"
        client.subscribe(MQTT_TOPIC)  # Subscribe ke topik "topic/test"
    else:
        print("Connection failed")
        
# Fungsi yang dipanggil ketika pesan diterima
def on_message(client, userdata, msg):
    # print("Received message on topic:", msg.topic)
    try:
        
        # print("Received JSON payload:")
        payload = msg.payload.decode()
        print(payload)
        data = json.loads(payload)
        
        temperature = data.get('temperature')
        humidity = data.get('humidity')
        ammonia = data.get('ammonia')
        client.publish('ais/temperature',temperature)
        client.publish('ais/humidity',humidity)
        client.publish('ais/ammonia',ammonia)
        sistem_fuzzy = SistemFuzzy(kelembaban=humidity, amoniak=ammonia)  # Atur beberapa nilai tes
        
        client.publish("ais/fuzzyfication",sistem_fuzzy.fuzzifikasi())
        client.publish("ais/inference",sistem_fuzzy.inference())
        nilai_crisp_kelembaban,nilai_crisp_amoniak = sistem_fuzzy.defuzzifikasi()
        client.publish("ais/defuzzyfication/kelembaban",nilai_crisp_kelembaban)
        client.publish("ais/defuzzyfication/ammonia",nilai_crisp_amoniak)
    except Exception as e:
        # Jika payload tidak bisa diuraikan sebagai JSON, cetak sebagai string biasa
        print(f"Received String Payload: {e}", msg.payload)
        
def mqtt_process():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    # client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    client.connect(MQTT_BROKER, 1883, 60)
    client.loop_forever()
    
if __name__ == "__main__":
    mqtt_main = multiprocessing.Process(target=mqtt_process)
    mqtt_main.start() 