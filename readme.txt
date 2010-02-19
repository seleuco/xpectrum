gp2xpectrum v1.7.2 for GP2X ported by Hermes and continued by Metalbrain & Seleuco with patches by kounch & GnoStiC.

INSTALLING
----------

You must decide where to put your spectrum games. By default, they'll be placed in directory roms/spectrum in your SD root, but you can place them in any other directory specified in the .ini file. This file must have the same name as the emulator. If you decide not to use the SD card, you may also put the complete path to the directory, starting from "/mnt" (e.g.: /mnt/nand/spectrum/roms ). You may also specify a path relative to emulator location, starting the path with "./", so if you want to place the games in subdirectory "games" inside the emulator directory, just put "./games" .

Spectrum games must be in this directory or in another directory within it (only one level). If this directory doesn't exists, the emulator will create it and, inside it, the /saves and /img directories will be created, too.


GAME FORMATS
------------

Allows .z80, .sp, .tap, .tzx, .sna and .dsk (+3 disks) Spectrum formats. The files
can be compressed in BZIP2 or ZIP (you'll be able to choose if there are several files
with valid extensions inside the archive).
You can load the save states (.sav) directly, too.


GAME SELECTION
--------------

Use the stick (UP/DOWN) to browse, L & R to move page and A or X to select.
If you press START all files in the directory will be compressed with BZIP2, (except all 
those files already in ZIP format).

Press SELECT to return to GP2X main menu (or Tape Browser).


PLAYING A GAME
--------------

The emulator will works @ 140-185 Mhz speed (depends on frameskip & contention options), with mono or stereo sound
between 11025-44100Hz. Both beeper and AY chip are supported.

Button layout:

Stick, A,B,X,Y,L,R -> programmable with the viryual keyboard. In joystick emulation A button is assigned as joy fire button.

-Vol+ -> adjust sound volume

SELECT -> access to the config menu

START -> show the virtual keyboard

PHOTO function: if enabled, L + SELECT makes a screenshot


VIRTUAL KEYBOARD
----------------

In the virtual keyboard, the buttons are disabled, except for L, R, Y and X.

The X button, press the key currently selected in the virtual keyboard

Pressing START we can exit the virtual keyboard.

Y button will press the currently selected key (as X) and exit the virtual keyboard at the same time (useful for initial menus).

BUTTON PROGRAMMING:

 Push SELECT (the key selector will flash) and press the stick direction or button (A,B,X,Y,L,R) you wish to assign to this key.
 You may abort the programming mode by pushing SELECT again.

SPECIAL FUNCTIONS:

In the virtual keyboard, 5 special functions are included:

RESET: by pressing X, a new submenu appears to select the Spectrum model you want to reset to. By default, the emulator resets to
the 128K model. If a game in .tap or .tzx format, doesn't load in this mode, reset the enulator using the 48K model.
To abort the operation, use Y button.

LOAD: if a game requires the 48K mode, push X in this option to get the LOAD "" spectrum command to load a game in .tap
or .tzx format.

KEYBOARD/JOYSTICKS: use this option to assign the GP2X's stick and buttons to spectrum keys or differents spectrum's joysticks interfaces.

MKEY: this function "pressess" the keyboard sequence previously programmed like a simultaneously key press and then erase
	all the programmed keys. To program the keys, go to the desired key and push A to add this key to the key program. Up to
	up to 8 keys are supported. There is an alternative function. You can use B button instead A button to get Multikey 	programming, but with this method the multikey combination won't be erased (this is useful in e.g.: ABU SIMBEL
	Profanation, where pushing the 'VICTOR' key combination you can get tricks)

TAPE: here we can access directly to the tape browser.


CONFIGURATION MENU
------------------

In this menu, you can access several options:

Load State -> loads the previously save state of a game
Save Keyboard & Speed Profile -> saves the currently programmed keys and selected speeds for the current game. This way
	you won't have to define controls or change speed again each time you load the same game.
Save State -> saves the state of a game

Full Screen [ON/OFF] -> activate/deactivate the full screen mode.
Wait VSYNC [ON/OFF] -> activate/deactivate the vertical retrace syncronization.
Show FPS [ON/OFF] -> show/hide the current frames per second.
Battery Icon [ON/OFF] (only on GP2X F100 model) -> show/hide the battery state icon (top right corner). In full screen mode,
	the icon only appears when the level is low. In that case, the red led will always flash.
POKE Manager-> shows the POKE menu (modification of spectrum memory values)
Photo Mode [Enabled/Disabled] -> allows/disallows the hability to take screenshoots by pressing L+SELECT. The images will be
	saved in /img/ directory.
Sleep Mode-> Suspension mode that turn off the LCD and slows down the CPUs. Press L+SELECT to wake up.
New +3 Disk (only appears while emulating the +3 model) -> Put a new formated +3 disc
Use SIDE X for +3 Disk (only appears while emulating the +3 model) -> Side selector (for double side discs)
Save +3 Disk (only appears while emulating the +3 model) -> Save the current disc in the directory /roms/spectrum/saves with a
	new name (if you write new data in the disc, use this to save).
FS [0/1], Contention [on/off] -> enable/disable frameskip and contended memory emulation. Each of the 4 available modes will
	use a different CPU speed, that can be adjusted in case you want to reduce it to save battery or increase it because
	a certain program needs more power. To change the speed in a mode, select it and use the volume buttons.
Emulation Speed [25 to 175]% -> Allows you to change the orginal Spectrum speed, which can be useful to vary the difficulty of
	games or to observe a detail more slowly. While this option is selected, the volume buttons will vary this speed, not
	the CPU's.

Sound Mute [OFF/High/Low] -> Allows reduction of sound gain, useful for the F200 GP2X model.
Sound [OFF/Mono/Stereo Beeper/Stereo ABC AY/Stereo ALL] -> sets sound mode.
Sound Rate [11025/22050/32000/44100] KHz -> sets the sound frecuency quality.

Fast Loading [ON/OFF] -> enable/disable fast loading mode.
Flash Loading [ON/OFF] -> enable/disable instant load of standard tape blocks.
Edge Loading [ON/OFF] -> enable/disable edge detection mode that may accelerate non-standard tape blocks.
Auto Tape Play/Stop [ON/OFF] -> enable/disable the tape autoplay/stop.
Tape Browser -> Access the tape browser.

Exit from Game -> exit from the game to the game selection screen.
Return to Game -> returns to the game (or press Y button).


Tape Browser
------------

Entering this menu, if a tape is inserted, we'll see the blocks inside the tape, and current block will be marked 
	with an asterisk sign.
Moving the joystick up and down we can select other blocks.
Pressing A/X while the current block is selected we'll manually start or stop the tape.
Pressing A/X while another block is selected, that block will become the current one.
START will allow us to load a differnt tape (or even a snapshot).
With SELECT we'll exit the Tape Browser.


POKE Manager
------------

Entering in this menu, the .pok file of the game will be loaded (if it exists) and shows up to 5 poke options.
You can edit them with A button and use them with X button. On exit, the .pok file will be stored in /saves/ directory.

POKE TITLE EDITOR:

Poke's name editor, it allows up to 29 characters using the virtual keyboard. Pressing ENT in the virtual keyboard, you
	access to value edition.

POKE VALUE EDITOR:

Poke's value editor, allows the edition up to 10 pokes which will be fixed by pushing X button in the Poke Manager.
	Addresses under 16384 are ignored.


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

 This emulator is the result from the work of many people, which we'll mention here:
* Main emulator authors are:
  - Hermes/PS2R: Did the original port for GP2X from GP32's fzx32.
  - Metalbrain: Many improvements, mainly on CPU, timings and screen emulation accuracy.
  - Seleuco: Many improvements, mainly on sound and tape support.
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
  - Philip Kendall, Darren Salt, Witold Filipczyk, Fredrick Meunier and Matan Ziv-Av: creators of FUSE emulator, where our sound code
  	(and also some z80 emulation bits) comes from, and its companion library libspectrum (used to add TZX support).
  - James McKay: Creator of X128 emulator.
  - Ulrich Doewich: Creator of Caprice, an Amstrad CPC emulator, from which the code for the +3 disk unit was taken.
  - Sergey Bulba: Creator of the utility AY2SNA.
  - Julian R Seward: Creator of BZIP.
  - Dieter Baron and Thomas Klausner: Creators of LIBZIP.

* Everybody else we forgot: contact us and you'll be added!


History
-------

V1.0 by Hermes/PS2R (2006/02/05)
- Initial version

V1.1 by Hermes/PS2R (2006/02/06)
- Fixed a bug that caused screen corruptions in Commando
- Fixed volume change problem (now it's more gradual)
- Added a timer to limit the maximum number of frames to spectrum rate.
- Possibility of saving the keyboard/joystick settings that we're using for a game in the configuration menu (it's automatically recovered when loaded).

V1.2 by Metalbrain (2006/09/05)
- Added kounch changes in minimal lib to allow a better TV-Out
- Default key mapping changed:
up	> Q
down	> A
left  	> O
right  	> P
A  	> SPC
B  	> M
X  	> ENTER
Y  	> 0
L  	> CAPS SHIFT
R  	> SYMBOL SHIFT
- L and R can be mapped to any other key, instead of being fixed to CAPS and SYMB
- You may assign a different path for games instead of /roms/spectrum , specifying it on a .ini file that's together with the program and has the same name
- Fixed a bug when loading some .z80 files with an uncompressed block
- L and R advance a page in games list (-25 and +25)
- Increased the maximum number of games from 512 to 10240
- Fixed the timings of repeat opcodes (LDIR,LDDR,CPIR,CPDR,INDR,INIR,OTIR,OTDR)
- Enabled contended memory emulation
- Added an option to display 50 fps (No Frameskip) instead of 25 (Frameskip 1), to better show some effects. When there's no frameskip, CPU speed increases to 175 MHz

V1.3 by Metalbrain (2006/12/02)
- Applied Squidge's hack, thanks to Kounch & Puck2099. This allows a CPU speed reduction to
120MHz (frameskip 1) and 166MHz (no frameskip). In previous versions, increasing to 175MHz
wasn't really enough, so it was slower than it should, that's why the speed reduction has
been lower for No frameskip mode.
- Fixed flags behaviour for block I/O instructions (INI,INIR,IND,INDR,OUTI,OTIR,OUTD,OTDR)
- GnoStiC added USB joystick support.
- Fixed a little speed problem: it was starting always at 166MHz, not taking in
consideration its config file.
- Fixed a bug that caused it to ignore the .ini file sometimes.
- Newlines (and anything else after first line) in the .ini file are now ignored.
- Fixed a bug when loading some .z80 files with version 1.45 .
- Fixed a bug on DAA instruction.
- Fixed flags behaviour for several instructions: BIT, CPI, SBC & SCF.

V1.4 by Metalbrain (2007/06/19)
- Fixed some instruction timings
- Added bus contention for IN and OUT
- Replaced the memory contention emulation using a more precise model. This contended memory emulation
is optional.
- Default CPU speed has gone up to 145/175MHz when contended memory emulation is active, and has been left
to 130/160MHz when it isn't.
- Speed can be changed from the menu using VOL+/-, and can be saved for each individual game together with
their keyboard profile. Allowed speed range is 100-220MHz.
- Fixed a bug in L & R buttons assignation from .key file.
- Now in virtual keyboard Y button will press a key and disable virtual keyboard (useful for game menus).
- Small changes to the configuration menu:
 · Now the default option is Return
 · Load State and Save State have been separated to decrease the chance of mistakes
- Volume is set to 70% at exit, instead of 0% as before, so games that don't set the volume don't become
soundless after executing GP2Xpectrum.

V1.5 by Metalbrain & Seleuco (2008/01/20)
- Fixed a bug that prevented the Y button to be assigned to a key using the virtual keyboard.
- The virtual keyboard won't make all frames show when frameskip is 1 (it resulted in half speed)
- I was still applying contention emulation in a place where I shouldn't when "no contention" was
selected. Now the no contention mode will be less precise, but default speed has been reduced to
125/155MHz.
- Fixed a bug that made port contention not to be applied in most cases.
- Fixed a bug in HALT timing, it was taking 8 states instead of 4.
- Improved floating bus emulation, now passes floatspy test and other similar ones.
- Improved interrupts emulation including how EI affects them.
- Small optimizations.
- Screen rendering code has been totally rewritten from scratch, and now border is completely
emulated (Sentinel letters can be seen), and visual effects of many demos are finally perfect
(Overscan, Shock and similar). Some demos (such as MDA) are still failing though.
- On F200 model, battery level ain't polled, avoiding the massive slowdown on that model. (Thx headoverheels)
- Fixed Sound emulation. Speaker & AY sounds OK now. New DSP Code. More accurated and there are fewer buffer underruns.
- Added stereo sound for the speaker & AY.
- Added sound mode selector: No sound, Mono, Stereo Beeper, Stereo AY, Stereo ALL.
- Added sound rate mode selector: 44100,32000,22050,11025KHz
- Added sound mute selector: Off,Low, High. Low is best for the f200 owners ;)
- Added emulation speed mode: from -75% to + 75 %. Try to play ManicMiner 75% faster and and lives to tell it:) 
- Added FPS selector.
- Added VSYNC selector.
- Bug fix saving configuration.
Note: The stereo sound needs more resources, try to change the cpu clock or change the sound mode if you have sound glitches.

V1.5.1 by Seleuco (2008/01/22)
- Fixed a bug that made default configuration inexistant if the .cfg file couldn't be found, and making it unable
to start on F200 models.

V1.6 - THERE IS NO 1.6!
(Really, that was a prerelease version of the final 1.5, but since the topic name in gp32spain caused some confusion, we've decided to skip it!)

V1.7 by Seleuco & Metalbrain (2008/02/24)
- Full tzx support. New tape code for tap and tzx formats.
- Added Tape sound emulation.
- "fast loading" option to disable speed emulation, tape sound, and contended mode while the game is loaded.
- "flash loading" option to flashload tape blocks when possible.
- "edge loading" option to accelerate game loaders dynamically.
- "Tape Auto Play/Stop" option to play/stop the tape automatically. Best for multilevel games.
- A yellow play label is shown on the screen while the tape is playing; if the label is green, the emulator has detected a loader 
  and is applying the "edge loader" algorithm.
- Auto "full screen off-on" when the tape is playing.
- Added a Tape browser to view/select tape blocks, play/stop the tape manually and to change the tape without resetting the speccy.
- Added the possibility to select the file to load from a zip file.
- Resized the program selection screen to allow longer file names and view the file extensions.
- Added reset for +2A mode.
- Fixed a bug when loading .z80 files saved in +2A/+3 mode (the ROM wasn't properly selected).
- To improve performance, the battery won't get polled on F100 models when battery icon is inactive, and it gets polled less frequently when active.
- Replaced the text "ROM LIST" with "PROGRAM LIST", and "+3 disc" with "+3 disk".
- Updated documentation.
- Compiled with profiling option.
Notes:
- To play a tape like the original speccy. disable "speed loading", "flash loading" and "edge loading".
- If you have a problem loading a tape, try disabling "edge loading", "flash loading" or both.

V1.7.1 by Seleuco & Metalbrain (2008/03/10)
- Fixed a bug that made the LOAD function from virtual keyboard fail most of the times.
- Added a hack to make the loading of the .ini file that indicates the programs path more compatible with gmenu2x.
- Edge loading improved. Now it detects more loaders, DJNZ loops and relative loops.
- rlyeh's minimal lib replaced by Seleuco's micro lib. Smaller, faster and GPL.
- Added some missing copyrights to source files. If you feel yours is missing too, contact us.

V1.7.2 by Metalbrain (2008/08/29)
- No +3 reset when loading a .dsk file if we're already using that model. This makes possible to play multi-disk games.
- Added USB keyboard support. 