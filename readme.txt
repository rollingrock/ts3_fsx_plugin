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

4) You need to setup 4 hotkeys in Teamspeak 3.  Go to Settings/Options/Hotkeys and click Add towards the bottom of the window
4a) This should bring up the hotkey Setup Diaglog.  Click Show advanced Actions
4b) Expand "Plugins" ; then Expand "Plugin Hotkey" ; Finally expand "ts3_fsx_radio_plugin"
4c) Setup hotkeys for the 4 listed.  Not it's really just 2 buttons but you have to setup "On key Down" and "on Key release" for each button.

5) Launch FSX with the plugin running and you're all set!

Usage:
Once in the sim just set your radios to desired frequency and use the hotkeys you setup in Teamspeak 3 to push-to-talk on the respective frequency.  Anyone else on the teamspeak server that is running the plugin and on your frequency should hear you!




Version History
---------------------------------------
V1.0 11/18/12
- Intial release.