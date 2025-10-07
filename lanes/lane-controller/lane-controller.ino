#include <Wire.h>
#include <VL53L0X.h>

constexpr uint8_t NUM_SENSORS = 6;
constexpr uint8_t NUM_GROUPS = 3;
constexpr uint8_t LIGHT_PIN = 13;  // Lane beacon

const uint8_t XSHUT_PINS[NUM_SENSORS] = { 4, 5, 6, 7, 10, 11 };
const uint8_t GROUP_START[NUM_GROUPS] = { 0, 4, 5 };
const uint8_t GROUP_END[NUM_GROUPS] = { 3, 4, 5 };
constexpr uint16_t MIN_RANGE_MM = 75;
constexpr uint16_t MAX_RANGE_MM = 100;
constexpr unsigned long FLASH_INTERVAL_MS = 25;

VL53L0X sensors[NUM_SENSORS];
bool sensorInitSuccessful[NUM_SENSORS] = { false };
unsigned long lastTriggeredTime[NUM_GROUPS] = { 0, 0, 0 };
char lastCommand = 'N';
const char laneAddress = '1';  // Change this per installation

void reportSensorStatus();

void setup() {
    Serial.begin(9600);   // USB debug console
    Serial1.begin(9600);  // RS-485 or UART back to the Teensy brain

    pinMode(LIGHT_PIN, OUTPUT);
    digitalWrite(LIGHT_PIN, LOW);

    Serial.print("Booting lane controller at address ");
    Serial.println(laneAddress);

    for (uint8_t i = 0; i < NUM_SENSORS; ++i) {
        pinMode(XSHUT_PINS[i], OUTPUT);
        digitalWrite(XSHUT_PINS[i], LOW);
    }

    delay(10);

    for (uint8_t i = 0; i < NUM_SENSORS; ++i) {
        digitalWrite(XSHUT_PINS[i], HIGH);
        delay(10);

        if (!sensors[i].init(true)) {
            Serial.print("Sensor ");
            Serial.print(i);
            Serial.println(" failed to initialize!");
            while (true) {
                digitalWrite(LIGHT_PIN, !digitalRead(LIGHT_PIN));
                delay(FLASH_INTERVAL_MS * 4);
            }
        }

        sensorInitSuccessful[i] = true;
        sensors[i].setAddress(0x30 + i);
        sensors[i].startContinuous();

        Serial.print("Sensor ");
        Serial.print(i);
        Serial.println(" initialized successfully with its respective I2C address!");

        digitalWrite(LIGHT_PIN, HIGH);
        delay(FLASH_INTERVAL_MS);
        digitalWrite(LIGHT_PIN, LOW);
        delay(FLASH_INTERVAL_MS);
    }
}

void loop() {
    digitalWrite(LIGHT_PIN, LOW);

    if (Serial1.available() > 0) {
        const char command = Serial1.read();

        switch (command) {
            case 'H':
                digitalWrite(LIGHT_PIN, HIGH);
                break;
            case 'T':
                for (int d = 0; d < 35; ++d) {
                    digitalWrite(LIGHT_PIN, !digitalRead(LIGHT_PIN));
                    delay(FLASH_INTERVAL_MS * 20);
                }
                break;
            case 'O':
                digitalWrite(LIGHT_PIN, LOW);
                break;
            case 'D':
                // Reserved for future sensor dumps
                break;
            case 'R':
                reportSensorStatus();
                break;
            default:
                break;
        }
    }

    for (uint8_t g = 0; g < NUM_GROUPS; ++g) {
        for (uint8_t i = GROUP_START[g]; i <= GROUP_END[g]; ++i) {
            const uint16_t range = sensors[i].readRangeContinuousMillimeters();

            if (sensors[i].timeoutOccurred()) {
                Serial.print("Sensor ");
                Serial.print(i);
                Serial.println(" reading timed out!");
                continue;
            }

            char commandToSend = 'N';

            if (range > MIN_RANGE_MM && range < MAX_RANGE_MM &&
                millis() - lastTriggeredTime[g] > (FLASH_INTERVAL_MS * 2)) {
                digitalWrite(LIGHT_PIN, LOW);

                if (g == 0) {
                    commandToSend = 'F';
                } else if (g == 1) {
                    commandToSend = 'M';
                } else {
                    commandToSend = 'L';
                }

                lastTriggeredTime[g] = millis();
            }

            lastCommand = commandToSend;
            Serial1.write(lastCommand);

            const unsigned long startTime = millis();
            bool acknowledgmentReceived = false;

            while (millis() - startTime < (FLASH_INTERVAL_MS * 4)) {
                if (!Serial1.available()) {
                    continue;
                }

                const char ack = Serial1.read();
                if (ack == 'A') {
                    acknowledgmentReceived = true;
                    break;
                }

                if (ack == 'R') {
                    reportSensorStatus();
                }
            }

            if (!acknowledgmentReceived) {
                Serial.println("No acknowledgment received for lane command.");
            }

            Serial.print("Sensor ");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(range);
            Serial.println(" mm");
        }
    }

    delay(FLASH_INTERVAL_MS * 2);
}

void reportSensorStatus() {
    for (uint8_t i = 0; i < NUM_SENSORS; ++i) {
        if (sensorInitSuccessful[i]) {
            Serial.println("SENSOR_OK");
        } else {
            Serial.println("SENSOR_ERROR");
        }
    }
}
