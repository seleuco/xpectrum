**INSTALLING**

You can download the debian package and install via SSH using the following command:

"dpkg -i iXpectrum.deb"

or more easily: install from Cydia.

Place your .z80, .sp, .tap, .tzx, .sna and .dsk in /var/mobile/Media/ROMs/iXpectrum/

To copy files to the iphone you can install SSH from Cydia and use a SFTP or SCP client.

To use SSH after jailbreak, you can go up to Cydia and search for OpenSSH to find the package. Tap the Install button to install it on your iPhone. Once installed, restart your iPhone. You wont find an icon for SSH on the home screen. But after restart, the SSH tool will be automatically launched and listens for remote request in background.

scp is a nice and powerful file copier for osx and linux, the best command we need to copy files between our pc and our iphone.  Just go to the terminal and type:

scp /source/**-F root@ip:/var/mobile/Media/ROMs/iXpectrum/  (where ip is the WIFI iPhone IP)**

this copy is done via ssh to the ip of the phone.

You can also use a rich GUI client like Filezilla to do a SFTP copy or maybe WinSCP if you prefer.

**iPHONE & iPad**

Tap over the options label (in portrait mode) to change the following options:

-Landscape Keeps Aspect. 'Enabled' keeps the aspect ratio in landscape mode; 'Disabled' will use all available screen.

-Portrait original size (iPad only). Enable to draw spectrum screen at its original size.

-Smoothed Landscape or Smoothed portrait. Enable to apply a image filter over the emulator screen.

-Safe Render Path (iPhone only). Enable at the expenses of performance for maximun compability. Slow in 3G.

You can tap in the speccy screen (in portrait mode) to change from the controller to the touch keyboard. Tape it again to go back to the controller.

Also, you can tap in the top of the screen to show the transparent keyboard while in landscape mode. The landscape keyboard can be hidden touching in the "hide" label. When hidden, you can touch in any position of the screen to show it again. Touch in the "Back to Controller" label to go back to the controller.

You can load a ula+ palette and insert a new tape to load a game with this palette applied, or you can set the option "ula+64 without reset" (in the configuration menu) to load a snapshot or tape from the standard menu. This doesn't reset the colours loaded previously, even if you reset the speccy (it makes testing games with palette colours easier).

Also, at the options screen you can press donate button to open a browser and donate some cash though Paypal. It's simple and reliable; just click the button and enter your credit card information (or PayPal account info if you have a PayPal account). Help me to support the emulator is you liked it, so i can develop more emulators and improve this one.