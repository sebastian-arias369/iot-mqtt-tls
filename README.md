# IoT MQTT TLS

Este proyecto es un ejemplo de cómo implementar un cliente IoT usando **MQTT** con soporte de **TLS** para una conexión segura. Está desarrollado en **C/C++** para dispositivos basados en **ESP32** y utiliza el protocolo **MQTT** para enviar y recibir datos entre el dispositivo y un broker MQTT. Además, este proyecto incluye autenticación mediante usuario/contraseña y utiliza un certificado de **Let's Encrypt** para garantizar una conexión segura (puede usarse cualquier otro certificado).

## Características

- **MQTT sobre TLS**: Comunicación segura mediante el protocolo MQTT con soporte de TLS.
- **Autenticación**: Uso de usuario y contraseña para conectarse al broker MQTT.
- **Certificados**: Uso del certificado raíz de Let's Encrypt (ISRG Root X1) para la autenticación del servidor.
- **Conexión automática**: Manejo de desconexiones y reconexión automática.
- **Publicación/Suscripción**: Soporte para publicar y suscribirse a tópicos definidos.

## Requisitos

- **ESP32** con soporte de **Wi-Fi**.
- **Broker MQTT** que soporte conexiones TLS (por ejemplo, Mosquitto).
- **Certificado raíz de Let's Encrypt** para la autenticación TLS.

### Dependencias

- **PubSubClient**: Librería MQTT para ESP32.
- **WiFiClientSecure**: Cliente para conexiones seguras sobre Wi-Fi.
- **Arduino Framework**: Para desarrollo de IoT con ESP32.

## Instalación

### 1. Configuración del broker MQTT

Debes tener un broker MQTT configurado con soporte para TLS. Si estás utilizando **Mosquitto**, asegúrese de que esté configurado con un certificado válido (por ejemplo, el certificado de **Let's Encrypt**).

### 2. Clonar el repositorio

```bash
git clone https://github.com/alvaro-salazar/iot-mqtt-tls.git
cd iot-mqtt-tls
```

### 3. Configurar el ESP32

Modifica los valores de conexión en el archivo principal para el ESP32:

```cpp
#define SSID "NOMBRE_DE_TU_RED"
#define PASSWORD "CONTRASEÑA_DE_TU_RED"
#define MQTT_SERVER "mqtt.tu-servidor.com"
#define MQTT_PORT 8883
#define MQTT_USER "device1"
#define MQTT_PASSWORD "a1b2c3d4"
```

Asegúrese de añadir el certificado raíz de Let's Encrypt (ISRG Root X1) en tu código:

```cpp
const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"... tu certificado aquí ...\n" \
"-----END CERTIFICATE-----\n";
```

### 4. Compilación y carga

Compila y sube el código a tu ESP32 usando tu entorno de desarrollo preferido (Arduino IDE o PlatformIO).

### 5. Ejecución

Una vez que el código esté en el ESP32, este se conectará a la red Wi-Fi y al broker MQTT utilizando TLS. Se publicarán y recibirán mensajes en los tópicos configurados.

## Tópicos MQTT

El formato de los tópicos utilizados es:

```plaintext
<pais>/<estado>/<ciudad>/<device-id>/<usuario>/out  // Para publicaciones
<pais>/<estado>/<ciudad>/<device-id>/<usuario>/in   // Para suscripciones
```

### Ejemplo:

```plaintext
colombia/valle/tulua/ESP32-CC50E3B65DD/device1/out
```

## Personalización

Puedes modificar el archivo para cambiar los tópicos, las credenciales del broker y el comportamiento del dispositivo.

## Manejo de Errores

Si el ESP32 pierde la conexión con el broker, el sistema intentará reconectar automáticamente. Puedes revisar los logs en la consola serie para diagnosticar cualquier problema con la conexión.

## Contribuciones

¡Las contribuciones son bienvenidas! Si deseas colaborar, por favor abre un **issue** o envía un **pull request** con tus sugerencias.

## Licencia

Este proyecto está bajo la licencia MIT. Para más detalles, consulta el archivo [LICENSE](LICENSE).
