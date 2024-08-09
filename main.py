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

    def xph_rendah(self,a,b,c,d):
        """Menghitung derajat keanggotaan trapezoidal."""
        if self.ph < a or self.ph > d:
            self.ph_rendah = 0
        elif self.ph >= a and self.ph < b:
            self.ph_rendah = (self.ph - a) / (b - a)
        elif self.ph > c and self.ph < d:
            self.ph_rendah = (d - self.ph) / (d - c)
        elif self.ph >= b and self.ph <= c: 
            self.ph_rendah = 1

        return self.ph_rendah

    def xph_normal(self,a,b,c,d):
        """Menghitung derajat keanggotaan trapezoidal."""
        if self.ph < a or self.ph > d:
            self.ph_normal = 0
        elif self.ph >= a and self.ph < b:
            self.ph_normal = (self.ph - a) / (b - a)
        elif self.ph > c and self.ph < d:
            self.ph_normal = (d - self.ph) / (d - c)
        elif self.ph >= b and self.ph <= c: 
            self.ph_normal = 1
        
        return self.ph_normal

    def xph_tinggi(self,a,b,c,d): #6.5, 6.5, 14.0, 14.0
        """Menghitung derajat keanggotaan trapezoidal."""
        if self.ph < a or self.ph > d:
            self.ph_tinggi = 0
        elif self.ph >= a and self.ph < b:
            self.ph_tinggi = (self.ph - a) / (b - a)
        elif self.ph > c and self.ph < d:
            self.ph_tinggi = (d - self.ph) / (d - c)
        elif self.ph >= b and self.ph <= c: 
            self.ph_tinggi = 1
        
        return self.ph_tinggi

    def ytds_rendah(self,a,b,c,d):
        """Menghitung derajat keanggotaan trapezoidal."""
        if self.tds < a or self.tds > d:
            self.tds_rendah = 0
        elif self.tds >= a and self.tds < b:
            self.tds_rendah = (self.tds - a) / (b - a)
        elif self.tds > c and self.tds < d:
            self.tds_rendah = (d - self.tds) / (d - c)
        elif self.tds >= b and self.tds <= c: 
            self.tds_rendah = 1
        
        return self.tds_rendah

    def ytds_normal(self,a,b,c,d):
        """Menghitung derajat keanggotaan trapezoidal."""
        if self.tds < a or self.tds > d:
            self.tds_normal = 0
        elif self.tds >= a and self.tds < b:
            self.tds_normal = (self.tds - a) / (b - a)
        elif self.tds > c and self.tds < d:
            self.tds_normal = (d - self.tds) / (d - c)
        elif self.tds >= b and self.tds <= c: 
            self.tds_normal = 1
        
        return self.tds_normal

    def ytds_tinggi(self,a,b,c,d):
        """Menghitung derajat keanggotaan trapezoidal."""
        if self.tds < a or self.tds > d:
            self.tds_tinggi = 0
        elif self.tds >= a and self.tds < b:
            self.tds_tinggi = (self.tds - a) / (b - a)
        elif self.tds > c and self.tds < d:
            self.tds_tinggi = (d - self.tds) / (d - c)
        elif self.tds >= b and self.tds <= c: 
            self.tds_tinggi = 1
            
        return self.tds_tinggi

    
    def fuzzifikasi(self):
        # print("test")
        self.xph_rendah(0, 0, 5.5, 5.5)
        self.xph_normal(5.5, 5.5, 6.5, 6.5)
        self.xph_tinggi(6.5, 6.5, 14.0, 14.0)
        self.ytds_rendah(0, 0, 1050, 1050)
        self.ytds_normal(1050, 1050, 1400, 1400)
        self.ytds_tinggi(1400, 1400, 2000, 2000)
        
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
        # print(f"ph_rendah: {self.ph_rendah}\tph_normal: {self.ph_normal}\tph_tinggi: {self.ph_tinggi}")
        # print(f"tds_rendah: {self.tds_rendah}\ttds_normal: {self.tds_normal}\ttds_tinggi: {self.tds_tinggi}")
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
        centroid_ph_rendah = 2.75  
        centroid_ph_normal = 6  
        centroid_ph_tinggi = 10.25 

        centroid_tds_rendah = 525  
        centroid_tds_normal = 1225  
        centroid_tds_tinggi = 1700

        # Hitung nilai crisp dengan metode centroid
        nilai_crisp_ph = (self.ph_tinggi * centroid_ph_tinggi + self.ph_normal * centroid_ph_normal + self.ph_rendah * centroid_ph_rendah) / (
                    self.ph_tinggi + self.ph_normal + self.ph_rendah)
        nilai_crisp_tds = (self.tds_rendah * centroid_tds_rendah + self.tds_normal * centroid_tds_normal + self.tds_tinggi * centroid_tds_tinggi) / (
                    self.tds_rendah + self.tds_normal + self.tds_tinggi)

        return nilai_crisp_ph,nilai_crisp_tds   
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to broker")
        client.publish("menpro fuzzy","Launched...!!!")  # Subscribe ke topik "topic/test"
        client.subscribe("menpro/esp32/ph_sensor")
        client.subscribe("menpro/esp32/tds_sensor")
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
        
        if topic == 'menpro/esp32/ph_sensor':
            ph_value = payload
        elif topic == 'menpro/esp32/tds_sensor':
            tds_value = payload
        
        ph_value = float(ph_value)
        tds_value = float(tds_value)
        print(f'Data pH:{ph_value} | TDS:{tds_value}')
        
        sistem_fuzzy = SistemFuzzy(ph_value, tds_value)  # Atur beberapa nilai tes
        result_fuzzifikasi = sistem_fuzzy.fuzzifikasi()
        result_inference = sistem_fuzzy.inference()
        nilai_crisp_ph,nilai_tds = sistem_fuzzy.defuzzifikasi()
        
        client.publish("menpro/fuzzy/fuzzifikasi",result_fuzzifikasi)
        client.publish("menpro/fuzzy/inference_fuzzy",result_inference)
        client.publish("menpro/fuzzy/nilai_crisp_ph",nilai_crisp_ph)
        client.publish("menpro/fuzzy/nilai_crisp_tds",nilai_tds)
        print("")
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