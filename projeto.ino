#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Variaveis
#define DEBUG
int const rele = 8;
int const sensor = 7;
bool irrigar = false;
String strMSG = "0";
char mensagem[30];

// WiFi
const char *ssid = "Subrabu"; // Enter your WiFi name
const char *password = "Subrabinho";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "lfe5be31.ala.us-east-1.emqxsl.com";
const char *topic = "test";
const char *mqtt_username = "teste123";
const char *mqtt_password = "teste123";
const int mqtt_port = 8883; // port of MQTT over TLS/SSL

// init wifi client
WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  Serial.begin(9600);
  pinMode(rele, OUTPUT);
  pinMode(sensor, INPUT);

  digitalWrite(rele, LOW);
  

  WiFi.begin(ssid, password);
  Serial.print("Entrando no Setup");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEBUG
    Serial.println("Aguarde! Conectando ao WiFi...");
#endif
  }
#ifdef DEBUG
  Serial.println("Conectado na rede WiFi com sucesso!");
#endif

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
#ifdef DEBUG
    Serial.println("Conectando ao Broker MQTT...");
#endif

    if (client.connect("ESP8266Client", mqtt_username, mqtt_password )) {
#ifdef DEBUG
      Serial.println("Conectado com sucesso");
#endif

    } else {
#ifdef DEBUG
      Serial.print("falha estado  ");
      Serial.print(client.state());
#endif
      delay(2000);
    }
  }

  //subscreve no tópico
  client.subscribe(topic);

}

void loop() {

  if (!client.connected()) {
    Serial.print("Passou no Connected");
    reconect();

  }

   irrigar = digitalRead(sensor);

  if(irrigar){
    digitalWrite(rele, LOW);
    
     //Enviando Mensagem ao Broker
    sprintf(mensagem, "0"); //Definindo o valor zero como parâmetro para o Broker mostrar uma imagem "vermelha"
    Serial.print("Mensagem enviada: ");
    Serial.println(mensagem);
    client.publish("Solo seco", mensagem);
    Serial.println("Mensagem enviada com sucesso...");

  }else{
    digitalWrite(rele, HIGH);

   //Enviando Mensagem ao Broker
    sprintf(mensagem, "1"); //Definindo o valor UM como parâmetro para o Broker mostrar uma imagem "verde"
    Serial.print("Mensagem enviada: ");
    Serial.println(mensagem); 
    client.publish("Regando", mensagem);
    Serial.println("Mensagem enviada com sucesso...");
  }

  delay(2000);
  client.loop();

}

void callback(char* topic, byte* payload, unsigned int length) {

  //Armazenando mensagem recebida em uma string
  payload[length] = '\0';
  strMSG = String((char*)payload);

#ifdef DEBUG
  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("-----------------------");
#endif

}

//Função pra reconectar ao servidor MQTT
void reconect() {
  //Enquanto estiver desconectado
  while (!client.connected()) {
#ifdef DEBUG
    Serial.print("Tentando conectar ao servidor MQTT");
#endif

    bool conectado = strlen(mqtt_username) > 0 ?
                     client.connect("ESP8266Client", mqtt_username, mqtt_password) :
                     client.connect("ESP8266Client");

    if (conectado) {
#ifdef DEBUG
      Serial.println("Conectado!");
#endif
      //Subscreve no tópico
      client.subscribe(topic, 1); //nivel de qualidade: QoS 1
    } else {
#ifdef DEBUG
      Serial.println("Falha durante a conexão.Code: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 10 s");
#endif
      //Aguarda 2 segundos
      delay(2000);
    }
  }
}
