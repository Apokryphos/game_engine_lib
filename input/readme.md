# input_lib

Input devices can be queried directly to see if an input (button, key, axis) was
pressed or released, but events should be used to ensure no user input is lost.


## Actions

Actions are created for each user input to query. The ActionSet used by the game
engine is stored in the InputManager class instance.

```cpp
    InputActionSet& action_set = input_mgr.get_action_set();
```

Actions are defined as an ID, an **input type** (analog or digital), and an
**event type** (when to produce an event). The table below describes when
an input event for a given input action will be fired after.

| Event Type | Description            |
| ---------- | ---------------------- |
| Down       | Input is in down state |
| Pressed    | Input was pressed      |
| Released   | Input was released     |
| Up         | Input is in up state   |
| Delta      | Input state changed    |
| Poll       | Every frame            |


## Events

Input events are fired in response to user input based on input action
bindings.

Events contain the origin device ID, the input action ID, the event type,
and the value of the input. Analog type actions will have a value of
[-1.0, 1.0] for the sticks and [0.0, 1.0] for the triggers. Digital type actions
will have an on or off value (zero and +1/-1).
