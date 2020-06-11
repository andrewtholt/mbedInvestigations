
#define MAX_TOPIC 32
#define MAX_MSG 32

enum class msgType {
    INVALID=0,
    HI_LEVEL,
    LO_LEVEL
} ;

enum class highLevelOperation {
    NOP=0,
    GET,
    SET,
    SUB,
    UNSUB
} ;

typedef struct {
// Sender
    highLevelOperation cmd;

    char topic[MAX_TOPIC];
    char msg[MAX_MSG];
} hl_message_t;

enum class lowLevelOperation {
    NOP=0,
    RD,
    WR,
    IOCTL
} ;

typedef struct {
// Sender
    lowLevelOperation cmd;

    uint8_t selector;
    uint16_t deviceRegister;
    void *buffer;
    uint16_t count;
} ll_message_t;

typedef struct {
    msgType type;
    union {
        highLevelOperation hl_op; 
        lowLevelOperation ll_op; 
    } op ;

    union {
        hl_message_t hl_body;
        ll_message_t ll_body;
    } body;
} message_t;


