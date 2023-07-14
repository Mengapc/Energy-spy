//Programa : Arduino Ethernet Shield W5100 e HC-SR04
//Alteracoes e adaptacoes : MakerHero
//
//Baseado no programa exemplo de
//by David A. Mellis e Tom Igoe


#include <SPI.h>
#include <Ethernet.h>

//Definicoes de IP, mascara de rede e gateway
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,15,201);          //Define o endereco IP
IPAddress gateway(192,168,15,1);     //Define o gateway
IPAddress subnet(255, 255, 255, 0); //Define a máscara de rede
 
// Inicializa o servidor web na porta 80
EthernetServer server(80);
// Inicializa Client
EthernetClient clientEnvio;

// Dados do Serviço
int    HTTP_PORT   = 80;
String HTTP_METHOD = "POST";
char   HOST_NAME[] = "energyspy.azurewebsites.net";
String PATH_NAME   = "/enviar_leitura/";
int    COD_CLIENTE = 1;

// Definição das varíaveis de sensores
float tensao = 127;
float corrente = 0;
float potencia = 0;

// Frequencia de atualização do servidor
const long interval = 30 * 1000; // ms
unsigned long previousMillis = 0;  // último envio ao serviço cloud

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
  ; // wait for serial port to connect. Needed for Leonardo only
  }
  //Inicializa a interface de rede
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip, gateway, subnet);
  } else {
    Serial.println("DHCP OK");
    Serial.print("IP: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
      // print the value of each byte of the IP address:
      Serial.print(Ethernet.localIP()[thisByte], DEC);
      Serial.print("."); 
    }
  }
  server.begin();
}
 
void loop() {

  //Aguarda conexao do browser
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\r\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 2"); //Recarrega a pagina a cada 2seg
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          //Configura o texto e imprime o titulo no browser
          client.print("<font color=#FF0000><b><u>");
          client.print("Envio de informacoes pela rede utilizando Arduino");
          client.print("</u></b></font>");
          client.println("<br />");
          client.println("<h3>Ethernet</h3>");
          client.print("<b>IP:</b>");
          for (byte thisByte = 0; thisByte < 4; thisByte++) {
            // print the value of each byte of the IP address:
            client.print(Ethernet.localIP()[thisByte], DEC);
            client.print("."); 
          }
          client.println("<br/>");
          client.print("<b>Default Gateway:</b>");
          for (byte thisByte = 0; thisByte < 4; thisByte++) {
            // print the value of each byte of the IP address:
            client.print(Ethernet.gatewayIP()[thisByte], DEC);
            client.print("."); 
          }
          client.println("<br/>");
          client.println("</html>");
          break;
        }
        if (c == '\r\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
  // Leitura de sensores e envio para cloud
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) > interval) {
    previousMillis = currentMillis;
    lerSensores();
    Serial.println(corrente);
    Serial.println(potencia);
    enviarDados();
  }
}

void lerSensores() {
  corrente = random(2, 10);
  potencia = tensao * corrente;
}

// void enviarDados() {

//   Serial.println("connecting...");
//   byte server[] = { 142, 251, 128, 142 }; // Google
//   if (clientEnvio.connect("www.google.com", 80)) {
//     Serial.println("connected");
//     clientEnvio.println("GET /search?q=arduino HTTP/1.0");
//     clientEnvio.println();
//     delay(2000);
//     int i = 0;
//     while ((clientEnvio.available()) && (i < 1024)) {
//       i++;
//       char c = clientEnvio.read();
//       Serial.print(c);
//     }

//     if (!clientEnvio.connected()) {
//       Serial.println();
//       Serial.println("disconnecting.");
//       clientEnvio.stop();
//       for(;;)
//         ;
//     }

//   } else {
//     Serial.println("connection failed");
//   }
// }

void enviarDados() {
  if(clientEnvio.connect(HOST_NAME, HTTP_PORT)) {
    Serial.println("Connected to server");
  } else {
    Serial.println("connection failed");
    return;
  }
  String postContent =  "{\r\n";
  postContent.concat("   \"cod_cliente\": ");
  postContent.concat(COD_CLIENTE);
  postContent.concat(",\r\n");
  postContent.concat("   \"corrente\": ");
  postContent.concat(corrente);
  postContent.concat(",\r\n");
  postContent.concat("   \"potencia\": ");
  postContent.concat(potencia);
  postContent.concat(",\r\n");
  postContent.concat("   \"tensao\": ");
  postContent.concat(tensao);
  postContent.concat("\r\n}\r\n");

  String postRequest = "POST ";
  postRequest.concat(PATH_NAME);
  postRequest.concat(" HTTP/1.1\r\n");
  postRequest.concat("Host: ");
  postRequest.concat(HOST_NAME);
  postRequest.concat("\r\n");
  postRequest.concat("User-Agent: Arduino/1.0\r\n");
  postRequest.concat("Connection: close\r\n"); 
  postRequest.concat("Content-Type: application/json\r\n");
  postRequest.concat("Content-Length: ");
  postRequest.concat(postContent.length());
  postRequest.concat("\r\n");
  postRequest.concat("\r\n");
  postRequest.concat(postContent);
  postRequest.concat("\r\n");
  clientEnvio.println(postRequest);
  clientEnvio.println();
  Serial.println(postRequest);
  delay(2000);
  int i = 0;
  while ((clientEnvio.available()) && (i < 1024)) {
    i++;
    char c = clientEnvio.read();
    Serial.print(c);
  }
  Serial.println("");

  if (!clientEnvio.connected()) {
    // if the server's disconnected, stop the client:
    Serial.println("disconnected");
  }
  Serial.println("closing");
  clientEnvio.stop();

}

/*     Esse codigo aqui ta errado, so deixei pq, vai q vc acha algo q vc entenda kkkk
httpClient.println(“POST /api/v1/device/strategy/vars/int32s/nAnInt32 HTTP/1.1”);
httpClient.println(“Host: restpac.groov.com 3”);
httpClient.println(“Authorization: Basic xxxxxxx”) //x in place of actual creds for forum
httpClient.println();
httpClient.println(“{value" : 1989}”); .
httpClient.println();
*/


//Esse de baixo foi o que o cara do site disse ser uma solucao mas ngm comentou nada, depois disso
/*
String postContent = "{\"value\" :2}";

String postRequest = "POST /api/v1/device/strategy/vars/int32s/nAnInt32 HTTP/1.1\r\n";
postRequest += "Host: restpac.groov.com\r\n";
postRequest += "Authorization: Basic cnc6cnc=\r\n";
postRequest += "Content-Length: " + String(postContent.length()) + "\r\n";

postRequest += "\r\n" + postContent;

httpClient.print(postRequest);
*/




/* Potentiometer reading program */
/*
// Constants
#define DELAY 500 // Delay between two measurements in ms
#define VIN 5 // V power voltage
#define R 10000 //ohm resistance value

// Parameters
const int sensorPin = A0; // Pin connected to sensor

//Variables
int sensorVal; // Analog value from the sensor
float res; //resistance value

void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  sensorVal = analogRead(sensorPin);
  res=sensorRawToPhys(sensorVal);
  Serial.print(F("Raw value from sensor= "));
  Serial.println(sensorVal); // the analog reading
  Serial.print(F("Physical value from sensor = "));
  Serial.print(res); // the analog reading
  Serial.println(F(" ohm")); // the analog reading
  delay(DELAY);
}

float sensorRawToPhys(int raw){
  // Conversion rule
  float Vout = float(raw) * (VIN / float(1023));// Conversion analog to voltage
  float phys = R *((Vout))/VIN; // Conversion voltage to resistance between GND and signal
  float phys2 = R *((VIN - Vout))/VIN; // Conversion voltage to resistance between 5V and signal
  return phys;
}
*/
