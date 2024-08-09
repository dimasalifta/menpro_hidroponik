from dotenv import load_dotenv
import os
load_dotenv()
import paho.mqtt.client as mqtt
import asyncio
import multiprocessing
import json
import time

MQTT_BROKER = os.getenv("MQTT_BROKER")
MQTT_PORT = os.getenv("MQTT_PORT")
# MQTT_USERNAME = os.getenv("MQTT_USERNAME")
# MQTT_PASSWORD = os.getenv("MQTT_PASSWORD")
MQTT_TOPIC = os.getenv("MQTT_TOPIC")
ph_value = None
tds_value = None
class SistemFuzzy:
    def __init__(self, ph, tds):
        self.ph = ph
        self.tds = tds
        self.ph_rendah = 0
        self.ph_normal = 0
        self.ph_tinggi = 0
        self.tds_rendah = 0
        self.tds_normal = 0
        self.tds_tinggi = 0
        self.hasil = ""

    def xph_rendah(self):
        # print(self.ph)
        if self.ph <= 5.5:
            self.ph_rendah = 1
        elif self.ph > 5.5:
            self.ph_rendah = 0

        return self.ph_rendah

    def xph_normal(self):
        if self.ph < 5.5:
            self.ph_normal = 0
        elif 5.5 <= self.ph <= 6.5:
            self.ph_normal = (self.ph - 5.5) / 1
        elif self.ph > 6.5:
            self.ph_normal = 0
        return self.ph_normal

    def xph_tinggi(self):
        if self.ph <= 6.5:
            self.ph_tinggi = 0
        elif self.ph > 6.5:
            self.ph_tinggi = 1
        return self.ph_tinggi

    def ytds_rendah(self):
        if self.tds <= 1050:
            self.tds_rendah = 1
        elif self.tds > 1050:
            self.tds_rendah = 0
        return self.tds_rendah

    def ytds_normal(self):
        if self.tds <= 1050:
            self.tds_normal = 0
        elif 1050 <= self.tds <= 1400:
            self.tds_normal = (self.tds - 1050) / 350
        elif self.tds > 1400:
            self.tds_normal = 0
        return self.tds_normal

    def ytds_tinggi(self):
        if self.tds <= 1400:
            self.tds_tinggi = 0
        elif self.tds > 1400:
            self.tds_tinggi = 1
        return self.tds_tinggi

    def fuzzifikasi(self):
        # print("test")
        self.xph_rendah()
        self.xph_normal()
        self.xph_tinggi()
        self.ytds_rendah()
        self.ytds_normal()
        self.ytds_tinggi()
        
        # print(self.ph_rendah())
        fuzzyfikasi_data = {
            "ph_rendah":self.ph_rendah,
            "ph_normal":self.ph_normal,
            "ph_tinggi":self.ph_tinggi,
            "tds_rendah":self.tds_rendah,
            "tds_normal":self.tds_normal,
            "tds_tinggi":self.tds_tinggi
        }
        fuzzyfikasi_data = json.dumps(fuzzyfikasi_data,indent=4)
        # print(fuzzyfikasi_data)
        print(f"ph_rendah: {self.ph_rendah}\tph_normal: {self.ph_normal}\tph_tinggi: {self.ph_tinggi}")
        print(f"tds_rendah: {self.tds_rendah}\ttds_normal: {self.tds_normal}\ttds_tinggi: {self.tds_tinggi}")
        return fuzzyfikasi_data

    def inference(self):
        if self.ph_rendah >= 0.5 and self.tds_rendah >= 0.5:
            self.hasil = "SAKIT"
        elif self.ph_rendah >= 0.5 and self.tds_normal >= 0.5:
            self.hasil = "STRES"
        elif self.ph_rendah >= 0.5 and self.tds_tinggi >= 0.5:
            self.hasil = "STRES"
        elif self.ph_normal >= 0.5 and self.tds_rendah >= 0.5:
            self.hasil = "STRES"
        elif self.ph_normal >= 0.5 and self.tds_normal >= 0.5:
            self.hasil = "SEHAT"
        elif self.ph_normal >= 0.5 and self.tds_tinggi >= 0.5:
            self.hasil = "STRES"
        elif self.ph_tinggi >= 0.5 and self.tds_rendah >= 0.5:
            self.hasil = "STRES"
        elif self.ph_tinggi >= 0.5 and self.tds_normal >= 0.5:
            self.hasil = "STRES"
        elif self.ph_tinggi >= 0.5 and self.tds_tinggi >= 0.5:
            self.hasil = "SAKIT"
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
        client.subscribe("tds_sensor")
        client.subscribe("ph_sensor")
    else:
        print("Connection failed")
        
# Fungsi yang dipanggil ketika pesan diterima
def on_message(client, userdata, msg):
    # print("Received message on topic:", msg.topic)
    try:
        global ph_value,tds_value
        # print("Received JSON payload:")
        topic = msg.topic
        payload = msg.payload.decode()
        # print(type(topic))
        
        if topic == 'ph_sensor':
            ph_value = payload
        elif topic == 'tds_sensor':
            tds_value = payload
        
        ph_value = float(ph_value)
        tds_value = float(tds_value)
        print(f'Data pH:{ph_value} | TDS:{tds_value}')
        # data = json.loads(payload)
        
        # temperature = data.get('temperature')
        # humidity = data.get('humidity')
        # ammonia = data.get('ammonia')
        # client.publish('ais/temperature',temperature)
        # client.publish('ais/humidity',humidity)
        # client.publish('ais/ammonia',ammonia)
        
        sistem_fuzzy = SistemFuzzy(3, 700)  # Atur beberapa nilai tes
        print(sistem_fuzzy.fuzzifikasi())
        print(sistem_fuzzy.inference())
        # client.publish("ais/fuzzyfication",sistem_fuzzy.fuzzifikasi())
        client.publish("inference_fuzzy",sistem_fuzzy.inference())
        # nilai_crisp_kelembaban,nilai_crisp_amoniak = sistem_fuzzy.defuzzifikasi()
        # client.publish("ais/defuzzyfication/kelembaban",nilai_crisp_kelembaban)
        # client.publish("ais/defuzzyfication/ammonia",nilai_crisp_amoniak)
        print("++++++++++++++++++++++++++++++++++++++++++++")
        time.sleep(5)
    except Exception as e:
        # Jika payload tidak bisa diuraikan sebagai JSON, cetak sebagai string biasa
        print(f"Received String Payload: {e}", msg.payload)
        pass
        
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