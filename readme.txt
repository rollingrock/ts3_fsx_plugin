Teamspeak 3 Plugin for FSX Comms!
Author : Jason Pharis - rollingrock16@gmail.com
www.virtualcarrierwing3.com

Installation:

Sorry no fancy installer yet!  Please follow these directions:

1) First understand if you have the 32 bit or 64 bit version of teamspeak installed.  Once you know this copy the appropriate ts3_fsx_radio_plugin_xxbit.dll into your teamspeak plugins folder (typically found in C:\Program Files\TeamSpeak 3 Client\plugins).
1a) Launch Teamspeak3 and go to Settings/Plugins and enable by clicking the checkbox in the dialog window.

2) Place simconnect_getradio.exe in your FSX root folder.

3) Find your EXE.xml file.  Usually found somewhere like : C:\Users\Jason\Application Data\Microsoft\FSX
3a) Open up the EXE.xml file and add the following:
	<Launch.Addon>
		<Name>TS3_radio</Name>
		<Disabled>False</Disabled>
		<Path>(PATH TO FSX ROOT)\simconnect_getradio.exe</Path>
		<CommandLine>auto</CommandLine>
	</Launch.Addon>

NOTE that (PATH TO FSX ROOT) should be replaced with the actual path.  For instance on my setup this line looks like:
		<Path>D:\fsx\simconnect_getradio.exe</Path>

4) You need to setup 4 hotkeys in Teamspeak 3.  Go into the Teamspeak Plugins menu and click the settings button with the plugin highlighted. Follow the directions in the dialog window. 

5) Launch FSX with the plugin running and you're all set!

Usage:
Once in the sim just set your radios to desired frequency and use the hotkeys you setup in Teamspeak 3 to push-to-talk on the respective frequency.  Anyone else on the teamspeak server that is running the plugin and on your frequency should hear you!




Version History
---------------------------------------
V1.0 11/18/12
- Intial release.
v1.1-1.4
- Internal testing.
- Added PTT hotkey support handled with teamspeak
- Added guard channels
- Added down sampling of audio output to provide slight distortion and provide difference to normal teamspeak transmission
v1.5
- initial public release.
- bug fixes with whisper list handling.
v1.6
- updated to boost v1.55
- updated to Qt 5.2.1
- Added settings dialog window
- Added method to set hotkeys in settings window.  will also detect conflicts and mismatches and flag the user
- working on enabling custom guard channels.  not fully implemented yet.