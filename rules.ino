void parseRules() {
  for (int i = 0; i < sizeof(rules) / sizeof(rules[0]); i++) {
    float value = getSensorValue(rules[i].sensorName);
    Serial.print("Value for sensor "+rules[i].sensorName+": ");
    Serial.println(value);

    if (value >= rules[i].treshold) {
      if (rules[i].triggered == false) {
        rules[i].triggered = true;
        Serial.println("Triggered rule "+rules[i].name);

        logMessage("Triggered rule "+rules[i].name);
        
        
        if (rules[i].actuator!="") {
          triggerActuator(rules[i].actuator,rules[i].seconds);
          rules[i].stopActuator = millis() + (rules[i].seconds*1000);
        }
      }
    } else {
      if (rules[i].triggered == true) {
        rules[i].triggered = false;
        Serial.println("Unriggered rule "+rules[i].name);
        logMessage("Untriggered rule "+rules[i].name);
      }      
    }
  }
}

float getSensorValue(String name) {
  for (int i = 0; i < sizeof(sensors) / sizeof(sensors[0]); i++) {
      if (sensors[i].name == name) {
        return sensors[i].value;
      }
  }
  return 0;
}

void triggerActuator(String name, int seconds)
{
    if (name == "pump") {
          digitalWrite(19, HIGH);
    }

    Serial.print("starting actuator "+name + " for ");
    Serial.print(seconds);
    Serial.println(" seconds");
}

void unTriggerActuator(String name)
{
    if (name == "pump") {
          digitalWrite(19, LOW);
    }
}

void checkActuatorsStop() {
  for (int i = 0; i < sizeof(rules) / sizeof(rules[0]); i++) {
      if (rules[i].stopActuator > 0) {
          if (millis() < rules[i].stopActuator) {
            unTriggerActuator(rules[i].actuator);
            rules[i].stopActuator = 0;
            Serial.println("stopping actuator "+rules[i].actuator);
          }
      }
  }
}

void logMessage(String message) {
  char cstr[100];
  message.toCharArray(cstr, 100);
  if (!logger.addEntry(lokiClient.getTimeNanos(), cstr, strlen(cstr))) {
            Serial.println(logger.errmsg);
  }
}
