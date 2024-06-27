#define FRONT_RED_PIN 3
#define FRONT_GREEN_PIN 5
#define FRONT_BLUE_PIN 6
#define CASE_RED_PIN 9
#define CASE_GREEN_PIN 10
#define CASE_BLUE_PIN 11
#define MIN_VALUE 0
#define MAX_VALUE 255

const int frontPanelRGBPins[3] = { FRONT_RED_PIN, FRONT_GREEN_PIN, FRONT_BLUE_PIN };
const int casePanelRGBPins[3] = { CASE_RED_PIN, CASE_GREEN_PIN, CASE_BLUE_PIN };
const int fanPin = A2;
bool casePanelState = false, frontPanelState = false, fanState = false;
bool loopControl = true;
char command;
String controlPackage;


void setup() {
  for (int i = 0; i < 3; i++) {
    pinMode(frontPanelRGBPins[i], OUTPUT);
    pinMode(casePanelRGBPins[i], OUTPUT);
  }

  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin, LOW);
  Serial.begin(9600);
}


void loop() {
  if (Serial.available()) {
    controlPackage = Serial.readString();
    command = controlPackage[0];

    if (command == 'z') {
      analogWrite(frontPanelRGBPins[0], MIN_VALUE);
      analogWrite(frontPanelRGBPins[1], MIN_VALUE);
      analogWrite(frontPanelRGBPins[2], MIN_VALUE);
      analogWrite(casePanelRGBPins[0], MIN_VALUE);
      analogWrite(casePanelRGBPins[1], MIN_VALUE);
      analogWrite(casePanelRGBPins[2], MIN_VALUE);
    } else if (command == 't') {
      // Fan Control
      fanState = !fanState;
      digitalWrite(fanPin, fanState);
    } else if (command == 's') {
      // Music Mode
      while (loopControl) {
        char value = Serial.read();

        if (value == 'c' || value == 'z') {
          analogWrite(frontPanelRGBPins[0], MIN_VALUE);
          analogWrite(frontPanelRGBPins[1], MIN_VALUE);
          analogWrite(frontPanelRGBPins[2], MIN_VALUE);
          break;
        }

        int soundValue = analogRead(A0);

        if (soundValue > 0 && soundValue < 10) {
          analogWrite(frontPanelRGBPins[0], 255);
          analogWrite(frontPanelRGBPins[1], MIN_VALUE);
          analogWrite(frontPanelRGBPins[2], MIN_VALUE);
        } else if (soundValue >= 10 && soundValue < 20) {
          analogWrite(frontPanelRGBPins[0], MIN_VALUE);
          analogWrite(frontPanelRGBPins[1], 255);
          analogWrite(frontPanelRGBPins[2], MIN_VALUE);
        } else {
          analogWrite(frontPanelRGBPins[0], MIN_VALUE);
          analogWrite(frontPanelRGBPins[1], MIN_VALUE);
          analogWrite(frontPanelRGBPins[2], 255);
        }
      }
    } else if (command == 'o') {
      // Front Panel Lighting
      panelLighting(controlPackage, frontPanelRGBPins);
    } else if (command == 'k') {
      // Case Panel Lighting
      panelLighting(controlPackage, casePanelRGBPins);
    } else if (command == 'm') {
      // Mod Values
      if (controlPackage[1] == '1') {
        modes(controlPackage, frontPanelRGBPins);
      }

      if (controlPackage[1] == '2') {
        modes(controlPackage, casePanelRGBPins);
      }
    }
  }
}

int parseValue(char v1, char v2, char v3) {
  String value;
  value = (v1 - 48) * 100 + (v2 - 48) * 10 + (v3 - 48);

  return value.toInt();
};

void panelLighting(String v, int rgbPins[3]) {
  if (v[1] == '1') {
    int value = parseValue(v[2], v[3], v[4]);
    analogWrite(rgbPins[0], value);
  } else {
    analogWrite(rgbPins[0], MIN_VALUE);
  }

  if (v[5] == '1') {
    int value = parseValue(v[6], v[7], v[8]);
    analogWrite(rgbPins[1], value);
  } else {
    analogWrite(rgbPins[1], MIN_VALUE);
  }

  if (v[9] == '1') {
    int value = parseValue(v[10], v[11], v[12]);
    analogWrite(rgbPins[2], value);
  } else {
    analogWrite(rgbPins[2], MIN_VALUE);
  }
}

void modes(String v, int rgbPins[3]) {
  while (loopControl) {
    char value = Serial.read();

    if (value == 'd' || value == 'z') {
      break;
    }

    switch (v[2]) {
      case '1':
        FadeEffect(0, rgbPins);
        break;
      case '2':
        FadeEffect(1, rgbPins);
        break;
      case '3':
        FadeEffect(2, rgbPins);
        break;
      case '4':
        MixedEffect(0, rgbPins);
        break;
      case '5':
        MixedEffect(1, rgbPins);
        break;
      case '6':
        MixedEffect(2, rgbPins);
        break;
      case '7':
        PoliceSirenEffect(rgbPins);
        break;
      default:
        break;
    }
  }
}

void FadeEffect(int speed, int rgbPins[3]) {
  int amount = 5, brightness = 0, i = 0, selectedColor = 0;

  analogWrite(rgbPins[0], MIN_VALUE);
  analogWrite(rgbPins[1], MIN_VALUE);
  analogWrite(rgbPins[2], MIN_VALUE);

  while (selectedColor < 3) {
    i = 0;

    for (brightness; i < 2; brightness += amount) {
      analogWrite(rgbPins[selectedColor], brightness);

      if (brightness < MIN_VALUE || brightness > MAX_VALUE) {
        amount = -amount;
        i++;
      }

      if (speed == 0) {
        delay(50);
      } else if (speed == 1) {
        delay(30);
      } else {
        delay(10);
      }
    }

    selectedColor++;
  }
}

void MixedEffect(int speed, int rgbPins[3]) {
  int delayAmonunt = 50;

  if (speed == 0) {
    delayAmonunt = 50;
  } else if (speed == 1) {
    delayAmonunt = 30;
  } else {
    delayAmonunt = 10;
  }

  for (int i = 0; i < 30; i++) {
    for (int p = 0; p < 3; p++) {
      analogWrite(rgbPins[p], MAX_VALUE);
      delay(delayAmonunt);
      analogWrite(rgbPins[p], MIN_VALUE);
      delay(delayAmonunt);
    }
  }
}

void PoliceSirenEffect(int rgbPins[3]) {
  int delayAmount = 80;

  for (int i = 0; i < 2; i++) {
    analogWrite(rgbPins[0], MAX_VALUE);
    analogWrite(rgbPins[2], MAX_VALUE);
    delay(delayAmount);
    analogWrite(rgbPins[0], MIN_VALUE);
    analogWrite(rgbPins[2], MIN_VALUE);
    delay(delayAmount);
  }
}
