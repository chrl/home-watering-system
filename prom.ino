
void configureProm() {
  transport.setUseTls(true);
    transport.setCerts(grafanaCert, strlen(grafanaCert));
    transport.setWifiSsid(WIFI_SSID);
    transport.setWifiPass(WIFI_PASSWORD);

    if (!transport.begin()) {
        Serial.println(transport.errmsg);
        while (true) {};
    }

    // Configure the Loki client
    lokiClient.setUrl(GC_LOKI_URL);
    lokiClient.setPath(GC_LOKI_PATH);
    lokiClient.setPort(GC_PORT);
    lokiClient.setUser(GC_LOKI_USER);
    lokiClient.setPass(GC_LOKI_PASS);

    if (!lokiClient.begin()) {
        Serial.println(lokiClient.errmsg);
        while (true) {};
    }

    // Configure the Prom client
    promClient.setUrl(GC_PROM_URL);
    promClient.setPath(GC_PROM_PATH);
    promClient.setPort(GC_PORT);
    promClient.setUser(GC_PROM_USER);
    promClient.setPass(GC_PROM_PASS);

    if (!promClient.begin()) {
        Serial.println(promClient.errmsg);
        while (true) {};
    }

    // Add our stream objects to the streams object
    streams.addStream(logger);

    // Add our TimeSeries to the WriteRequest
    for (int i = 0; i < sizeof(sensors) / sizeof(sensors[0]); i++) {
        req.addTimeSeries(*sensors[i].tsobj);
    }
}

void postProm() {
    uint64_t time;
   
    time = lokiClient.getTimeNanos();

    if (loopCounter < 2) {
        // Add log entry

        for (int i = 0; i < sizeof(sensors) / sizeof(sensors[0]); i++) {
          if (!sensors[i].tsobj->addSample(time / 1000 / 1000, sensors[i].value)) {
              Serial.println(sensors[i].tsobj->errmsg);
          }
        }

        loopCounter++;
    }
    else {
        // Send Prom
        PromClient::SendResult lres = promClient.send(req);
        if (!lres == PromClient::SendResult::SUCCESS) {
            Serial.println(promClient.errmsg);
        }
        // Reset Batches
        for (int i = 0; i < sizeof(sensors) / sizeof(sensors[0]); i++) {
            sensors[i].tsobj->resetSamples();
        }

        // Send Loki
        LokiClient::SendResult pres = lokiClient.send(streams);
        if (pres != LokiClient::SendResult::SUCCESS) {
            Serial.println(lokiClient.errmsg);
        }

        // Reset log entries
        logger.resetEntries();
        loopCounter = 0;
    }

}
