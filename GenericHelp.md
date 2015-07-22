**GAME FORMATS**

Allows .z80, .sp, .tap, .tzx, .sna and .dsk (+3 disks) Spectrum formats. The files
can be compressed in BZIP2 (you'll be able to choose if there are several files
with valid extensions inside the archive).
You can load the save states (.sav) directly, too.


**GAME SELECTION**

Use the stick (UP/DOWN) to browse, stick (LEFT/RIGHT) to move page, L & R to go to first or last entry and B to select.

X exits resetting the speccy to 128K mode. (you can reset to another model in the virtual keyboard, see later)

Press SELECT to return to the main menu (or Tape Browser).


**PLAYING A GAME**

The emulator will works full speed in iPhone 3G in landscape or portrait mode (depends on frames emulated and contention options), with mono or stereo sound between 11025-44100Hz. Both beeper and AY chip are supported.

Button layout:

Stick, A,B,X,Y,L,R -> programmable with the virtual keyboard (not from the touch keyboard). In joystick emulation B button is assigned as joy fire button.

SELECT (called "m2" in iPhone) -> access to the config menu

START (called "m1" in iPhone)-> show the virtual keyboard


**VIRTUAL KEYBOARD**

In the virtual keyboard, the buttons are disabled, except for L, R and B.

The B button, press the key currently selected in the virtual keyboard

Pressing START we can exit the virtual keyboard.

BUTTON PROGRAMMING:

Push SELECT (the key selector will flash) and press the stick direction or button (A,B,X,Y,L,R) you wish to assign to this key. You may abort the programming mode by pushing SELECT again.

SPECIAL FUNCTIONS:

In the virtual keyboard, 5 special functions are included:

RESET: by pressing B, a new submenu appears to select the Spectrum model you want to reset to. By default, the emulator resets to the 128K model. If a game in .tap or .tzx format, doesn't load in this mode, reset the enulator using the 48K model.To abort the operation, use X button.

LOAD: if a game requires the 48K mode, push B in this option to get the LOAD "" spectrum command to load a game in .tap or .tzx format.

KEYBOARD/JOYSTICKS: use this option to assign the stick and buttons to spectrum keys or differents spectrum's joysticks interfaces.

MKEY: this function "pressess" the keyboard sequence previously programmed like a simultaneously key press and then erase all the programmed keys. To program the keys, go to the desired key and push A to add this key to the key program. Up to up to 8 keys are supported. There is an alternative function. You can use B button instead A button to get Multikey programming, but with this method the multikey combination won't be erased (this is useful in e.g.: ABU SIMBEL Profanation, where pushing the 'VICTOR' key combination you can get tricks)

TAPE: here we can access directly to the tape browser.


**CONFIGURATION MENU**


In this menu, you can access several options:

Load State -> loads the previously save state of a game

Save Keyboard -> saves the currently programmed keys for the current game. This way you won't have to define controls again each time you load the same game.

Save State -> saves the state of a game

Spectrum Screen or Full Screen -> activate/deactivate the full screen mode.

Show FPS [on/off] -> show/hide the current frames per second.

POKE Manager-> shows the POKE menu (modification of spectrum memory values)

"Ula+64 disabled", "Ula+64 with colour reset" and "Ula+64 without color reset" disable ULAPlus emulated HW or enable it keeping the palette colours between resets or not.

New +3 Disk (only appears while emulating the +3 model) -> Put a new formated +3 disc Use SIDE A for +3 Disk (only appears while emulating the +3 model) -> Side selector (for double side discs)

Save +3 Disk (only appears while emulating the +3 model) -> Save the current disc in the directory /roms/spectrum/saves with a new name (if you write new data in the disc, use this to save).

Draw all (50fps) , 1/2 (25fps) or autoskip frames. -> ZX Spectrum works in PAL mode so 25fps should be enough in most cases, but if you want to see all ULA effects enable "50 fps" option at the expense of losing performance.

Contention [on/off] -> Enable/disable contended memory emulation.

Emulation Speed [to 175](25.md)% -> Allows you to change the orginal Spectrum speed, which can be useful to vary the difficulty of games or to observe a detail more slowly. While this option is selected, the stick buttons (LEFT & RIGHT) will vary this speed.

Sound [OFF/Mono/Stereo Beeper/Stereo ABC AY/Stereo ALL] -> sets sound mode.

Sound Rate [11025/22050/32000/44100] KHz -> sets the sound frecuency quality.

Fast Loading [ON/OFF] -> enable/disable fast loading mode.

Flash Loading [ON/OFF] -> enable/disable instant load of standard tape blocks.

Edge Loading [ON/OFF] -> enable/disable edge detection mode that may accelerate non-standard tape blocks.

Auto Tape Play/Stop [ON/OFF] -> enable/disable the tape autoplay/stop.

Tape Browser -> access the tape browser.

Exit from Game -> exit from the game to the game selection screen.

Return to Game -> returns to the game.

Note: If you want to load a game in high compability mode, or just as original spectrum did, disable Fast & Flash & Edge loading. If you still have problems loading the game, remember to reset to 48k mode.

**Tape Browser**


Entering this menu, if a tape is inserted, we'll see the blocks inside the tape, and current block will be marked with an asterisk sign. Moving the joystick up and down we can select other blocks. Pressing B while the current block is selected we'll manually start or stop the tape. Pressing B while another block is selected, that block will become the current one.

"A" will allow us to load a differnt tape (or even a snapshot). With "X" we'll exit the Tape Browser.

**POKE Manager**


Entering in this menu, the .pok file of the game will be loaded (if it exists) and shows up to 5 poke options.
You can edit them with A button and use them with B button. On exit, the .pok file will be stored in /saves/ directory.

POKE TITLE EDITOR:

Poke's name editor, it allows up to 29 characters using the virtual keyboard. Pressing ENT in the virtual keyboard, you access to value edition.

POKE VALUE EDITOR:

Poke's value editor, allows the edition up to 10 pokes which will be fixed by pushing B button in the Poke Manager. Addresses under 16384 are ignored.


**Game formats and use**


.TAP, .TZX -> Use the 128k loader, and if it fails, try reseting in 48K mode (with RESET in virtual keyboard) and use LOAD "" 	(pressing LOAD in the virtual keyboard)
.Z80, .SNA -> Direct load
.DSK -> Use the +3 loader