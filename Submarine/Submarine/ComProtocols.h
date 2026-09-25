#ifndef ComProtocols
#define ComProtocols

enum ProtType : uint8_t {
    PROT_VIDEO = 0x01,
    PROT_ACTION = 0x02,
    PROT_TELEMETRY = 0x03
};

struct FrameProt {
    uint8_t protType = PROT_VIDEO;
    uint32_t frameId;       // fame unique ID
    uint32_t totalSize;     // bytes total size
    uint32_t chunkId;       // slice index
    uint32_t totalChunks;   // total fragments size expected

     // Construtor
    FrameProt(uint32_t fid, uint32_t tsize, uint32_t cidx, uint32_t tchunks)
        : frameId(fid), totalSize(tsize), chunkId(cidx), totalChunks(tchunks) {}
};

const size_t CHUNK_TOTAL_SIZE = 1400;
const size_t CHUNK_DATA_SIZE = CHUNK_TOTAL_SIZE - sizeof(FrameProt);

//frame with header and data
struct UDPPkt {
    uint8_t data[CHUNK_TOTAL_SIZE];
    size_t len;
};

struct ActionProt {
    uint8_t protType = PROT_ACTION;
    uint8_t button;
    uint8_t action;

    // Construtor
    ActionProt(uint8_t btn, bool act)
        : button(btn), action(act) {}
};

struct TelemetryProt {
    uint8_t protType = PROT_TELEMETRY;
};

void DebugPacketHeader(const FrameProt& prot) {
    Serial.printf("frame_id=%u, total_size=%u, chunk_index=%u/%u\n",
                  prot.frameId, prot.totalSize, 
                  prot.chunkId + 1, prot.totalChunks);
}

#endif