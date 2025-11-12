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
constexpr unsigned long SENSOR_BOOT_DELAY_MS = 10;
constexpr unsigned long COMMAND_COOLDOWN_MS = FLASH_INTERVAL_MS * 2;
constexpr unsigned long ACK_TIMEOUT_MS = FLASH_INTERVAL_MS * 4;
constexpr size_t MAX_SERIAL_BYTES_PER_LOOP = 8;

VL53L0X sensors[NUM_SENSORS];
bool sensorInitSuccessful[NUM_SENSORS] = { false };
unsigned long lastTriggeredTime[NUM_GROUPS] = { 0, 0, 0 };
char lastCommand = 'N';
const char laneAddress = '1';  // Change this per installation

enum class InitPhase : uint8_t {
    HoldReset,
    WaitAfterReset,
    PowerSensor,
    WaitAfterPowerOn,
    ConfigureSensor,
    Complete,
    Failed
};

struct PendingCommand {
    bool awaitingAck = false;
    unsigned long sentAt = 0;
    char command = 'N';
};

struct BeaconTask {
    bool active = false;
    bool infinite = false;
    unsigned long nextToggle = 0;
    unsigned long interval = 0;
    uint16_t remainingToggles = 0;
};

InitPhase initPhase = InitPhase::HoldReset;
uint8_t currentInitSensor = 0;
unsigned long initPhaseDeadline = 0;
BeaconTask beaconTask;
bool beaconState = false;
PendingCommand pendingCommands[NUM_GROUPS];

void reportSensorStatus();
void serviceInitialization(unsigned long now);
void serviceBeacon(unsigned long now);
void scheduleBeaconToggle(uint16_t toggles, unsigned long interval, bool forceHigh, bool infinite = false);
void cancelBeaconSequence();
void handleIncomingByte(char incoming, unsigned long now);
void resolveAck();
void serviceSensorGroups(unsigned long now);
void maybeSendGroupCommand(uint8_t group, char command, unsigned long now);
void serviceAckTimeouts(unsigned long now);
void setBeacon(bool state);

void setup() {
    Serial.begin(9600);   // USB debug console
    Serial1.begin(9600);  // RS-485 or UART back to the Teensy brain
    Wire.begin();

    pinMode(LIGHT_PIN, OUTPUT);
    setBeacon(false);

    Serial.print("Booting lane controller at address ");
    Serial.println(laneAddress);

    for (uint8_t i = 0; i < NUM_SENSORS; ++i) {
        pinMode(XSHUT_PINS[i], OUTPUT);
        digitalWrite(XSHUT_PINS[i], LOW);
    }

    initPhase = InitPhase::HoldReset;
    serviceInitialization(millis());
}

void loop() {
    const unsigned long now = millis();

    serviceInitialization(now);
    serviceBeacon(now);
    serviceAckTimeouts(now);

    size_t serialBudget = MAX_SERIAL_BYTES_PER_LOOP;
    while (Serial1.available() > 0 && serialBudget-- > 0) {
        handleIncomingByte(static_cast<char>(Serial1.read()), now);
    }

    if (initPhase == InitPhase::Complete) {
        serviceSensorGroups(now);
    }
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

void serviceInitialization(unsigned long now) {
    switch (initPhase) {
        case InitPhase::HoldReset:
            initPhase = InitPhase::WaitAfterReset;
            initPhaseDeadline = now + SENSOR_BOOT_DELAY_MS;
            break;
        case InitPhase::WaitAfterReset:
            if (now >= initPhaseDeadline) {
                initPhase = InitPhase::PowerSensor;
            }
            break;
        case InitPhase::PowerSensor:
            if (currentInitSensor >= NUM_SENSORS) {
                initPhase = InitPhase::Complete;
                cancelBeaconSequence();
                setBeacon(false);
                break;
            }

            digitalWrite(XSHUT_PINS[currentInitSensor], HIGH);
            initPhase = InitPhase::WaitAfterPowerOn;
            initPhaseDeadline = now + SENSOR_BOOT_DELAY_MS;
            break;
        case InitPhase::WaitAfterPowerOn:
            if (now >= initPhaseDeadline) {
                initPhase = InitPhase::ConfigureSensor;
            }
            break;
        case InitPhase::ConfigureSensor: {
            VL53L0X& sensor = sensors[currentInitSensor];
            if (!sensor.init(true)) {
                Serial.print("Sensor ");
                Serial.print(currentInitSensor);
                Serial.println(" failed to initialize!");
                scheduleBeaconToggle(0, FLASH_INTERVAL_MS * 4, true, true);
                initPhase = InitPhase::Failed;
                break;
            }

            sensorInitSuccessful[currentInitSensor] = true;
            sensor.setAddress(0x30 + currentInitSensor);
            sensor.startContinuous();

            Serial.print("Sensor ");
            Serial.print(currentInitSensor);
            Serial.println(" initialized successfully with its respective I2C address!");

            setBeacon(true);
            scheduleBeaconToggle(1, FLASH_INTERVAL_MS, true);

            ++currentInitSensor;
            initPhase = InitPhase::PowerSensor;
            break;
        }
        case InitPhase::Complete:
        case InitPhase::Failed:
            break;
    }
}

void serviceBeacon(unsigned long now) {
    if (!beaconTask.active) {
        return;
    }

    if (now < beaconTask.nextToggle) {
        return;
    }

    setBeacon(!beaconState);

    if (!beaconTask.infinite) {
        if (beaconTask.remainingToggles > 0) {
            --beaconTask.remainingToggles;
        }

        if (beaconTask.remainingToggles == 0) {
            beaconTask.active = false;
            return;
        }
    }

    beaconTask.nextToggle = now + beaconTask.interval;
}

void scheduleBeaconToggle(uint16_t toggles, unsigned long interval, bool forceHigh, bool infinite) {
    if (forceHigh) {
        setBeacon(true);
    }

    beaconTask.active = true;
    beaconTask.infinite = infinite;
    beaconTask.interval = interval;
    beaconTask.remainingToggles = toggles;
    beaconTask.nextToggle = millis() + interval;
}

void cancelBeaconSequence() {
    beaconTask.active = false;
    beaconTask.infinite = false;
    beaconTask.remainingToggles = 0;
}

void handleIncomingByte(char incoming, unsigned long now) {
    switch (incoming) {
        case 'H':
            cancelBeaconSequence();
            setBeacon(true);
            break;
        case 'T':
            scheduleBeaconToggle(35, FLASH_INTERVAL_MS * 20, !beaconState);
            break;
        case 'O':
            cancelBeaconSequence();
            setBeacon(false);
            break;
        case 'D':
            // Reserved for future sensor dumps
            break;
        case 'R':
            reportSensorStatus();
            break;
        case 'A':
            resolveAck();
            break;
        default:
            // Mystery commands are ignored but logged for curious minds.
            Serial.print("Unhandled command from host: ");
            Serial.println(incoming);
            break;
    }
}

void resolveAck() {
    for (uint8_t g = 0; g < NUM_GROUPS; ++g) {
        if (pendingCommands[g].awaitingAck) {
            pendingCommands[g].awaitingAck = false;
            Serial.print("Received acknowledgment for command ");
            Serial.println(pendingCommands[g].command);
            break;
        }
    }
}

void serviceSensorGroups(unsigned long now) {
    for (uint8_t g = 0; g < NUM_GROUPS; ++g) {
        if (pendingCommands[g].awaitingAck) {
            continue;
        }

        for (uint8_t i = GROUP_START[g]; i <= GROUP_END[g]; ++i) {
            const uint16_t range = sensors[i].readRangeContinuousMillimeters();

            if (sensors[i].timeoutOccurred()) {
                Serial.print("Sensor ");
                Serial.print(i);
                Serial.println(" reading timed out!");
                continue;
            }

            Serial.print("Sensor ");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(range);
            Serial.println(" mm");

            char commandToSend = 'N';

            if (range > MIN_RANGE_MM && range < MAX_RANGE_MM &&
                now - lastTriggeredTime[g] > COMMAND_COOLDOWN_MS) {
                setBeacon(false);

                if (g == 0) {
                    commandToSend = 'F';
                } else if (g == 1) {
                    commandToSend = 'M';
                } else {
                    commandToSend = 'L';
                }

                lastTriggeredTime[g] = now;
            }

            if (commandToSend != 'N') {
                maybeSendGroupCommand(g, commandToSend, now);
                break;
            }
        }
    }
}

void maybeSendGroupCommand(uint8_t group, char command, unsigned long now) {
    Serial1.write(command);
    lastCommand = command;

    pendingCommands[group].awaitingAck = true;
    pendingCommands[group].sentAt = now;
    pendingCommands[group].command = command;
}

void serviceAckTimeouts(unsigned long now) {
    for (uint8_t g = 0; g < NUM_GROUPS; ++g) {
        PendingCommand& pending = pendingCommands[g];
        if (!pending.awaitingAck) {
            continue;
        }

        if (now - pending.sentAt > ACK_TIMEOUT_MS) {
            Serial.print("No acknowledgment received for lane command ");
            Serial.print(pending.command);
            Serial.print(" from group ");
            Serial.println(g);
            pending.awaitingAck = false;
        }
    }
}

void setBeacon(bool state) {
    if (beaconState == state) {
        return;
    }

    digitalWrite(LIGHT_PIN, state ? HIGH : LOW);
    beaconState = state;
}
