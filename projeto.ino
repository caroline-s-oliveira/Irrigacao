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
