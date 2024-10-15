#include <WiFiClient.h>
#include <WiFi.h>
#include <libwifi.h>
#include <libdisplay.h>



/**
 * Verifica si el dispositivo está conectado al WiFi.
 * Si no está conectado intenta reconectar a la red.
 * Si está conectado, intenta conectarse a MQTT si aún 
 * no se tiene conexión.
 */
void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED){
    Serial.print("Probando WiFi");
    while (WiFi.waitForConnectResult() != WL_CONNECTED){
      WiFi.begin(ssid, password);
      Serial.print(".");
      displayNoSignal();
      delay(10);
    }
    Serial.println("Conectado");
  }
}

/**
 * Imprime en consola la cantidad de redes WiFi disponibles y
 * sus nombres.
 */
void listWiFiNetworks() {
  int numberOfNetworks = WiFi.scanNetworks();
  Serial.println("\nNumber of networks: ");
  Serial.print(numberOfNetworks);
  for(int i =0; i<numberOfNetworks; i++){
      Serial.println(WiFi.SSID(i));
 
  }
}

/**
 * Adquiere la dirección MAC del dispositivo y la retorna en formato de cadena.
 */
String getHostname() {
  uint8_t mac[6];
  char macStr[18];
  WiFi.macAddress(mac);
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", 
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(macStr);
}

/**
 * Inicia el servicio de WiFi e intenta conectarse a la red WiFi específicada en las constantes.
 */
void startWiFi(String hostname) {
  if(hostname.length() == 0) hostname = getHostname();
  WiFi.hostname(hostname);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("(\n\nIntentando conectar a SSID: ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    switch(WiFi.status()) {
    case WL_NO_SSID_AVAIL:
      Serial.print("\nNo se encuentra la red WiFi: ");
      Serial.print(ssid);
      break;
    case WL_CONNECTION_LOST:
      Serial.println("\nSe perdio la conexion con la red WiFi.");
      break;
    case WL_DISCONNECTED:
      Serial.println("\nSe desconecto de la red WiFi.");
      break;
    case WL_CONNECT_FAILED:
      Serial.println("\nNo se ha logrado conectar con la red, resetee el node y vuelva a intentar");
      break;
    }
    WiFi.begin(ssid, password);
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Conectado!");
}
