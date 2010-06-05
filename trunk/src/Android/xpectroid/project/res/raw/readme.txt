   Xpectroid v1.1 for Android by David Valdeita (Seleuco)

Based on iXpectrum from Seleuco and gp2xpectrum by Hermes/PS2Reality, Metalbrain & Seleuco.

1.1 what's new
--------------

- Improved overall performance. 50 real fps in N1.

- New auto frame skip mode for slower devices. 12 levels.

- Improved multitouch responsiveness.

- Improved Android 1.5 cupcake support.

- New landscape touch controller suited for no multitouch or slower devices (no alpha blending is needed). The older one 
  can be selected at the menu if you prefer. In the new controller, you can tap in center of the emulator view to show the touch keyboard.
  Also, multitouch devices can fire touching on the right side of the emulator view.
  
- New option for redefine hardware keys.

- Trackball support.
  
- Improved touch keyboard. New auto Caps Shift and Symbol Shift keys. Android delete key mapped to Caps Shift + 0. 


Xpectroid is a ZX Spectrum emulator for Android with the following features:

- ZX Spectrum 48K, 128K, Plus2, Plus2A, Plus3 emulation.

- Support from Android 1.5 cupcake or any later version.

- Portrait-Landscape auto rotation.

- Accurate audio, mono or stereo beeper ABC. Stereo AY support. Sound rate from 11025 KHz to 44100Khz.

- Image smoothing. Original or full screen with aspect ratio options. Speccy border cropping option.

- Multitouch control.
 
- DPAD Control.

- Phone physical keyboard support.

- Touchable Keyboard.

- Virtual keyboard and buttons for not touchable devices.

- Multiple resolutions support.

- Save states.

- Tape and TZX turbo support. Tape loading speed options. Tape Browser.

- Kemspton Joystick, interface II,I, cursor, fuller, keyboard input emulation.

- Frame skip options.

- UlaPlus support.

- Emulation speed selection option.

- Poke Manager.


The emulator includes the following great games with the permission of Jonathan Cauldwell:

-Egghead Round the Med
-Banger Management
-Albatrossity (very funny)
-Christmas Cracker
-Battery's not Precluded (Ula+64 compatible)
-Eggheadplus (ula+64 demo)

there is also included the following games from the spanish's scene created by mojontwins:

-Lala Prologue
-Moggy-Adventure
-Subaquatic (ula+64 compatible)
-Subaquatic-Reloaded (ula+64 compatible)
-Uwol (Ula+64 compatible)
"Lala Prologue" is just great. :)


INSTALLING
----------

Place your .z80, .sp, .tap, .tzx, .sna and .dsk files in the sdcard directory "/ROMs/Xpectroid/"  


ANDROID
-------

Press menu button to show virtual buttons, Android settings and quit options.

You can tap in the speccy screen (in portrait mode) to change from the controller to the touch keyboard. Tape it again to go back to the controller.

Also, you can tap in the top of the screen to show the transparent keyboard while in landscape mode.

Trackball button is mapped to B.
Back button is mapped to X
Search Button is mapped to A.


GAME FORMATS
------------

Allows .z80, .sp, .tap, .tzx, .sna and .dsk (+3 disks) Spectrum formats. The files
can be compressed in BZIP2 (you'll be able to choose if there are several files
with valid extensions inside the archive).
You can load the save states (.sav) directly, too.


GAME SELECTION
--------------

Use the stick (or DPAD) (UP/DOWN) to browse, stick (LEFT/RIGHT) to move page, L & R to go to first or last entry and B to select (or trackball button). 

X exits resetting the speccy to 128K mode. (you can reset to another model in the virtual keyboard, see later)

Press SELECT ("m2" or little right red button in portrait mode) to return to the main (inner) menu (or Tape Browser).


PLAYING A GAME
--------------

Button layout:

Stick, A,B,X,Y,L,R -> programmable with the virtual keyboard (not from the touch keyboard). In joystick emulation B button is assigned as joy fire button.

START ("m1" or left little left button)-> show the virtual keyboard

SELECT ("m2" or right little red button) -> access to the config menu


VIRTUAL KEYBOARD
----------------

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


CONFIGURATION MENU
------------------

In this menu, you can access several options:

Load State -> loads the previously save state of a game 

Save Keyboard -> saves the currently programmed keys for the current game. This way you won't have to define controls again each time you load the same game.

Save State -> saves the state of a game

Show FPS [on/off] -> show/hide the current frames per second.

POKE Manager-> shows the POKE menu (modification of spectrum memory values)

"Ula+64 disabled", "Ula+64 with colour reset" and "Ula+64 without color reset" disable ULAPlus emulated HW or enable it keeping the palette colours between resets or not.

New +3 Disk (only appears while emulating the +3 model) -> Put a new formated +3 disc Use SIDE A for +3 Disk (only appears while emulating the +3 model) -> Side selector (for double side discs)

Save +3 Disk (only appears while emulating the +3 model) -> Save the current disc in the directory /roms/spectrum/saves with a new name (if you write new data in the disc, use this to save).

Draw 1/2, 1/2 All Frames -> ZX Spectrum works in PAL mode so 25fps (1/2) should be enough in most cases, but if you want to see all ULA effects enable "50 fps" option at the expense of losing performance. You can enable (1/3) to gain performance.

Contention [on/off] -> Enable/disable contended memory emulation.

Emulation Speed [25 to 175]% -> Allows you to change the orginal Spectrum speed, which can be useful to vary the difficulty of games or to observe a detail more slowly. While this option is selected, the stick buttons (LEFT & RIGHT) will vary this speed.

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

Tape Browser
------------

Entering this menu, if a tape is inserted, we'll see the blocks inside the tape, and current block will be marked with an asterisk sign. Moving the joystick up and down we can select other blocks. Pressing B while the current block is selected we'll manually start or stop the tape. Pressing B while another block is selected, that block will become the current one.

"A" will allow us to load a differnt tape (or even a snapshot). With "X" we'll exit the Tape Browser.

POKE Manager
------------

Entering in this menu, the .pok file of the game will be loaded (if it exists) and shows up to 5 poke options.
You can edit them with A button and use them with B button. On exit, the .pok file will be stored in /saves/ directory.

POKE TITLE EDITOR:

Poke's name editor, it allows up to 29 characters using the virtual keyboard. Pressing ENT in the virtual keyboard, you access to value edition.

POKE VALUE EDITOR:

Poke's value editor, allows the edition up to 10 pokes which will be fixed by pushing B button in the Poke Manager. Addresses under 16384 are ignored.


Game formats and use:
---------------------

.TAP, .TZX -> Use the 128k loader, and if it fails, try reseting in 48K mode (with RESET in virtual keyboard) and use LOAD "" 	(pressing LOAD in the virtual keyboard)
.Z80, .SNA -> Direct load
.DSK -> Use the +3 loader


Thanks and greetings
--------------------

First of all, to Rlyeh for his work in GP32 as in GP2X, for his tools and the base source code for this emulator.

Of course, my sincerelly greeting to all the people that in a form or another, were contributed with his work to
make real this emulator. Greetings to all the people of Gp32spain's forums their support and caress to me.
I hope that you like this emulator. Enjoy ;)

To many thank to OscarBraindeaD by the translation.

Credits
-------

--> Xpectroid

* Seleuco: Main port emulator author.

--> iXpectrum:

* Seleuco: Main port emulator author.
* ZodTTD: The emulator is based in his code and findings.
* J.Valdeita.L:  Portrait skin art.

--> gp2Xpectrum:

 This emulator is the result from the work of many people, which we'll mention here:
* Main emulator authors are:
  - Hermes/PS2R: Did the original port for GP2X from GP32's fzx32.
  - Metalbrain: Many improvements, mainly on CPU, timings and screen emulation accuracy.
  - Seleuco: Many improvements, mainly on sound and tape support.
  - SplinterGU: SDL code & improved the load screen.
* The following people contributed with patches:
  - kounch: TV-out scale patch.
  - GnoStiC: USB joystick patch.
* Generic GP2X code is also used, from:
  - rlyeh: his minimal library was used in old versions, and the current micro lib has been created partly thanks to his findings.
  - Squidge: MMU hack
  - Puck2099: USB joystick library.
  - notaz: code to flush the uppermem cache.
* And last but not least, GP2Xpectrum has used code from many other open source projects, whose authors we should also name:
  - rlyeh (again): creator of fzx32 for the GP32 console.
  - Santiago Romero: creator of ASpectrum emulator, which was the base for fzx32.
  - Philip Kendall, Darren Salt, Witold Filipczyk, Fredrick Meunier and Matan Ziv-Av: creators of FUSE emulator, where our sound code (and also some z80 emulation bits) comes from, and its companion library libspectrum (used to add TZX support).
  - James McKay: Creator of X128 emulator.
  - Ulrich Doewich: Creator of Caprice, an Amstrad CPC emulator, from which the code for the +3 disk unit was taken.
  - Sergey Bulba: Creator of the utility AY2SNA.
  - Julian R Seward: Creator of BZIP.
  - Dieter Baron and Thomas Klausner: Creators of LIBZIP.

* Everybody else we forgot: contact us and you'll be added!
