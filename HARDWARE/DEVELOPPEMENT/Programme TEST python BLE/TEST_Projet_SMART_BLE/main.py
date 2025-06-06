# === IMPORT DES BIBLIOTHÈQUES ===
import asyncio  # Pour la boucle asynchrone BLE
from bleak import BleakScanner, BleakClient  # Pour la communication BLE
import requests  # Pour envoyer les données au backend
from flask import Flask, request, jsonify  # Pour créer l’API Flask
import threading  # Pour lancer BLE et Flask en parallèle
import time  # Pour les délais

# === PARAMÈTRES À PERSONNALISER ===
DEVICE_NAME = "MinuteurESP32"  # Nom de l’appareil BLE à scanner
NOTIFY_CHAR_UUID = "0000abf2-0000-1000-8000-00805f9b34fb"  # UUID de la caractéristique de notification
WRITE_CHAR_UUID = "0000abf1-0000-1000-8000-00805f9b34fb"  # UUID de la caractéristique d’écriture
BACKEND_URL = "http://localhost:8080/douches"  # URL de l’API backend pour recevoir les données de douche

# === INITIALISATION DE FLASK ===
app = Flask(__name__)

# === VARIABLES GLOBALES BLE ===
ble_client = None  # Instance du client BLE connecté
ble_loop = asyncio.new_event_loop()  # Nouvelle boucle asyncio pour BLE
ble_lock = threading.Lock()  # Verrou pour sécuriser l'accès au client BLE

# === PARSING DU MESSAGE BLE REÇU ===
def parse_ble_message(msg):
    """
    Extrait le nom d’utilisateur et la durée depuis une chaîne BLE.
    Format attendu : "User: Nom; Time: 300 s"
    """
    user, time_s = None, None
    try:
        parts = msg.split(";")
        for p in parts:
            if p.strip().startswith("User:"):
                user = p.split(":", 1)[1].strip()
            elif p.strip().startswith("Time:"):
                time_s = int(p.split(":", 1)[1].strip().split()[0])
    except Exception as e:
        print("Erreur parsing BLE :", e)
    return user, time_s

# === GESTION DES NOTIFICATIONS BLE ===
def notification_handler(sender, data):
    """
    Callback appelée automatiquement lors de la réception d’une notification BLE.
    """
    msg = data.decode(errors='ignore')
    print(f"🔔 Notification reçue : {msg}")
    user, time_s = parse_ble_message(msg)

    if user and time_s is not None:
        payload = {"user": user, "timeSeconds": time_s}
        try:
            # Envoi de la donnée vers le backend
            resp = requests.post(BACKEND_URL, json=payload)
            print("Donnée envoyée au backend :", resp.status_code)
        except Exception as e:
            print("Erreur d’envoi HTTP :", e)
    else:
        print("Message BLE non reconnu, ignoré.")

# === CONNEXION ET ABONNEMENT AUX NOTIFICATIONS BLE ===
async def connect_ble():
    """
    Recherche l’ESP32 BLE, s’y connecte et s’abonne aux notifications.
    Se reconnecte automatiquement en cas de déconnexion.
    """
    global ble_client
    while True:
        try:
            print("🔍 Recherche de l'ESP32 BLE...")
            devices = await BleakScanner.discover()
            esp_device = next((d for d in devices if d.name and DEVICE_NAME in d.name), None)

            if not esp_device:
                print("❌ ESP32 non trouvé. Nouvelle tentative dans 5s...")
                await asyncio.sleep(5)
                continue

            print(f"🔗 Connexion à {esp_device.name} ({esp_device.address})...")
            client = BleakClient(esp_device.address)
            await client.connect()

            if not client.is_connected:
                print("❌ Connexion échouée. Nouvelle tentative dans 5s...")
                await asyncio.sleep(5)
                continue

            print("✅ Connecté. Abonnement aux notifications...")
            await client.start_notify(NOTIFY_CHAR_UUID, notification_handler)

            # Stocker le client connecté
            with ble_lock:
                ble_client = client

            # Boucle tant que la connexion est active
            while client.is_connected:
                await asyncio.sleep(1)

            print("⚠️ Déconnexion détectée. Reconnexion en cours...")

        except Exception as e:
            print("❌ Erreur pendant connexion BLE :", e)

        finally:
            # Nettoyage en cas d'erreur ou de déconnexion
            with ble_lock:
                ble_client = None
            await asyncio.sleep(3)

# === DÉMARRAGE DE LA BOUCLE BLE DANS UN THREAD ===
def start_ble_loop(loop):
    """
    Initialise et lance la boucle BLE dans un thread séparé.
    """
    asyncio.set_event_loop(loop)
    loop.run_until_complete(connect_ble())

# === FLASK - ENDPOINT POUR ENVOYER UN MESSAGE VERS L'ESP32 ===
@app.route('/sendToEsp32', methods=['POST'])
def send_to_esp32():
    """
    Reçoit une requête POST contenant un message à transmettre à l’ESP32 via BLE.
    Exemple JSON : { "message": "START" }
    """
    data = request.json
    message = data.get("message", "")
    print(f"Reçu du backend pour envoi BLE : {message}")

    # Fonction asynchrone interne pour écrire sur la caractéristique BLE
    async def write_ble():
        with ble_lock:
            client = ble_client
        if client and client.is_connected:
            try:
                await client.write_gatt_char(WRITE_CHAR_UUID, message.encode('utf-8'))
                print(f"📤 Message BLE envoyé à l'ESP32 : {message}")
            except Exception as e:
                print("Erreur écriture BLE :", e)
        else:
            print("❌ ESP32 non connecté !")

    # Exécution sécurisée dans la boucle BLE
    asyncio.run_coroutine_threadsafe(write_ble(), ble_loop)
    return jsonify({"status": "sent"}), 200

# === LANCEMENT DE L’APPLICATION ===
if __name__ == "__main__":
    # Lancer BLE dans un thread secondaire (non-bloquant)
    threading.Thread(target=start_ble_loop, args=(ble_loop,), daemon=True).start()

    # Lancer Flask (API REST pour communication backend <-> ESP32)
    app.run(host="0.0.0.0", port=5001)  # Accessible sur le réseau local
