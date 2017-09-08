esp32_ble_gateway
==================

This repository hosts the project for the ESP32 based BLE WiFi Gateway.

The device will scan for BLE devices every n seconds, then ping them to a configured URL.

The WiFi to connect to will be provisioned by means of a factory reset button that puts the device in Access Point mode running an HTTP server, so that the WiFI network name and password can be entered by means of any web enabled device


Known issues
------------

- Requests made in quick succession fail. Should have something to do with the requests code.