#ifndef FrameHanddler
#define FrameHanddler

QueueHandle_t frameSlicesQueue;

const size_t CHUNK_DATA_SIZE = 1400;

#define QUEUE_SIZE 6
struct PacketData {
    uint8_t data[CHUNK_DATA_SIZE];
    size_t len;
};

struct PacketHeader {
    uint32_t frameId;       // Fame inique ID
    uint32_t totalSize;     // bytes total size
    uint32_t chunkId;       // slice index
    uint32_t totalChunks;   // total fragments size expected

     // Construtor
    PacketHeader(uint32_t fid, uint32_t tsize, uint32_t cidx, uint32_t tchunks)
        : frameId(fid), totalSize(tsize), chunkId(cidx), totalChunks(tchunks) {}
};

void DebugPacketHeader(const PacketHeader& header) {
    Serial.printf("frame_id=%u, total_size=%u, chunk_index=%u/%u\n",
                  header.frameId, header.totalSize, 
                  header.chunkId + 1, header.totalChunks);
}

#endif