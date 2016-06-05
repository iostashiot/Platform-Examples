# IOStash on Raspsberry Pi using MQTT


import paho.mqtt.client as paho
import RPi.GPIO as GPIO
import json, time

# Credentials
accessToken  = '<YOUR X-ACCESS-TOKEN HERE>'
device_id    = '<YOUR DEVICE ID HERE>'
client_id = 'Raspberry PI'

# Define board constants
buttonPin = 15
ledPin = 8

# Setup board
GPIO.setmode(GPIO.BOARD)
GPIO.cleanup() 
GPIO.setup(ledPin, GPIO.OUT)
GPIO.setup(buttonPin, GPIO.IN)


# On connection event callback
def on_connect(client, data, flags, rc):
    print('Connected, Reconnected -> ' + str(rc))

# Subscription event callback
def on_subscribe(client, userdata, mid, gqos):
    print('Subscribed: ' + str(mid))

# On message received event callback
def on_message(client, obj, msg):
    json_data = msg.payload
    # Print the data received
    print(json_data)
    value = json.loads(json_data)['publishData']['msg']
    if value == 1:
        led_status = GPIO.HIGH
        GPIO.output(ledPin, GPIO.HIGH)
    else:
        led_status = GPIO.LOW
        GPIO.output(ledPin, GPIO.LOW)

    # Publish back the state to IOStash
    payload = { 'state': value }
    client.publish(out_topic, json.dumps(payload))

# Setup MQTT client
client = paho.Client(client_id=client_id, protocol=paho.MQTTv31)

# Setup callbacks
client.on_message = on_message
client.on_connect = on_connect
client.on_subscribe = on_subscribe


# Setup topics
listen_topic  = '/' + accessToken + '/' + device_id + '/custom' # Listen to custom events
publish_topic = '/' + accessToken + '/' + device_id + '/'  # Publish state back

# Initiate MQTT connection
client.username_pw_set(accessToken, '123')  
client.connect("api.iostash.io")
client.subscribe(listen_topic, 1) # QoS 1 

prev_status = GPIO.LOW
led_status  = GPIO.LOW
updated_at  = 0  
debounce    = 0.2 

# Continue the network loop, exit when an error occurs
rc = 0
while rc == 0:
    rc = client.loop()
    button = GPIO.input(buttonPin)

    if button != prev_status and time.time() - updated_at > debounce:
        prev_status = button
        updated_at  = time.time()

        if button:
            led_status = not led_status
            button = 0
            if led_status == GPIO.HIGH:
                button = 1
            GPIO.output(ledPin, led_status)
            payload = { 'state': button }
            client.publish(publish_topic, json.dumps(payload))

print('Reconnection -> ' + str(rc))
