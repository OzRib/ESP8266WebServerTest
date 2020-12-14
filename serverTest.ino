#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define ssid "*************"
#define psswd "*************"
#define led LED_BUILTIN

ESP8266WebServer server(80);

void showRequest(){
  Serial.println("Nova requisição: ");
  server.method() == HTTP_GET ? Serial.print("GET"):Serial.print("POST");
  Serial.println(server.uri());
}

void handleRoot(){
  server.sendHeader("Connection", "keep-alive");
  server.send(200, "text/html", F("<!DOCTYPE html>\n\
  <html>\n\
  <head>\n\
  <title>ESP8266</title>\n\
  <meta charset='UTF-8'>\n\
  <style type='text/css'>*{font-size:45px;}\n\
  :root{box-sizing:border-box;height: 100vh;width:100vw;}\n\
  html, body{background:rgb(80,80,80);display:flex;justify-content:center;}\n\
  #container{align-self:center;justify-content:center;display:flex;height:80vh;width:70vw;border:solid black 1px;border-radius:40px;background:rgb(110,170,255);}\n\
  .button{margin-top:1rem;height:5%;min-width:40%; border:solid black 1px; border-radius:20px;}\n\
  #b1{background:rgb(250,250,250);color='white'}\n\
  </style>\n\
  </head>\n\
  <body><div id='container'>\n\
  <input type='button' onClick='req.led()' value='Clique aqui para trocar o led' id='b1' class='button'>\n\
  </button>\n\
  </div>\n\
  </body>\n\
  </html>\n\
  <script type='text/javascript'>\n\
  const req={\n\
    'led':() =>{\n\
      const bt=document.getElementById('b1');\n\
      fetch('led').then(resp=>{return resp.text()}).then(txt=>{if(txt[txt.length-1] == true){bt.value='Ligado';bt.style.background='green'}else{bt.value='Desligado';\n\
      bt.style.background='red';}})\n\
    }\n\
  }</script>"));
  showRequest();
}
void turnLed(){
  digitalWrite(led, !digitalRead(led));
  String message = "led status: ";
  message += !digitalRead(led);
  server.send(200, "text/plain", message);
  showRequest();
}

void handleNotFound(){
  String message = "File not found\n\nURL: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for(uint8_t i=0; i<server.args(); i++){
    message += " "+server.argName(i)+server.arg(i)+"\n";
  }
  server.send(404, "text/plain", message);
  showRequest();
  Serial.println("Requisição inválida");
}

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, psswd);
  Serial.println();
  while(WiFi.status() != WL_CONNECTED){
    Serial.print('.');
    delay(400);
  }
  Serial.println();
  Serial.print(F("Conectado a: "));
  Serial.println(ssid);
  server.begin();
  Serial.println(F("Servidor iniciado"));
  Serial.print(F("Endereço IP: "));
  Serial.println(WiFi.localIP());
  if(MDNS.begin("esp8266")){
    Serial.println(F("MDNS responder iniciado"));  
  }
  server.on("/", handleRoot);
  server.on("/led", turnLed);
  server.onNotFound(handleNotFound);
  server.begin();
  for(byte i=0;i<4;i++){
    digitalWrite(led, !digitalRead(led));
    delay(100);
  }
}

void loop() {
  server.handleClient();
  MDNS.update();
}
