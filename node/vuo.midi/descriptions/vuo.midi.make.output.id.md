Finds a MIDI output device that is connected to the computer running the composition.

This node is useful when you know the ID of the MIDI output device that you want. For example, you can use it in place of `Make MIDI Output from Name` if there are multiple devices with the same name and you know the ID of the one you want. If you don't know the ID, then you can instead use `Make MIDI Output from Name` or `List MIDI Devices`.

   - `ID` — The device's ID, which is 0 or greater. 
