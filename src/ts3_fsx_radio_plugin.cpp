/*
 * TeamSpeak 3 to FSX radio app based on ts3 demo project
 *
 * Copyright (c) 2008-2012 TeamSpeak Systems GmbH
 * Copyright (c) 2012 Jason Pharis - Virtual Carrier Wing -
 */

#define BOOST_LIB_DIAGNOSTIC

#ifdef _WIN32
#pragma warning (disable : 4100)  /* Disable Unreferenced parameter warning */
#include <Windows.h>
#endif
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/thread.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include "public_errors.h"
#include "public_errors_rare.h"
#include "public_definitions.h"
#include "public_rare_definitions.h"
#include "clientlib.h"
#include "clientlib_publicdefinitions.h"
#include "ts3_functions.h"
#include "ts3_fsx_radio_plugin.h"
#include "configwindow.h"


using namespace boost::interprocess;

static struct TS3Functions ts3Functions;

#ifdef _WIN32
#define _strcpy(dest, destSize, src) strcpy_s(dest, destSize, src)
#define snprintf sprintf_s
#else
#define _strcpy(dest, destSize, src) { strncpy(dest, src, destSize-1); (dest)[destSize-1] = '\0'; }
#endif

#define PLUGIN_API_VERSION 20

#define PATH_BUFSIZE 512
#define COMMAND_BUFSIZE 128
#define INFODATA_BUFSIZE 128
#define SERVERINFO_BUFSIZE 256
#define CHANNELINFO_BUFSIZE 512
#define RETURNCODE_BUFSIZE 128

#define GUARD_FREQ1 8528   //for guard frequency 121.50
#define GUARD_FREQ2 12336  // for guard frequency 130.30

static char* pluginID = NULL;

enum WHISPER_LIST_COMM
{
	USE_COMM1,
	USE_COMM2
};

uint64 serverHandlerID = 1;
std::vector<int> client_list;

/* ClientID whisper lists */
std::vector<anyID> whisper_comm1;
std::vector<anyID> whisper_comm2;
//std::vector<anyID> whisper_guard;

bool simconnect_status = false;
bool thread_quit = false;
bool thread_exit = false;

struct POSITION_STRUCT 
{ 
    double  latitude; 
    double  longitude; 
}; 

struct SHR_MEM_STRUCT
{
	int comm1_freq;
	int comm2_freq;
	POSITION_STRUCT curr_pos;
};

struct COMM_FREQ
{
	int comm1_freq;
	int comm2_freq;
} comm_freqs;

int *shr_mem_start;
int lock_whisper;
bool whisper_in;

ConfigWindow *config_plugin;  //Pointer for config dialog invoked in api function.


#ifdef _WIN32
/* Helper function to convert wchar_T to Utf-8 encoded strings on Windows */
static int wcharToUtf8(const wchar_t* str, char** result) {
	int outlen = WideCharToMultiByte(CP_UTF8, 0, str, -1, 0, 0, 0, 0);
	*result = (char*)malloc(outlen);
	if(WideCharToMultiByte(CP_UTF8, 0, str, -1, *result, outlen, 0, 0) == 0) {
		*result = NULL;
		return -1;
	}
	return 0;
}
#endif

/*** Non TS3 functions ***/

void update_meta_data()
{
	std::string str;

//	printf("serverHandlerID is %d\n", serverHandlerID);
	str = "COMM1_FREQ:";
	str.append(std::to_string((long long)comm_freqs.comm1_freq));
	str.append(";COMM2_FREQ:");
	str.append(std::to_string((long long)comm_freqs.comm2_freq));

//	printf("string is %s\n", str.c_str());

	if (ts3Functions.setClientSelfVariableAsString(serverHandlerID, CLIENT_META_DATA, str.c_str()) != ERROR_ok) {
		printf("Error setting CLIENT_META_DATA!!!\n");
		return;
	}

	ts3Functions.flushClientSelfUpdates(serverHandlerID, NULL);

}

void update_client_list()
{
	anyID* list;

	client_list.clear();

	
//	printf("Client List Size after clear : %d\n", client_list.size());

	if (ts3Functions.getClientList(serverHandlerID, &list) != ERROR_ok) {
		printf("error getting client list\n");
		return;
	}

	for (int i=0;list[i] != NULL;i++) {
//		printf("client number is %d\n", list[i]);
		client_list.push_back(list[i]);
	}
	ts3Functions.freeMemory(list);
}

void set_whisperlist(anyID clientID, WHISPER_LIST_COMM comm, bool match)
{
	size_t i;

	switch (comm)
	{
		case USE_COMM1: {
			for (i=0; i < whisper_comm1.size(); i++) {
//				printf("clientid = %d and whisper_comm1 = %d\n", clientID, whisper_comm1[i]);
				if (clientID == whisper_comm1[i]) {
					if (match){
						return;
					} 
					else {
						whisper_comm1.erase(whisper_comm1.begin()+i);
						return;
					}
				}
			}
				// if it makes it this far add to end of array before the NULL last element
				if (match) {
					whisper_comm1.insert(whisper_comm1.end()-1, clientID);
				}
				return;
		
			break;
		}

		case USE_COMM2: {
			for (i=0; i < whisper_comm2.size(); i++) {
				if (clientID == whisper_comm2[i]) {
					if (match){
						return;
					} 
					else {
						whisper_comm2.erase(whisper_comm2.begin()+i);
						return;
						}
					}
				}
				// if it makes it this far add to end of array before the NULL last element
				if (match) {
					whisper_comm2.insert(whisper_comm2.end()-1, clientID);
				}
				return;
			
			break;
		}
	}
	return;
}

void get_client_data()
{
	size_t pos;
	std::string parse_str, str1, str2;
	int comm1, comm2;
	char* result;
	anyID own_id;

	int j;

	ts3Functions.getClientID(serverHandlerID, &own_id);

//	printf("Client List Size : %d\n", client_list.size());

	for (int i=0;i < client_list.size(); i++) {
		if (client_list[i] != own_id) {
			if (ts3Functions.getClientVariableAsString(serverHandlerID, client_list[i], CLIENT_META_DATA, &result) != ERROR_ok) {
				printf("Error Getting Client Variable\n");
			}
		
			parse_str = result;

			if (parse_str.size() != 0)
			{
				pos = parse_str.find(';',0);

				str1 = parse_str.substr(0,pos);
				str2 = parse_str.substr(pos+1,parse_str.size());

				pos = str1.find(':',0);
				str1 = str1.substr(pos+1, str1.size());

				pos = str2.find(':',0);
				str2 = str2.substr(pos+1, str2.size());

				comm1 = atoi(str1.c_str());
//				printf("comm1 for client %d is %d\n", client_list[i], comm1);
				comm2 = atoi(str2.c_str());
//				printf("comm2 for client %d is %d\n", client_list[i], comm1);


				
				if (((comm1 == comm_freqs.comm1_freq) || (comm2 == comm_freqs.comm1_freq)) && (comm_freqs.comm1_freq != 0)) {
					set_whisperlist(client_list[i], USE_COMM1, true); 
				}
				else {
					set_whisperlist(client_list[i], USE_COMM1, false);
				}

				if (((comm1 == comm_freqs.comm2_freq) || (comm2 == comm_freqs.comm2_freq)) && (comm_freqs.comm2_freq != 0)) {
					set_whisperlist(client_list[i], USE_COMM2, true);  
				}
				else {
					set_whisperlist(client_list[i], USE_COMM2, false);
				}
				
				if (((comm_freqs.comm1_freq == GUARD_FREQ1) /*|| (comm_freqs.comm1_freq == GUARD_FREQ2)*/) && ((comm1 != 0) || (comm2 != 0)))
				{
					set_whisperlist(client_list[i], USE_COMM1, true);
				}

				if (((comm_freqs.comm2_freq == GUARD_FREQ1)/* || (comm_freqs.comm2_freq == GUARD_FREQ2)*/) && ((comm1 != 0) || (comm2 != 0)))
				{
					set_whisperlist(client_list[i], USE_COMM2, true);
				}

				printf("whisperlist1 contains");
				for (j=0; j<whisper_comm1.size();j++) {
					printf(" %d ",whisper_comm1[j]);
				}
				printf("\n");

				printf("whisperlist2 contains");
				for (j=0; j<whisper_comm2.size();j++) {
					printf(" %d ",whisper_comm2[j]);
				}
				printf("\n");

				ts3Functions.freeMemory(result);
			}	
		}
	}
}
void check_shr_mem_thrd()
{
	bool updated_data = false;
	
	SHR_MEM_STRUCT* data_from_fsx;
	while (1)
	{
		if (thread_quit) {
			break; 
		}
		
		try {
			shared_memory_object shr_mem(open_only, "radio_freq_shrd", read_only);
			mapped_region region(shr_mem, read_only);

			shr_mem_start = (int*)region.get_address();

//			printf("region address is %x\n", shr_mem_start);

			data_from_fsx = (SHR_MEM_STRUCT*)shr_mem_start;

			if (data_from_fsx->comm1_freq != comm_freqs.comm1_freq)
			{
				comm_freqs.comm1_freq = data_from_fsx->comm1_freq;
				updated_data = true;
			}

			if (data_from_fsx->comm2_freq != comm_freqs.comm2_freq)
			{
				comm_freqs.comm2_freq = data_from_fsx->comm2_freq;
				updated_data = true;
			}

			if (updated_data) {
				update_meta_data();
				updated_data = false;
			}

			update_client_list();

			get_client_data();

//			printf("comm1 freq is %d\n", data_from_fsx->comm1_freq);
//			printf("comm2 freq is %d\n", data_from_fsx->comm2_freq);
/*			
			printf("latitude is %f\n", data_from_fsx->curr_pos.latitude);
			printf("longitude is %f\n", data_from_fsx->curr_pos.longitude);
			*/
		}
		catch(interprocess_exception &ex)
		{
			printf("exception %s\n",ex.what());
			comm_freqs.comm1_freq = 0;
			comm_freqs.comm2_freq = 0;
		}
		
		Sleep(1000);
	}

	thread_exit = true;
}

void set_client_whisper_list(WHISPER_LIST_COMM comm) {

	switch (comm) {
		case USE_COMM1:
			if (ts3Functions.requestClientSetWhisperList(serverHandlerID, 0, (uint64*)NULL, &whisper_comm1[0], NULL) != ERROR_ok) {
				printf("problem setting whisper list\n");
			}
			break;
		case USE_COMM2:
			if (ts3Functions.requestClientSetWhisperList(serverHandlerID, 0, (uint64*)NULL, &whisper_comm2[0], NULL) != ERROR_ok) {
				printf("problem setting whisper list\n");
			}
			break;
	}
}

void clear_client_whisper_list()
{
	if (ts3Functions.requestClientSetWhisperList(serverHandlerID, 0, (uint64*)NULL, (anyID*)NULL, NULL) != ERROR_ok) {
			printf("problem clearing whisper list\n");
	}

}

/*********************************** Required functions ************************************/
/*
 * If any of these required functions is not implemented, TS3 will refuse to load the plugin
 */

/* Unique name identifying this plugin */
const char* ts3plugin_name() {
#ifdef _WIN32
	/* TeamSpeak expects UTF-8 encoded characters. Following demonstrates a possibility how to convert UTF-16 wchar_t into UTF-8. */
	static char* result = NULL;  /* Static variable so it's allocated only once */
	if(!result) {
		const wchar_t* name = L"TS3 FSX radio plugin";
		if(wcharToUtf8(name, &result) == -1) {  /* Convert name into UTF-8 encoded result */
			result = "TS3 FSX radio plugin";  /* Conversion failed, fallback here */
		}
	}
	return result;
#else
	return "TS3 FSX radio plugin";
#endif
}

/* Plugin version */
const char* ts3plugin_version() {
    return "1.6";
}

/* Plugin API version. Must be the same as the clients API major version, else the plugin fails to load. */
int ts3plugin_apiVersion() {
	return PLUGIN_API_VERSION;
}

/* Plugin author */
const char* ts3plugin_author() {
	/* If you want to use wchar_t, see ts3plugin_name() on how to use */
    return "Jason Pharis";
}

/* Plugin description */
const char* ts3plugin_description() {
	/* If you want to use wchar_t, see ts3plugin_name() on how to use */
    return "Used with SimConnect FSX addon to get in-sim radio frequency settings and use within TeamSpeak 3 to create self contained radio enviroments with minimal input from the user";
}

/* Set TeamSpeak 3 callback functions */
void ts3plugin_setFunctionPointers(const struct TS3Functions funcs) {
    ts3Functions = funcs;
}

/*
 * Custom code called right after loading the plugin. Returns 0 on success, 1 on failure.
 * If the function returns 1 on failure, the plugin will be unloaded again.
 */
int ts3plugin_init() {
	freopen("CONOUT$", "wb", stdout);
	comm_freqs.comm1_freq = 0;
	comm_freqs.comm2_freq = 0;

	whisper_comm1.push_back(NULL);
	whisper_comm2.push_back(NULL);

	boost::thread updateThrd(check_shr_mem_thrd);
    return 0;  
}

/* Custom code called right before the plugin is unloaded */
void ts3plugin_shutdown() {
    /* Your plugin cleanup code here */
	thread_quit = true;

    printf("TS3 FSX Radio Plugin: shutdown\n");

    while(!thread_exit)
	{
		Sleep(50);
	}

	if(pluginID) {
		free(pluginID);
		pluginID = NULL;
	}
}

/****************************** Optional functions ********************************/
/*
 * Following functions are optional, if not needed you don't need to implement them.
 */

/* Tell client if plugin offers a configuration window. If this function is not implemented, it's an assumed "does not offer" (PLUGIN_OFFERS_NO_CONFIGURE). */
int ts3plugin_offersConfigure() {
	printf("PLUGIN: offersConfigure\n");
	/*
	 * Return values:
	 * PLUGIN_OFFERS_NO_CONFIGURE         - Plugin does not implement ts3plugin_configure
	 * PLUGIN_OFFERS_CONFIGURE_NEW_THREAD - Plugin does implement ts3plugin_configure and requests to run this function in an own thread
	 * PLUGIN_OFFERS_CONFIGURE_QT_THREAD  - Plugin does implement ts3plugin_configure and requests to run this function in the Qt GUI thread
	 */
	return PLUGIN_OFFERS_CONFIGURE_QT_THREAD;  /* In this case ts3plugin_configure does not need to be implemented */
}

/* Plugin might offer a configuration window. If ts3plugin_offersConfigure returns 0, this function does not need to be implemented. */
void ts3plugin_configure(void* handle, void* qParentWidget) {
    printf("PLUGIN: configure\n");

	config_plugin = new ConfigWindow;
	config_plugin->setAttribute(Qt::WA_DeleteOnClose);
	config_plugin->set_ts3func(&ts3Functions);
	config_plugin->set_pluginID(pluginID);
	config_plugin->set_all_ptt_labels();
	config_plugin->show();
}

/*
 * If the plugin wants to use error return codes, plugin commands, hotkeys or menu items, it needs to register a command ID. This function will be
 * automatically called after the plugin was initialized. This function is optional. If you don't use these features, this function can be omitted.
 * Note the passed pluginID parameter is no longer valid after calling this function, so you must copy it and store it in the plugin.
 */
void ts3plugin_registerPluginID(const char* id) {
	const size_t sz = strlen(id) + 1;
	pluginID = (char*)malloc(sz * sizeof(char));
	_strcpy(pluginID, sz, id);  /* The id buffer will invalidate after exiting this function */
	printf("FSX PLUGIN: registerPluginID: %s\n", pluginID);
}

/*
 * Implement the following three functions when the plugin should display a line in the server/channel/client info.
 * If any of ts3plugin_infoTitle, ts3plugin_infoData or ts3plugin_freeMemory is missing, the info text will not be displayed.
 */

/* Static title shown in the left column in the info frame */
const char* ts3plugin_infoTitle() {
	return "TS3 to FSX Radio Plugin";
}

/*
 * Dynamic content shown in the right column in the info frame. Memory for the data string needs to be allocated in this
 * function. The client will call ts3plugin_freeMemory once done with the string to release the allocated memory again.
 * Check the parameter "type" if you want to implement this feature only for specific item types. Set the parameter
 * "data" to NULL to have the client ignore the info data.
 */
void ts3plugin_infoData(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, char** data) {
	char* name;

	serverHandlerID = serverConnectionHandlerID;

	/* For demonstration purpose, display the name of the currently selected server, channel or client. */
	switch(type) {
		case PLUGIN_CLIENT:
			if(ts3Functions.getClientVariableAsString(serverConnectionHandlerID, (anyID)id, CLIENT_META_DATA, &name) != ERROR_ok) {
				printf("Error getting client meta data\n");
				return;
			}
			break;
		default:
			printf("Invalid item type: %d\n", type);
			data = NULL;  /* Ignore */
			return;
	}

	*data = (char*)malloc(INFODATA_BUFSIZE * sizeof(char));  /* Must be allocated in the plugin! */
	snprintf(*data, INFODATA_BUFSIZE, "meta data is \"%s\"", name);
	ts3Functions.freeMemory(name);
}

/* Required to release the memory for parameter "data" allocated in ts3plugin_infoData and ts3plugin_initMenus */
void ts3plugin_freeMemory(void* data) {
	free(data);
}

/*
 * Plugin requests to be always automatically loaded by the TeamSpeak 3 client unless
 * the user manually disabled it in the plugin dialog.
 * This function is optional. If missing, no autoload is assumed.
 */
int ts3plugin_requestAutoload() {
	return 1;  /* 1 = request autoloaded, 0 = do not request autoload */
}

/* Helper function to create a hotkey */
static struct PluginHotkey* createHotkey(const char* keyword, const char* description) {
	struct PluginHotkey* hotkey = (struct PluginHotkey*)malloc(sizeof(struct PluginHotkey));
	_strcpy(hotkey->keyword, PLUGIN_HOTKEY_BUFSZ, keyword);
	_strcpy(hotkey->description, PLUGIN_HOTKEY_BUFSZ, description);
	return hotkey;
}

/* Some makros to make the code to create hotkeys a bit more readable */
#define BEGIN_CREATE_HOTKEYS(x) const size_t sz = x + 1; size_t n = 0; *hotkeys = (struct PluginHotkey**)malloc(sizeof(struct PluginHotkey*) * sz);
#define CREATE_HOTKEY(a, b) (*hotkeys)[n++] = createHotkey(a, b);
#define END_CREATE_HOTKEYS (*hotkeys)[n++] = NULL; assert(n == sz);

/*
 * Initialize plugin hotkeys. If your plugin does not use this feature, this function can be omitted.
 * Hotkeys require ts3plugin_registerPluginID and ts3plugin_freeMemory to be implemented.
 * This function is automatically called by the client after ts3plugin_init.
 */
void ts3plugin_initHotkeys(struct PluginHotkey*** hotkeys) {
	/* Register hotkeys giving a keyword and a description.
	 * The keyword will be later passed to ts3plugin_onHotkeyEvent to identify which hotkey was triggered.
	 * The description is shown in the clients hotkey dialog. */
	BEGIN_CREATE_HOTKEYS(4);  /* Create 4 hotkeys. Size must be correct for allocating memory. */
	CREATE_HOTKEY("FSX_COMM1_PTT_PRESS", "FSX Comm1 PTT_keydown");
	CREATE_HOTKEY("FSX_COMM1_PTT_RELEASE", "FSX Comm1 PTT_keyup");
	CREATE_HOTKEY("FSX_COMM2_PTT_PRESS", "FSX Comm2 PTT_keydown");
	CREATE_HOTKEY("FSX_COMM2_PTT_RELEASE", "FSX Comm2 PTT_keyup");
	END_CREATE_HOTKEYS;

	/* The client will call ts3plugin_freeMemory to release all allocated memory */
}

/************************** TeamSpeak callbacks ***************************/
/*
 * Following functions are optional, feel free to remove unused callbacks.
 * See the clientlib documentation for details on each function.
 */

/* Clientlib */

void ts3plugin_onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber) {
	serverHandlerID = serverConnectionHandlerID;
	update_meta_data();
}

void ts3plugin_onUpdateClientEvent(uint64 serverConnectionHandlerID, anyID clientID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {

	printf("clientEvent\n");

}

void ts3plugin_onNewChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID) {
}

void ts3plugin_onNewChannelCreatedEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {
}

void ts3plugin_onDelChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {
}

void ts3plugin_onChannelMoveEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 newChannelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {
	
}

void ts3plugin_onUpdateChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
}

void ts3plugin_onUpdateChannelEditedEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {
}

void ts3plugin_onUpdateClientEvent(uint64 serverConnectionHandlerID, anyID clientID) {
		serverHandlerID = serverConnectionHandlerID;
		update_meta_data();
}

void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage) {
		serverHandlerID = serverConnectionHandlerID;
		update_meta_data();
}

void ts3plugin_onClientMoveSubscriptionEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility) {
}

void ts3plugin_onClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage) {
}

void ts3plugin_onClientMoveMovedEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID moverID, const char* moverName, const char* moverUniqueIdentifier, const char* moveMessage) {
}

void ts3plugin_onClientKickFromChannelEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage) {
}

void ts3plugin_onClientKickFromServerEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage) {
}

void ts3plugin_onClientIDsEvent(uint64 serverConnectionHandlerID, const char* uniqueClientIdentifier, anyID clientID, const char* clientName) {
}

void ts3plugin_onClientIDsFinishedEvent(uint64 serverConnectionHandlerID) {
		serverHandlerID = serverConnectionHandlerID;
		update_meta_data();
}

void ts3plugin_onServerEditedEvent(uint64 serverConnectionHandlerID, anyID editerID, const char* editerName, const char* editerUniqueIdentifier) {
}

void ts3plugin_onServerUpdatedEvent(uint64 serverConnectionHandlerID) {
		serverHandlerID = serverConnectionHandlerID;
		update_meta_data();
}

int ts3plugin_onServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage) {
	return 0;
}

void ts3plugin_onServerStopEvent(uint64 serverConnectionHandlerID, const char* shutdownMessage) {
}

int ts3plugin_onTextMessageEvent(uint64 serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored) {
	return 0;
}

void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID) {
	whisper_in = false;

	if (status && isReceivedWhisper) {
		if (ts3Functions.playWaveFile(serverHandlerID, "plugins/fsx_radio-beep.wav") != ERROR_ok) {
			printf("can't play wav\n");
		}
		whisper_in = true;
	}
/*
	anyID myID;

	if (ts3Functions.getClientID(serverHandlerID, &myID) == ERROR_ok) 
	{
		if ((myID == clientID) && (!isReceivedWhisper))
		{
			lock_whisper = status ? 1 : 0;
		}
	}
*/
}

void ts3plugin_onConnectionInfoEvent(uint64 serverConnectionHandlerID, anyID clientID) {
}

void ts3plugin_onServerConnectionInfoEvent(uint64 serverConnectionHandlerID) {
}

void ts3plugin_onChannelSubscribeEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
}

void ts3plugin_onChannelSubscribeFinishedEvent(uint64 serverConnectionHandlerID) {
}

void ts3plugin_onChannelUnsubscribeEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
}

void ts3plugin_onChannelUnsubscribeFinishedEvent(uint64 serverConnectionHandlerID) {
}

void ts3plugin_onChannelDescriptionUpdateEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
}

void ts3plugin_onChannelPasswordChangedEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
}

void ts3plugin_onPlaybackShutdownCompleteEvent(uint64 serverConnectionHandlerID) {
}

void ts3plugin_onSoundDeviceListChangedEvent(const char* modeID, int playOrCap) {
}

void ts3plugin_onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels) {
//	printf("samples are %d\n", samples[200]);

	if (whisper_in) {
		int i;
		for (i=0;i<sampleCount;i++) {
			if (i%6 == 0) {
			samples[i+1] = samples[i];
			samples[i+2] = samples[i];
			samples[i+3] = samples[i];
			samples[i+4] = samples[i];
			samples[i+5] = samples[i];
		}
		}

		for (i=0;i<sampleCount;i++) {
				samples[i] = samples[i] & 0xFFC0;
	
		}
	}
}


void ts3plugin_onEditPostProcessVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask) {
}

void ts3plugin_onEditMixedPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask) {
}

void ts3plugin_onEditCapturedVoiceDataEvent(uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, int* edited) {
}

void ts3plugin_onCustom3dRolloffCalculationEvent(uint64 serverConnectionHandlerID, anyID clientID, float distance , float* volume ){
}

void ts3plugin_onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString) {
}


/* Client UI callbacks */

/*
 * Called from client when an avatar image has been downloaded to or deleted from cache.
 * This callback can be called spontaneously or in response to ts3Functions.getAvatar()
 */
void ts3plugin_onAvatarUpdated(uint64 serverConnectionHandlerID, anyID clientID, const char* avatarPath) {

}

/* This function is called if a plugin hotkey was pressed. Omit if hotkeys are unused. */
void ts3plugin_onHotkeyEvent(const char* keyword) {

	int talking;

		if (strcmp(keyword, "FSX_COMM1_PTT_PRESS") == 0) {

			if (ts3Functions.getClientSelfVariableAsInt(serverHandlerID, CLIENT_FLAG_TALKING, &talking) == ERROR_ok) {

//				printf("talking is %d\n", talking);

				switch(talking) {
				case STATUS_TALKING:
					break;

				case STATUS_NOT_TALKING: {
	//				    printf("i'm here\n");
						set_client_whisper_list(USE_COMM1);

						lock_whisper = 1;

						if (ts3Functions.setClientSelfVariableAsInt(serverHandlerID, CLIENT_INPUT_DEACTIVATED, INPUT_ACTIVE) != ERROR_ok) {
							printf("Activating PTT failed\n");
						}

						ts3Functions.flushClientSelfUpdates(serverHandlerID, NULL);
						break;
					}
				default:
					printf("INvalid value for CLIENT_FLAG_TALKING\n");
				}
			}
		}
		else if (strcmp(keyword, "FSX_COMM1_PTT_RELEASE") == 0) {

			if (lock_whisper) {
				if (ts3Functions.setClientSelfVariableAsInt(serverHandlerID, CLIENT_INPUT_DEACTIVATED, INPUT_DEACTIVATED) != ERROR_ok) {
					printf("Activating PTT failed\n");
				}

				ts3Functions.flushClientSelfUpdates(serverHandlerID, NULL);
				clear_client_whisper_list();

				lock_whisper = 0;
			}
		}
		else if (strcmp(keyword, "FSX_COMM2_PTT_PRESS") == 0) {
						if (ts3Functions.getClientSelfVariableAsInt(serverHandlerID, CLIENT_FLAG_TALKING, &talking) == ERROR_ok) {

				switch(talking) {
				case STATUS_TALKING:
					break;

				case STATUS_NOT_TALKING: {
							set_client_whisper_list(USE_COMM2);

							lock_whisper = 1;
							if (ts3Functions.setClientSelfVariableAsInt(serverHandlerID, CLIENT_INPUT_DEACTIVATED, INPUT_ACTIVE) != ERROR_ok) {
								printf("Activating PTT failed\n");
							}

							ts3Functions.flushClientSelfUpdates(serverHandlerID, NULL);
							break;
					}
				default:
					printf("INvalid value for CLIENT_FLAG_TALKING\n");
				}
			}
		}
		else if (strcmp(keyword, "FSX_COMM2_PTT_RELEASE") == 0) {
			if (lock_whisper) {
				if (ts3Functions.setClientSelfVariableAsInt(serverHandlerID, CLIENT_INPUT_DEACTIVATED, INPUT_DEACTIVATED) != ERROR_ok) {
					printf("Activating PTT failed\n");
				}

				ts3Functions.flushClientSelfUpdates(serverHandlerID, NULL);
				clear_client_whisper_list();

				lock_whisper = 0;
			}
		}
}

/* Called when recording a hotkey has finished after calling ts3Functions.requestHotkeyInputDialog */
void ts3plugin_onHotkeyRecordedEvent(const char* keyword, const char* key) {
	config_plugin->set_ptt_label(keyword);
}
