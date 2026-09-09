#ifndef CameraStream
#define CameraStream

#include "sensor.h"
#include "esp_camera.h"
#include "BoardConfig.h"

struct PacketHeader {
    uint32_t frameId;       // Fame inique ID
    uint32_t totalSize;     // bytes total size
    uint32_t chunkId;       // slice index
    uint32_t totalChunks;   // total fragments size expected

     // Construtor
    PacketHeader(uint32_t fid, uint32_t tsize, uint32_t cidx, uint32_t tchunks)
        : frameId(fid), totalSize(tsize), chunkId(cidx), totalChunks(tchunks) {}
};

inline void (*onToSendFrameSlice)(const PacketHeader& header, const uint8_t* data, size_t data_len) = nullptr;

// 1400 bytes - header size
const size_t CHUNK_DATA_SIZE = 1400 - sizeof(PacketHeader);

void CameraStreamCallback(void (*ToSendFrameSlice)(const PacketHeader& header, const uint8_t* data, size_t data_len)){
  onToSendFrameSlice = ToSendFrameSlice;
}

void StreamInit() {
  
  //Check esp32-s3 cam
  Serial.println("=== HARDWARE CHECK ESP32-S3 CAM ===");

  // turn camera off
  pinMode(PWDN_GPIO_NUM, OUTPUT);
  digitalWrite(PWDN_GPIO_NUM, HIGH);
  delay(1000);

  uint32_t flashSize = ESP.getFlashChipSize();
  Serial.printf("Flash size: %d MB (%d Bytes)\n", flashSize / (1024 * 1024), flashSize);
  
  if (psramInit()) {
    Serial.println("=== PSRAM initialized ===");
    uint32_t psramSize = ESP.getPsramSize();
    uint32_t freePsram = ESP.getFreePsram();
    Serial.printf("PSRAM total size: %d MB (%d Bytes)\n", psramSize / (1024 * 1024), psramSize);
    Serial.printf("PSRAM Livre: %d Bytes\n", freePsram);
  } else {
    Serial.println("ERR: PSRAM fail or desactivated");
  }

  //initializing camera
  Serial.println("=== Camera initializing... ===");

  digitalWrite(PWDN_GPIO_NUM, LOW);
  delay(1000);

  camera_config_t config = CameraConfig();
  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Fail on camera initilizing: 0x%x\n", err);
    return;
  }

  Serial.println("Camera initialized!");
  delay(1000);
  
  sensor_t * s = esp_camera_sensor_get();
  if (s != NULL) {
    Serial.printf("Detected camera - PID: 0x%02X, VER: 0x%02X\n", s->id.PID, s->id.VER);
  } else {
    Serial.println("Camera detection fail");
    return;
  }
  
  Serial.println("=== Camera initialized ===");

  delay(1000);
}

void DebugPacketHeader(const PacketHeader& header) {
    Serial.printf("frame_id=%u, total_size=%u, chunk_index=%u/%u\n",
                  header.frameId, header.totalSize, 
                  header.chunkId + 1, header.totalChunks);
}

void FrameSplit(void *pvParameters) {
  while (true) {
    // get camera frame
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }

    // calc how mane slice given the frame size
    size_t total_bytes = fb->len;
    size_t total_chunks = (total_bytes + CHUNK_DATA_SIZE - 1) / CHUNK_DATA_SIZE;
    static uint32_t frame_id = 0;

    // send eache frame with his header
    for (size_t i = 0; i < total_chunks; i++) {
      size_t offset = i * CHUNK_DATA_SIZE;
      size_t data_len = (i == total_chunks - 1) ? (total_bytes - offset) : CHUNK_DATA_SIZE;

      // setup the header
      PacketHeader header(frame_id, total_bytes, i, total_chunks);
      if(onToSendFrameSlice != nullptr) {
        onToSendFrameSlice(header, fb->buf + offset, data_len);
        DebugPacketHeader(header);
      }

      // short time out to not overloas connection queue core usage by this task
      taskYIELD();
    }

    esp_camera_fb_return(fb);
    frame_id++;

    // hardcode controll FPS
    vTaskDelay(pdMS_TO_TICKS(5000)); 
  }
}

#endif