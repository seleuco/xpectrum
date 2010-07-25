iXpectrum v1.3 for Jailbroken iPhone, iPad & iPod Touch by David Valdeita (Seleuco)

Based in gp2xpectrum by Hermes/PS2Reality and continued by Metalbrain & Seleuco.

iXpectrum is a ZX Spectrum emulator for Jailbroken iPhone, iPad & iPod Touch with the following features:

- Full speed on iPhone 3G in landscape or portrait mode.
- Portrait-Landscape auto rotation.
- Accurate audio. 
- Image smoothing & full screen with aspect ratio options.
- Touch ZX Spectrum keyboard.

If you wish to colaborate, get the source code or port to another platform, take a look at the xpectrum site: 

http://code.google.com/p/xpectrum/

Also consider make a donation if you liked the emulator, so i can improve it.

New in 1.3 (World Champion Edition)

This new version is to thank you all for your suggestions and donations. Also to celebrate the recent Spanish football world cup win :)

- Added "World Of Spectrum" Integration. Find & Download WoS game browser. (see http://www.worldofspectrum.org/)

- Added ZIP file support.

- Added scanlines & TV like filters.

- Speccy border crop options.

- Fixed missing line on 3GS. You may enable 'safe render path' on 3GS.

- Fixed lots of Objective-C memory leaks.

New in 1.2

- Added iPad HD support.

- iOS4 compability.

- New safe render path (private APIs not used); selectable on iPhone, forced on iPad. Select it for maximun future compability. Slow in 3G.
  
- Improved Z80 emulation, thanks to Metalbrain. Caution, older saved game states may not be compatible.

- Portrait, Landscape smoothing options, now indepently selectable.

- Save game state, now keeps the old save state and the new save state, meaning you have two save state slots.

- Added Xpectroid improvements (autoframeskip and other minor changes)

- New option to donate easily. Help me to support the emulator, so i can add new features and develop another emulators.

Thanks to Ryosaebaa for his iPad support and testing.

(some people have had problems saving game sates... check you have write permisions. chmod 777 app and ROMs directory if it is necesary)

New in 1.1:

- Added a real :) ZX Spectrum touch keyboard in portrait mode and a transparent touch keyboard in landscape mode so you can play adventure text games and make some BASIC programs if you like :P.

- Added emulation for ULAplus.
 (ULAplus is a plug-in replacement ULA for the ZX Spectrum which increases the total 
 palette to 256 colours). 

Read more at: http://sites.google.com/site/ulaplus/

Thanks to Chris Smith & Andrew Owen and others involved.

- Added thumbnails in save states.

- Reallocated some keys. Some minor bug fixes.

Also includes all features from gp2Xpectrum :).

The emulator includes the following great games with the permission of Jonathan Cauldwell:

-Egghead Round the Med

-Banger Management

-Albatrossity (very funny)

-Christmas Cracker

-Kuiper Pursuit

-Battery's not Precluded (Ula+64 compatible)

-Eggheadplus (ula+64 demo)

there is also included the following games from the spanish's scene created by mojontwins:

-Lala Prologue

-Moggy-Adventure

-Cheril of the Bosque

-Cheril Perils (ula+64 compatible)

-Moggy Adventure

-Sgt. Helmet Zero (ula+64 compatible)

-Subaquatic (ula+64 compatible)

-Subaquatic-Reloaded (ula+64 compatible)

-Uwol (Ula+64 compatible)

"Lala Prologue" is just great. :)

The Emulator also includes some slideshows showing the Ula+64 working (take a look at the c64 slideshow thanks to Tom Cat. It looks great), and some ula+64 palettes that you can load before a game to change its colours.

also is included Ghost Castle game thanks to CodenameV

INSTALLING
----------

Place your .z80, .sp, .tap, .tzx, .sna and .dsk in /var/mobile/Media/ROMs/iXpectrum/ 

To copy files to the iphone you can install SSH from Cydia and use a SFTP or SCP client.

To use SSH after jailbreak, you can go up to Cydia and search for OpenSSH to find the package. Tap the Install button to install it on your iPhone. Once installed, restart your iPhone. You wont find an icon for SSH on the home screen. But after restart, the SSH tool will be automatically launched and listens for remote request in background.

scp is a nice and powerful file copier for osx and linux, the best command we need to copy files between our pc and our iphone.  Just go to the terminal and type:

scp /source/* -F root@ip:/var/mobile/Media/ROMs/iXpectrum/  (where ip is the WIFI iPhone IP)

this copy is done via ssh to the ip of the phone.

You can also use a rich GUI client like Filezilla to do a SFTP copy or maybe WinSCP if you prefer.

iPHONE & iPad
-------------

Download games:

Tap over the options label and press the download option. A browser is open. Search for the game and click the corresponding download hyperlink. The game is downloaded on the downloads directory. Press 'A' to refesh the downloads directory if the game is not showed. You can also delete the game pressing 'Y'.

Tap over the options label (in portrait mode) and press options button to change the following settings:

-Landscape & Portrait Crop Speccy Border. 'Enable" to crops spectrum boder so you have more visible area.

-Smoothed Landscape or Smoothed portrait. Enable to apply a smoothing image filter over the emulator screen.

-TV Filter. Enable to apply a TV like filter over the image.

-Sacanline Filter. Enable to apply a scanline filter over the image.

-Landscape Keeps Aspect (iPhone only). 'Enabled' keeps the aspect ratio in landscape mode; 'Disabled' will use all available screen.

-Original size (iPad only). Enable to draw spectrum screen at its original size on portrait mode.

-Safe Render Path (iPhone only). Enable at the expenses of performance for maximun compability. Slow in 3G (not recomended unless you find any problem).

You can tap in the speccy screen (in portrait mode) to change from the controller to the touch keyboard. Tape it again to go back to the controller.

Also, you can tap in the top of the screen to show the transparent keyboard while in landscape mode. The landscape keyboard can be hidden touching in the "hide" label. When hidden, you can touch in any position of the screen to show it again. Touch in the "Back to Controller" label to go back to the controller.

You can load a ula+ palette and insert a new tape to load a game with this palette applied, or you can set the option "ula+64 without reset" (in the configuration menu) to load a snapshot or tape from the standard menu. This doesn't reset the colours loaded previously, even if you reset the speccy (it makes testing games with palette colours easier).

Also, at the options screen you can press donate button to open a browser and donate some cash though Paypal. It's simple and reliable; just click the button and enter your credit card information (or PayPal account info if you have a PayPal account). Help me to support the emulator if you liked it, so i can develop more emulators and improve this one.

GAME FORMATS
------------

Allows .z80, .sp, .tap, .tzx, .sna and .dsk (+3 disks) Spectrum formats. The files
can be compressed in BZIP2 (you'll be able to choose if there are several files
with valid extensions inside the archive).
You can load the save states (.sav) directly, too.


GAME SELECTION
--------------

Use the stick (UP/DOWN) to browse, stick (LEFT/RIGHT) to move page, L & R to go to first or last entry and B to select. 

X exits resetting the speccy to 128K mode. (you can reset to another model in the virtual keyboard, see later)

Press SELECT to return to the main menu (or Tape Browser).


PLAYING A GAME
--------------

The emulator will works full speed in iPhone 3G in landscape or portrait mode (depends on frames emulated and contention options), with mono or stereo sound between 11025-44100Hz. Both beeper and AY chip are supported.

Button layout:

Stick, A,B,X,Y,L,R -> programmable with the virtual keyboard (not from the touch keyboard). In joystick emulation B button is assigned as joy fire button.

SELECT (called "m2" in iPhone) -> access to the config menu

START (called "m1" in iPhone)-> show the virtual keyboard


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

Draw all (50fps) , 1/2 (25fps) or autoskip frames. -> ZX Spectrum works in PAL mode so 25fps should be enough in most cases, but if you want to see all ULA effects enable "50 fps" option at the expense of losing performance.

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

--> iXpectrum:

* Seleuco: Main port emulator author.
* ZodTTD: The emulator is based in his code and findings.
* J.Valdeita.L:  Portrait skin art.
* Ryosaebaa. iPad art, design, testing.

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


History
-------

iXpectrum

v.1.3 by Seleuco (2010/25/07)

- Added "World Of Spectrum" Integration. Find & Download WoS game browser. (see http://www.worldofspectrum.org/)
- Added ZIP file support.
- Added scanlines & TV like filters.
- Speccy Border crop options.
- Fixed missing line on 3GS.
- Fixed lots of Objective-C memory leaks.

v.1.2 by Seleuco (2010/07/07)

- iPad HD support
- iOS 4 compability.
- New safe render path.
- Z80 and overall emulation improved by Metalbrain:
  Fixed flags and contention in IND,INDR,INI,INIR instructions.
  Fixed flags in OUTI,OTIR,OUTD,OTDR instructions.
  Improved port contention emulation.
  Only read AY registers from port 0xbffd if we're in +2A/+3 mode.
  In 128k models (not +2A/+3), when reading from port 0x7ffd the value read is written back to port 0x7ffd.
- Portrait, Landscape separated smoothing options.
- Save game state, now keeps the old save state.
- New option to donate easily.

V.1.1 by Seleuco (2010/02/23)

- Added a full ZX Spectrum Portrait touch keyboard with key zooming.
- Added a landscape transparent touch keyboard that can be hidden.
- Added emulation for ULAplus 64 color mode.
- Added support for ULaPlus in save states (retaining the compability with previous savestates)
- Added thumbnails for save states
- Changed some key mappings for better coherency.
- L & R changed to go to the  first or last ROM in the ROMs screen.
- Changed frame skip option name to 25fps or 50fps option name (is more clear)
- Some minor bug fixes. 

v.1.0 by Seleuco (2010/02/05)

- iPhone & iPod Touch port based in ZodTTD MAME port.
- added IO Remote Audio Unit for improved performance and low latency audio.
- Landscape & Portrait mode autorotate.
- Added option to enable\disable image smoothing. 
- Added option to enable\disable keep aspect ratio in landscape mode.


GP2Xpectrum

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

v1.8 by SplinterGU (2009)
- Wiz & Linux port based on SDL.
- Removed HW dependencies with GP2x (The best GP2X version is still 1.7.2)
- Added game images in file manager.

v.1.8.1 by Seleuco (2010/05/02)
- Corrected a memory bug in file manager.
- Added auto fullscreen off while is loading a tape.
- Set default keys when load a new rom. Fixed some key mappings.
- Some SDL refactoring to make it more portable.


v.1.9 by Seleuco (2010/23/02)
- Added emulation for ULAplus 64 color mode.
- Added support for ULaPlus in save states (retaining the compability with previous savestates)
- Added thumbnails for save states
- Changed some keys mapping for better coherency.
- L & R changed to go to the  first or last ROM in the ROMs screen.
- Changed frame skip option name to 25fps or 50fps option name. (is more clear)
- Some minor bug fixes. 

v.1.9.1 by Seleuco (2010/04)
- Added autoframeskip.
- Minor changes.

v.1.9.2 by Seleuco (2010/07)
- Added ZLIB support

