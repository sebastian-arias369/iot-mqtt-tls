/*
 * The MIT License
 *
 * Copyright 2024 Alvaro Salazar <alvaro@denkitronik.com>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <WiFi.h>
#include <libiot.h>
#include <libwifi.h>
#include <libdisplay.h>
#include <libota.h>

SensorData data;  // Estructura para almacenar los datos de temperatura y humedad del SHT21
time_t hora;      // Timestamp de la hora actual

/**
 * Configura el dispositivo para conectarse a la red WiFi y ajusta parametros IoT
 */
void setup() {
  Serial.begin(115200);     // Paso 1. Inicializa el puerto serie
  listWiFiNetworks();       // Paso 2. Lista las redes WiFi disponibles
  delay(1000);              // -- Espera 1 segundo para ver las redes disponibles
  startDisplay();           // Paso 3. Inicializa la pantalla OLED
  displayConnecting(ssid);  // Paso 4. Muestra en la pantalla el mensaje de "Conectandose a :" y luego el nombre de la red a la que se conecta
  startWiFi("");            // Paso 5. Inicializa el servicio de WiFi
  setupIoT();               // Paso 6. Inicializa el servicio de IoT
  hora = setTime();         // Paso 7. Ajusta el tiempo del dispositivo con servidores SNTP
  Serial.println("Firmware version v1.0"); // Paso 8. Imprime en el puerto serie la versión del firmware
}

// Función loop
void loop() {
  checkWiFi();                                                   // Paso 1. Verifica la conexión a la red WiFi y si no está conectado, intenta reconectar
  checkMQTT();                                                   // Paso 2. Verifica la conexión al servidor MQTT y si no está conectado, intenta reconectar
  String message = checkAlert();                                 // Paso 3. Verifica si hay alertas y las retorna en caso de haberlas
  if(measure(&data)){                                            // Paso 4. Realiza una medición de temperatura y humedad
    displayLoop(message, hora, data.temperature, data.humidity); // Paso 5. Muestra en la pantalla el mensaje recibido, las medidas de temperatura y humedad
    sendSensorData(data.temperature, data.humidity);             // Paso 6. Envía los datos de temperatura y humedad al servidor MQTT
  }   
}
