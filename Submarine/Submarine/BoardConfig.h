#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

//camera pins-----------------------------------

#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  15
#define SIOD_GPIO_NUM  4
#define SIOC_GPIO_NUM  5

#define Y2_GPIO_NUM 11
#define Y3_GPIO_NUM 9
#define Y4_GPIO_NUM 8
#define Y5_GPIO_NUM 10
#define Y6_GPIO_NUM 12
#define Y7_GPIO_NUM 18
#define Y8_GPIO_NUM 17
#define Y9_GPIO_NUM 16

#define VSYNC_GPIO_NUM 6
#define HREF_GPIO_NUM  7
#define PCLK_GPIO_NUM  13

//w5500 (ETHERNET) pins----------------------------

#define W5500_SCLK 14
#define W5500_CS   45
#define W5500_MOSI 21
#define W5500_MISO 47
#define W5500_RST  48

//Config functions ----------------------------------

camera_config_t CameraConfig(){
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 25000000; //25000000 | 20000000 | 10000000 | 8000000 | 6000000
  config.frame_size = FRAMESIZE_320X320; //FRAMESIZE_240X240 | FRAMESIZE_QQVGA
  config.pixel_format = PIXFORMAT_JPEG; //streming
  config.grab_mode = CAMERA_GRAB_LATEST; //CAMERA_GRAB_WHEN_EMPTY | CAMERA_GRAB_LATEST
  config.fb_location = CAMERA_FB_IN_PSRAM; // CAMERA_FB_IN_DRAM | CAMERA_FB_IN_PSRAM
  config.jpeg_quality = 20; //0-63, for OV series camera sensors, lower number means higher quality
  config.fb_count = 1;
  return config;
}

#endif
