import os
from google.cloud import storage
from datetime import datetime

now = datetime.now() # current date and time
date_time = now.strftime("%Y-%m-%d_%H:%M:%S")

client = storage.Client.from_service_account_json('./scripts/auth.json')
# https://console.cloud.google.com/storage/browser/[bucket-id]/
bucket = client.get_bucket('remote-esp32-upload-firmwares')

blob2 = bucket.blob('firmware_esp32'+date_time+'.bin')
blob2.upload_from_filename(filename='./.pio/build/esp32/firmware_esp32.bin')
