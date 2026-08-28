This is a 4x3 USB keypad using the STM32F042F6.

Please check the photos in the "img" folder.


To switch modes, press the Mode/DFU button.

The following is the key map in Mode 1 (the leftmost LED lights up).
    Ins    Home   PgUp
    Del    End    PgDn
    Lctrl  ↑      Lshift
    ←      ↓      →

The following is the key map in Mode 2 (the second LED from the left lights up).
    7&     8*     9(
    4$     5%     6^
    1!     2@     3#
    0)     Bspace Enter

The following is the key map in Mode 3 (the 3rd LED from the left lights up).
    F10    F11    F12
    F7     F8     F9
    F4     F5     F6
    F1     F2     F3


The rightmost LED toggles whenever it receives USB SOF. (500Hz)
