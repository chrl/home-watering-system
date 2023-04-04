void initSensors() {
    for (int i = 0; i < sizeof(sensors) / sizeof(sensors[0]); i++) {
      sensors[i].tsobj = new TimeSeries(5, (char*) sensors[i].name.c_str(), String("{job=\"esp32\"}").c_str());

      if (sensors[i].type == "dht.temperature") {
         dht.setup(sensors[i].pin, DHTesp::DHT22); // Connect DHT sensor to GPIO 4
      }
      
    }
}

void fillSensorData(){
   for (int i = 0; i < sizeof(sensors) / sizeof(sensors[0]); i++) {
      if (sensors[i].type == "dht.temperature") {
          sensors[i].value = dht.getTemperature();
      }
      if (sensors[i].type == "dht.humidity") {
          sensors[i].value = dht.getHumidity();
      }
      if (sensors[i].type == "analog") {
          sensors[i].value = analogRead(sensors[i].pin);
      }
      if (sensors[i].type == "system.millis") {
          sensors[i].value = millis();
      }
      if (sensors[i].type == "system.memfree") {
          sensors[i].value = esp_get_free_heap_size();
      }

   }
  
}
