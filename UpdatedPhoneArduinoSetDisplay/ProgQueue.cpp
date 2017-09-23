#include "Arduino.h"
#include "MegaIOT.h"
ProgQueue::ProgQueue() {
    this->_psClient = NULL;
}

ProgQueue::ProgQueue(PubSubClient& psClient) {
    setPubSubClient(psClient);
}
PubSubClient& ProgQueue::setPubSubClient(PubSubClient& psClient){
    this->_psClient = &psClient;
    return *(this->_psClient);
}

//sets the command to be followed for a certain byte path given returns the object by reference for chaining purposes
ProgQueue& ProgQueue::setCMDef(const uint16_t path, funcMethod _funcMethod, boolCont _boolCont){

}
ProgQueue& ProgQueue::setCMDef(const uint16_t path, funcMethod _funcMethod, contMethod _contMethod){
    struct cmd temp{_contMethod,_funcMethod};
    this->commands[path] = temp;
    return *this;
}

//sets command mode between autonomous and teleopperated (autonomous runs the chain of commands given directly allowing for loops and stuff 
//whereas teleopperated deletes a command upon completion of running)
//returns itself to make it easy to chain also argument is int to allow for addition of other modes.
ProgQueue& ProgQueue::setMode(const uint8_t mode){
    this->_mode = mode;
    return *this;
}
ProgQueue& ProgQueue::setCMD(const uint16_t iCMD, const uint16_t path[3]){
    this->cmdQueue[iCMD] = path[0];
    this->argQueue[iCMD] = path[1]*65536 + path[2];
}
ProgQueue& ProgQueue::setCMDs(const uint16_t iCMD, const uint16_t paths[],const int length){
    for(int i = 0;i<length;i++){
        uint16_t temparr[] = {paths[i*3],paths[i*3+1],paths[i*3+2]};
        setCMD(iCMD+i,temparr);
    }
    return *this;
}
ProgQueue& ProgQueue::insCMD(const uint16_t iCMD, const uint16_t path[3]){

    for(int i = 511;i>iCMD;i--){
        this->cmdQueue[i] = this->cmdqueue[i-1];
        this->argQueue[i] = this->argQueue[i-1];
    }
    this->cmdQueue[iCMD] = path[0];
    this->argQueue[iCMD] = path[1]*65536 + path[2];
    
}
ProgQueue& ProgQueue::insCMDs(const uint16_t iCMD, const uint16_t paths[],const int length){

}
ProgQueue& ProgQueue::loop(){
    
}

ProgQueue& ProgQueue::jumpCMD(const uint16_t toCMD){

}