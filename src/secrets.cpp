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

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <libiot.h>
#include <libwifi.h>
#include <stdio.h>

/*********** Inicio de parametros configurables por el usuario *********/

// Cambia estos valores por los de su red y servidor MQTT
// Los topicos deben tener la estructura: <país>/<estado>/<ciudad>/<usuario>/out
#define COUNTRY "colombia"                        ///< País donde se encuentra el dispositivo
#define STATE "valle"                             ///< Estado donde se encuentra el dispositivo
#define CITY "tulua"                              ///< Ciudad donde se encuentra el dispositivo
#define MQTT_SERVER "esp32uceva.freeddns.org"         ///< Servidor MQTT
#define MQTT_PORT 8883                            ///< Puerto seguro (TLS)
#define MQTT_USER "admin"                       ///< Usuario MQTT no adninistrador 
#define MQTT_PASSWORD "admin1234"                  ///< Contraseña del usuario MQTT

// Variables de configuración de la red WiFi
// Cambia estos valores por los de su red WiFi
#define SSID "SEBASTIAN"                     ///< Cambia por el nombre de tu red WiFi
#define PASSWORD "arias3009"                       ///< Cambia por la contraseña de tu red WiFi

// Cambia al certificado del broker: Certificado raíz de Let's Encrypt (ISRG Root X1) en formato PEM
const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";

/*********** Fin de parametros configurables por el usuario ***********/


/* Constantes de configuración del servidor MQTT, no cambiar */
const char* mqtt_server = MQTT_SERVER;            ///< Dirección de tu servidor MQTT
const int mqtt_port = MQTT_PORT;                  ///< Puerto seguro (TLS)
const char* mqtt_user = MQTT_USER;                ///< Usuario MQTT
const char* mqtt_password = MQTT_PASSWORD;        ///< Contraseña MQTT

// Obtener la MAC Address
String macAddress = getMacAddress();
const char * client_id = macAddress.c_str();      ///< ID del cliente MQTT

// Tópicos de publicación y suscripción
String mqtt_topic_pub( String(COUNTRY) + "/" + String(STATE) + "/"+ String(CITY) + "/" + String(client_id) + "/" + String(mqtt_user) + "/out");
String mqtt_topic_sub( String(COUNTRY) + "/" + String(STATE) + "/"+ String(CITY) + "/" + String(client_id) + "/" + String(mqtt_user) + "/in");

// Convertir los tópicos a constantes de tipo char*
const char * MQTT_TOPIC_PUB = mqtt_topic_pub.c_str();
const char * MQTT_TOPIC_SUB = mqtt_topic_sub.c_str();

long long int measureTime = millis();   // Tiempo de la última medición
long long int alertTime = millis();     // Tiempo en que inició la última alerta
WiFiClientSecure espClient;             // Conexión TLS/SSL con el servidor MQTT
PubSubClient client(espClient);         // Cliente MQTT para la conexión con el servidor
time_t now;                             // Timestamp de la fecha actual.
const char* ssid = SSID;                // Cambia por el nombre de tu red WiFi
const char* password = PASSWORD;        // Cambia por la contraseña de tu red WiFi