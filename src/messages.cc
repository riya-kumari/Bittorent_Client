#include <string>
#include <sstream>
#include "../include/message.h"

using namespace std;


// Just setting global variables
Message::Message(uint8_t message_id, const string& payload = ""): message_id(message_id), payload(payload){}

std::string Message::create_message(){
    std::stringstream buffer;

    switch(this->message_id){
        case CHOKE: break;
        case UNCHOKE: break;
        case INTERSTED: break;
        case NOT_INTERESTED: break;
        case BITFIELD: break;
        case REQUEST: break;
        case PIECE: break;
        case CANCEL: break;
        
        

    }
    return buffer.str();
    
}


