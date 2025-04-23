import cv2
import pytesseract
import time
import threading
import urllib.request
import numpy as np
import requests


ESP32_CAM_URL = "http:///capture"
ESP8266_URL = "http:///braille"  

pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'

braille_mapping = {
    'a': '100000', 'b': '110000', 'c': '100100', 'd': '100110', 'e': '100010',
    'f': '110100', 'g': '110110', 'h': '110010', 'i': '010100', 'j': '010110',
    'k': '101000', 'l': '111000', 'm': '101100', 'n': '101110', 'o': '101010',
    'p': '111100', 'q': '111110', 'r': '111010', 's': '011100', 't': '011110',
    'u': '101001', 'v': '111001', 'w': '010111', 'x': '101101', 'y': '101111',
    'z': '101011', ' ': '000000',
    '0': '010110', '1': '100000', '2': '110000', '3': '100100', '4': '100110',
    '5': '100010', '6': '110100', '7': '110110', '8': '110010', '9': '010100',
    '#': '001111', '.': '010011', ',': '010000', '?': '011001', '!': '011010',
    '-': '001001', '(': '011011', ')': '011011', ';': '011000', ':': '010010',
    '/': '001100', '+': '011101', '=': '111111', '*': '100001', '@': '000100',
    '$': '110011', '%': '101101', '&': '111101', '"': '001011', "'": '001000',
    '_': '001111',
}

extracted_text = ""
lock = threading.Lock()
processing = False

def capture_frame_from_esp32():
    try:
        resp = urllib.request.urlopen(ESP32_CAM_URL)
        img_array = np.array(bytearray(resp.read()), dtype=np.uint8)
        frame = cv2.imdecode(img_array, -1)
        rotated_frame = cv2.rotate(frame, cv2.ROTATE_180)
        return rotated_frame
    except Exception as e:
        print(f"Error capturing frame: {e}")
        return None

def extract_text_from_frame(frame, confidence_threshold=60):
    data = pytesseract.image_to_data(frame, output_type=pytesseract.Output.DICT, config='--psm 6')
    text = []
    for i in range(len(data['text'])):
        if data['text'][i].strip():
            try:
                conf = int(data['conf'][i])
                if conf > confidence_threshold:
                    text.append(data['text'][i])
            except ValueError:
                pass
    return ' '.join(text)

def text_extraction_thread(frame):
    global extracted_text, processing
    text = extract_text_from_frame(frame)
    with lock:
        extracted_text = text
    processing = False

def send_braille_to_esp8266(pattern):
    try:
        payload = {"pattern": pattern}
        response = requests.post(ESP8266_URL, json=payload, timeout=2)
        if response.status_code == 200:
            print(f"Sent pattern to ESP8266: {pattern}")
        else:
            print(f"ESP8266 error: {response.status_code}")
    except Exception as e:
        print(f"Error sending to ESP8266: {e}")

def display_braille(text, delay=0.5):
    for char in text.lower():
        if char in braille_mapping:
            pattern = braille_mapping[char]
            print(f"Braille for '{char}': {pattern}")
            send_braille_to_esp8266(pattern)
            time.sleep(delay)
        else:
            print(f"No Braille mapping found for '{char}'")

def preprocess_text_for_braille(text):
    processed_text = []
    i = 0
    while i < len(text):
        if text[i].isdigit():
            if i == 0 or text[i - 1] != '#':
                processed_text.append('#')
        processed_text.append(text[i])
        i += 1
    return ''.join(processed_text)


frame_delay = 0.5  
word_delay = 1     

while True:
    frame = capture_frame_from_esp32()
    if frame is None:
        continue

    if not processing:
        processing = True
        threading.Thread(target=text_extraction_thread, args=(frame,)).start()

    cv2.imshow("ESP32-CAM Feed", frame)

    with lock:
        if extracted_text:
            print("Extracted Text:", extracted_text)
            processed_text = preprocess_text_for_braille(extracted_text)
            print("Processed Text for Braille:", processed_text)

            words = processed_text.split()
            for word in words:
                print("Processing word:", word)
                display_braille(word, delay=0.5)
                time.sleep(word_delay)

            extracted_text = ""

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    time.sleep(frame_delay)

cv2.destroyAllWindows()