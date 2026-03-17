# UNO-Q Router Bridge – Data Corruption Demonstration

## How to reproduce the demonstration

This repository demonstrates the difference between:

- `Bridge.provide()` (unsafe)
- `Bridge.provide_safe()` (thread-safe)

### Step 1 — Run the unsafe version

In `sketch.ino`, use:

Bridge.provide("update", updateCounter);

Upload and run the program.

After some time you should observe messages like:

> ERROR inconsistent state: counter=21 double=40  
> ERROR inconsistent state: counter=RPC update: counter=1919 double= double=3836  
> ERROR inconsistent state: counter=27 double=RPC update: counter=5227 double=54  

This shows a race condition: `loop()` has read the variables while the RPC callback was updating them.

### Step 2 — Run the safe version

Now replace the line with:

Bridge.provide_safe("update", updateCounter);

Upload and run the program again.

The inconsistent state disappears.

---

## Overview

This example demonstrates a **data corruption scenario** caused by concurrent access to a shared output resource (`Monitor`) on the Arduino UNO Q.

It compares the behavior of:

* `Bridge.provide()` (unsafe)
* `Bridge.provide_safe()` (safe)

---

## Objective

To reproduce the situation described below:

* Thread A prints: `Forum`
* Callback prints: `12.37`

### Expected behavior

* With `provide()` → output may be corrupted:

```

12.37
For12.37
um
12.37
F12.37
orum
12.37
12.37
Forum
12.37
12.37
Foru12.37
```

* With `provide_safe()` → output is clean:

```
Forum
12.37
```

---

## Architecture

* **MPU (Python)**: periodically calls a remote procedure via `Bridge.call()`
* **MCU (C++)**:

  * main loop prints `"Forum"` slowly
  * callback prints the value received from MPU

Both write to the same output: `Monitor`

---

## Code

### MPU (Python)

```python
from arduino.app_utils import App, Bridge
import time

def loop():
    Bridge.call("print_value", 12.37)
    time.sleep(0.05)

App.run(user_loop=loop)
```

---

### MCU (C++)

```cpp
#include <Arduino.h>
#include <Arduino_RouterBridge.h>

void printSlow(const char *s) {
  for (int i = 0; s[i] != '\0'; i++) {
    Monitor.print(s[i]);
    delay(10);
  }
  Monitor.println();
}

void printFromMpu(float value) {
  Monitor.println(value, 2);
}

void setup() {
  Bridge.begin();
  Monitor.begin();

  Bridge.provide("print_value", printFromMpu);
  //Bridge.provide_safe("print_value", printFromMpu);

  delay(5000);
}

void loop() {
  printSlow("Forum");
  delay(100);
}
```

---

## Important Note

The delay in `loop()` is required.

Without it, the main loop runs too fast, and when using `provide_safe()`,
`Forum` may be printed multiple times before the deferred callback executes.

Adding a delay ensures a clearer and more consistent demonstration.

---

## Key Concept

* `provide()` allows the callback to execute in a way that can interfere with ongoing operations → **possible output corruption**

* `provide_safe()` defers execution to a safe point → **no corruption**

---

## Conclusion

This example clearly shows that:

* Concurrent access to a shared resource can lead to corrupted output
* `provide_safe()` prevents this by ensuring safe execution timing
