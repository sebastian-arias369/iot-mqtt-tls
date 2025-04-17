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

#include <libota.h>
#include <libiot.h>

// Variables externas
extern PubSubClient client;

/**
 * Configuración inicial de OTA
 */
void setupOTA() {
    // Inicializa la biblioteca Update
    Update.onProgress([](unsigned int progress, unsigned int total) {
        // Muestra el progreso de la actualización
        Serial.printf("Progreso: %u%%\r", (progress * 100) / total);
    });
    
    // Suscribe al tópico de OTA
    subscribeToOTATopic();
    
    Serial.println("OTA configurado correctamente");
}

/**
 * Suscribe al tópico de OTA
 */
void subscribeToOTATopic() {
    if (client.connected()) {
        client.subscribe(OTA_TOPIC);
        Serial.println("Suscrito al tópico OTA: " + String(OTA_TOPIC));
    } else {
        Serial.println("No se pudo suscribir al tópico OTA: Cliente MQTT no conectado");
    }
}

/**
 * Verifica si hay actualizaciones disponibles
 * Procesa el mensaje JSON recibido en el tópico OTA
 */
void checkOTAUpdate(const char* payload) {
    Serial.println("Mensaje OTA recibido: " + String(payload));
    
    // Parsea el JSON
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
        Serial.print("Error al parsear JSON: ");
        Serial.println(error.c_str());
        return;
    }
    
    // Verifica si el JSON contiene la URL
    if (doc.containsKey("url")) {
        const char* url = doc["url"];
        const char* version = doc["version"] | "desconocida";
        
        Serial.println("Nueva versión disponible: " + String(version));
        Serial.println("URL de actualización: " + String(url));
        
        // Realiza la actualización OTA
        if (performOTAUpdate(url)) {
            Serial.println("Actualización completada. Reiniciando...");
            ESP.restart();
        } else {
            Serial.println("Error en la actualización OTA");
        }
    } else {
        Serial.println("Mensaje OTA inválido: No contiene URL");
    }
}

/**
 * Realiza la actualización OTA
 * Descarga el firmware desde la URL proporcionada
 */
bool performOTAUpdate(const char* url) {
    Serial.println("Iniciando actualización OTA desde: " + String(url));
    
    // Inicializa HTTP
    HTTPClient http;
    http.begin(url);
    
    // Realiza la petición HTTP
    int httpCode = http.GET();
    
    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("Error HTTP: %d\n", httpCode);
        http.end();
        return false;
    }
    
    // Obtiene el tamaño del firmware
    int contentLength = http.getSize();
    Serial.printf("Tamaño del firmware: %d bytes\n", contentLength);
    
    // Inicia la actualización
    if (!Update.begin(contentLength)) {
        Serial.println("No hay espacio suficiente para la actualización");
        http.end();
        return false;
    }
    
    // Buffer para descargar el firmware
    uint8_t buff[OTA_BUFFER_SIZE] = { 0 };
    WiFiClient * stream = http.getStreamPtr();
    
    // Descarga y escribe el firmware
    size_t written = 0;
    while (written < contentLength) {
        // Lee datos del servidor
        size_t available = stream->available();
        if (available) {
            size_t bytesToRead = min(available, sizeof(buff));
            size_t bytesRead = stream->readBytes(buff, bytesToRead);
            
            // Escribe los datos en la memoria flash
            if (Update.write(buff, bytesRead) != bytesRead) {
                Serial.println("Error al escribir en la memoria flash");
                http.end();
                return false;
            }
            
            written += bytesRead;
        }
        
        // Pequeña pausa para evitar sobrecarga
        delay(1);
    }
    
    // Finaliza la actualización
    if (Update.end()) {
        Serial.println("Actualización completada correctamente");
        http.end();
        return true;
    } else {
        Serial.println("Error al finalizar la actualización: " + String(Update.errorString()));
        http.end();
        return false;
    }
}
