import asyncio
from bleak import BleakClient, BleakScanner

# UUIDs de ton ESP32
SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

async def main():
    print("Scan des périphériques BLE...")
    devices = await BleakScanner.discover(timeout=5)

    target = None
    for d in devices:
        print(f"{d.name} - {d.address}")
        if d.name and "MinuteurESP32" in d.name:
            target = d
            break

    if not target:
        print("ESP32 non trouvé. Assure-toi qu'il est sous tension et en mode BLE.")
        return

    print(f"Connexion à {target.name} ({target.address})...")

    async with BleakClient(target.address) as client:
        print("Connecté !")

        def handle_notify(handle, data):
            print(f"Notification reçue : {data.decode()}")

        await client.start_notify(CHARACTERISTIC_UUID, handle_notify)

        print("En attente des données BLE... Ctrl+C pour quitter.")
        while True:
            await asyncio.sleep(1)

if __name__ == "__main__":
    asyncio.run(main())
