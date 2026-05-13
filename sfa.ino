#include <WiFi.h>
#include <HTTPClient.h>

// ===== WIFI =====
const char* ssid = "Starlink";
const char* password = "Itesm2018$";

// ===== URL APPS SCRIPT =====
const char* scriptURL = "https://script.google.com/macros/s/AKfycbyVIqZbl4petSERD49mgNXJQ3CjjhBTjxiMylhAFHqxo2BXs_KeEl-XZ6wu-GCRQGnP/exec";

// ===== BOTON =====
const int boton = 4;

// ===== MULTICLICK =====
int contadorClicks = 0;
unsigned long ultimoClick = 0;
const unsigned long tiempoEspera = 1000;

bool ultimoEstado = HIGH;
bool mensajeInicioEnviado = false;

void setup() {
  Serial.begin(115200);
  pinMode(boton, INPUT_PULLUP);

  conectarWiFi();

  delay(2000); // esperar estabilidad de conexión

  enviarMensajeInicio();
}

void loop() {
  bool estadoActual = digitalRead(boton);

  // detectar pulsación
  if (ultimoEstado == HIGH && estadoActual == LOW) {
    contadorClicks++;
    ultimoClick = millis();
    delay(200); // debounce
  }

  ultimoEstado = estadoActual;

  // ejecutar acción tras esperar más clics
  if (contadorClicks > 0 && millis() - ultimoClick > tiempoEspera) {
    ejecutarAccion(contadorClicks);
    contadorClicks = 0;
  }
}

void conectarWiFi() {
  WiFi.begin(ssid, password);

  Serial.print("Conectando WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void enviarRequest(String url) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    Serial.println("Enviando request:");
    Serial.println(url);

    http.begin(url);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    int codigo = http.GET();

    Serial.print("HTTP code: ");
    Serial.println(codigo);

    if (codigo > 0) {
      String respuesta = http.getString();
      Serial.println("Respuesta servidor:");
      Serial.println(respuesta);
    }

    http.end();
  } else {
    Serial.println("WiFi no conectado");
  }
}

void mandarPalabra(String palabra) {
  String url = String(scriptURL) +
               "?accion=agregar&texto=" + palabra;

  enviarRequest(url);
}

void enviarMensaje() {
  String url = String(scriptURL) + "?accion=enviar";
  enviarRequest(url);
}

void enviarMensajeInicio() {
  if (!mensajeInicioEnviado) {
    String url = String(scriptURL) +
                 "?accion=agregar&texto=Bienvenido_Sign_For_All";

    enviarRequest(url);
    mensajeInicioEnviado = true;
  }
}

void ejecutarAccion(int clicks) {
  Serial.print("Clicks detectados: ");
  Serial.println(clicks);

  switch (clicks) {
    case 1:
      mandarPalabra("hola");
      break;

    case 2:
      mandarPalabra("ayuda");
      break;

    case 3:
      mandarPalabra("agua");
      break;

    case 4:
      enviarMensaje();
      break;

    default:
      Serial.println("Accion no asignada");
      break;
  }
}
