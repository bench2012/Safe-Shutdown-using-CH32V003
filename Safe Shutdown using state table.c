Sure, here is an updated example C program that implements the modified state machine logic based on the updated state table:

```c
// Define the possible states of the state machine
typedef enum {
  RELAY_ON,
  SHUTDOWN_ON,
  POWEROFF,
  BUTTON_TIMER_OFF,
  BUTTON_TIMER_ON
} State;

// Initialize the state machine to the RELAY_ON state
State currentState = RELAY_ON;

// Initialize the button state and last button state
int buttonState = LOW;
int lastButtonState = LOW;

// Initialize the button timer and button timer start time
unsigned long buttonTimer = 0;
unsigned long buttonTimerStart = 0;

// Initialize the relay and shutdown states
int relayState = LOW;
int shutdownState = LOW;

// Loop forever
while (true) {
  // Read the current button state
  buttonState = digitalRead(buttonPin);

  // Check if the button has been pressed
  if (buttonState == HIGH && lastButtonState == LOW) {
    // Start the button timer
    buttonTimerStart = millis();
  }

  // Check if the button is being held down
  if (buttonState == HIGH && lastButtonState == HIGH) {
    // Check if the button timer has expired
    if (millis() - buttonTimerStart > 2000) {
      currentState = POWEROFF;
    }
  }

  // Check if the button has been released
  if (Sure, here is an updated example C program that implements the modified state machine logic based on the updated state table:

```c
// Define the possible states of the state machine
typedef enum {
  RELAY_ON,
  SHUTDOWN_ON,
  POWEROFF,
  BUTTON_TIMER_OFF,
  BUTTON_TIMER_ON
} State;

// Initialize the state machine to the RELAY_ON state
State currentState = RELAY_ON;

// Initialize the button state and last button state
int buttonState = LOW;
int lastButtonState = LOW;

// Initialize the button timer and button timer start time
unsigned long buttonTimer = 0;
unsigned long buttonTimerStart = 0;

// Initialize the relay and shutdown states
int relayState = LOW;
int shutdownState = LOW;

// Loop forever
while (true) {
  // Read the current button state
  buttonState = digitalRead(buttonPin);

  // Check if the button has been pressed
  if (buttonState == HIGH && lastButtonState == LOW) {
    // Start the button timer
    buttonTimerStart = millis();
  }

  // Check if the button is being held down
  if (buttonState == HIGH && lastButtonState == HIGH) {
    // Check if the button timer has expired
    if (millis() - buttonTimerStart > 2000) {
      currentState = POWEROFF;
    }
  }

  // Check if the button has been released
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Stop the button timer
    buttonTimer = millis() - buttonTimerStart;

    // Reset the relay and shutdown states
    relayState = LOW;
    shutdownState = LOW;

    // Switch to the appropriate state based on the current state
    switch (currentState) {
      case RELAY_ON:
        currentState = SHUTDOWN_ON;
        break;
      case SHUTDOWN_ON:
        if (buttonTimer > 0) {
          currentState = BUTTON_TIMER_OFF;
        }
        break;
      case POWEROFF:
        if (buttonTimer == 0 || (buttonTimer > 0 && shutdownState == LOW)) {
          currentState = RELAY_ON;
        }
        break;
      case BUTTON_TIMER_OFF:
        currentState = BUTTON_TIMER_ON;
        break;
      case BUTTON_TIMER_ON:
        if (buttonTimer == 0) {
          currentState = SHUTDOWN_ON;
        } else if (buttonTimer > 2000) {
          shutdownState = HIGH;
          currentState = POWEROFF;
        }
        break;
    }
  }

  // Update the last button state
  lastButtonState = buttonState;

  // Perform the appropriate actions based on the current state
  switch (currentState) {
    case RELAY_ON:
      if (relayState == LOW) {
        // Turn the relay on
        relayState = HIGH;
        digitalWrite(relayPin, relayState);
     }
      break;
    case SHUTDOWN_ON:
      if (shutdownState == LOW) {
        // Turn the shutdown on
        shutdownState = HIGH;
      }
      if (relayState == LOW) {
        // Turn the relay on
        relayState = HIGH;
        digitalWrite(relayPin, relayState);
      }
      break;
    case POWEROFF:
      // Turn the relay and shutdown off
      relayState = LOW;
      shutdownState = LOW;
      digitalWrite(relayPin, relayState);
      break;
    case BUTTON_TIMER_OFF:
      if (relayState == LOW) {
        // Turn the relay on
        relayState = HIGH;
        digitalWrite(relayPin, relayState);
      }
      break;
    case BUTTON_TIMER_ON:
      if (shutdownState == LOW) {
        // Turn the shutdown on
        shutdownState = HIGH;
      }
      break;
  }
}
```

Note that this code assumes that `relayPin` and `buttonPin` have been defined earlier in the program as the input and output pins for the relay and button, respectively. You will need to adjust these values to match your specific hardware setup. Additionally, this code assumes that the `digitalRead()` and `digitalWrite()` functions are available to read the state of the button and control the state of the relay.