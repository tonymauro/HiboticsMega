#ifndef MegaIOT_h
#define MegaIOT_h

#include <Arduino.h>
#include <PubSubClient.h>
#include "Client.h"



class ProgQueue
{
	private:
		PubSubClient* _psClient;
		typedef int (*contMethod)(int);
		typedef void (*funcMethod)(uint8_t *);
		typedef bool (*boolCont)();
		uint8_t _mode;
		struct cmd{
			contMethod contMeth;
			funcMethod funcMeth;
		};
		cmd commands[1024];
		uint16_t cmdQueue[512];
		long argQueue[512];

	public:
		ProgQueue();
		ProgQueue(PubSubClient& psClient);
		
		//sets the PubSubCLient object to be used by the object and returns the PubSubClient to allow chaining to set up values of the pubsubclient
		PubSubClient& setPubSubClient(PubSubClient& psClient);
		
		//sets the command to be followed for a certain byte path given returns the object by reference for chaining purposes
		ProgQueue& setCMDef(const uint16_t path, funcMethod, boolCont);
		ProgQueue& setCMDef(const uint16_t path, funcMethod, contMethod);
		
		//sets command mode between autonomous and teleopperated (autonomous runs the chain of commands given directly allowing for loops and stuff 
		//whereas teleopperated deletes a command upon completion of running)
		//returns itself to make it easy to chain also argument is int to allow for addition of other modes.
		ProgQueue& setMode(const uint8_t mode);
		ProgQueue& setCMD(const uint16_t iCMD, const uint16_t path[]);
		ProgQueue& setCMDs(const uint16_t iCMD, const uint16_t paths[],const int length);
		ProgQueue& insCMD(const uint16_t iCMD, const uint16_t path[]);
		ProgQueue& insCMDs(const uint16_t iCMD, const uint16_t paths[],const int length);

		ProgQueue& loop();

		ProgQueue& jumpCMD(const uint16_t toCMD);


};
#endif