#include <string>

enum MessageID{

    CHOKE=0,
    UNCHOKE=1,
    INTERSTED=2,
    NOT_INTERESTED=3,
    BITFIELD=5,
    REQUEST=6,
    PIECE=7,
    CANCEL=8,
};

class Message
{
    private:
        uint8_t message_id; // 1 byte
        uint32_t length_prefix; // 4 bytes
        std::string payload; 

    public:
        Message(uint8_t message_id, const std::string& payload);
        std::string create_message();

};