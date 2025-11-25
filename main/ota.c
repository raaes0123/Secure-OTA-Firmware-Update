#include "mongoose.h"
#include "event.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_log.h"

#include "lvgl.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define CHUNK_SIZE 1024

extern QueueHandle_t lvgl_queue;

// First web page in history
// static const char *s_url = "http://info.cern.ch/";  
//static const char *s_url = "http://188.184.67.127/188";
//static const char *s_url = "http://info.cern.ch/hypertext/WWW/TheProject.html";

static const char *s_url = "http://10.57.234.25:8443/firmware";

//static const char *s_url = "http://192.168.1.12:8443/firmware";

static const char *ca =
"-----BEGIN CERTIFICATE-----\n"
"MIICxzCCAa+gAwIBAgIUQBt7+2CCpoceI68423IXIPOZeDAwDQYJKoZIhvcNAQEL\n"
"BQAwEzERMA8GA1UEAwwITXlSb290Q0EwHhcNMjUxMTAxMTkwNTE1WhcNMjgwODIx\n"
"MTkwNTE1WjATMREwDwYDVQQDDAhNeVJvb3RDQTCCASIwDQYJKoZIhvcNAQEBBQAD\n"
"ggEPADCCAQoCggEBAPAl03g/yjPXY5U+JTtWGww5RKXCNfp/3u4Tg/kIXoV2oNbc\n"
"OAcsHRRruX8bWezal3GK5dFGXUfHEeHLQG6BS18m5bRT50XK+vmtkud8dB0UifBw\n"
"7XTSQZA8QDMKgJsIpdfqBwQl0/1iBOgqjPDgeQfXGmze5iDGiieGWm2F8Zqvcg5t\n"
"2UyD2HsFCzcu2A01WqaL1qluOkWxaG7ClxywapjqjO0otUCiQxN0LdHT7VrymoeY\n"
"hM1HSMlWdmBbq8HA4KWzZuu9MPOiPCNzetac8dJvRhr9X8H1ZkXJsgMRzzn2tPkd\n"
"o7F/h9OfGbEtnFfNxKbiUwDXsXHO4HWcLDt8KGECAwEAAaMTMBEwDwYDVR0TAQH/\n"
"BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAJyPoHUYbsG6MoQ6/RuQrg6mvCcXX\n"
"I25N9f4YOIIHbtITWPQH62HNkMCNAtj0lbXHqyYiR312siKAw0MlKDhdkQCF/dDU\n"
"aBp0uD03UVQzJ6eImsGgK6ClBIzHg7ULqSg2t/9lha4pM2pbE05SwXGpkIHjKph/\n"
"wHuzragkyPX3/OWOsNem6WVCQJjzJcEFmYgdOt5QqxMosx7/VC28tsk+cFapl0UK\n"
"O2fLQCm5g3A1I4fpS/HHetubmcrHqYkcGfyZn0Xju8VlBruejHTy5cALq06hWAWy\n"
"1TBGg9k6PPon6PFTPZ9dC1HdGH+woae/NpTCWfgI4Tuqf9HLrhFH7yoJKw==\n"
"-----END CERTIFICATE-----\n";

// Get request
void getRequest(struct mg_connection *c, uint32_t start, uint32_t end){
  struct mg_str host = mg_url_host(s_url);
  // Send request
  mg_printf(c,
            "GET %s HTTP/1.0\r\n"
            "Host: %.*s\r\n"
            "Range: bytes=%lu-%lu\r\n"
            "Connection: close\r\n"
            "\r\n",
            mg_url_uri(s_url), (int) host.len, host.buf, start, end);
}

uint32_t start = 0;
uint32_t end = -1;
bool head_request_done = false;
uint32_t total_size = 0;
uint32_t current_size = 0;
const esp_partition_t* update_partition;
esp_ota_handle_t update_handle;

// Client event handler function
static void ev_handler(struct mg_connection *c, int ev, void *ev_data) {
  struct mg_mgr *mgr = c->mgr;
  
  if (ev == MG_EV_CONNECT) {
    // TLS Handshake
    struct mg_tls_opts opts = {
      .ca = mg_str(ca),
      .name = mg_str("localhost")
    };
    mg_tls_init(c, &opts);
    printf("TLS initialized!\n");
  }
  
  if (ev == MG_EV_TLS_HS) {
    printf("TLS handshake done!\n");
    if(!head_request_done){
      // Make HEAD request
      struct mg_str host = mg_url_host(s_url);
      mg_printf(c,
                "HEAD %s HTTP/1.0\r\n"
                "Host: %.*s\r\n"
                "Connection: close\r\n"
                "\r\n",
                mg_url_uri(s_url), (int) host.len, host.buf);
      head_request_done = true;
    }
    else{
      uint32_t remaining_size = total_size - current_size; 
      uint16_t request_size = remaining_size >= CHUNK_SIZE ? CHUNK_SIZE : remaining_size;
      start = end + 1;
      end = start + request_size - 1;
      printf("Requesting chunk: %lu-%lu\n", start, end);
      getRequest(c,start,end);
    }
  }

  // Wait for close before opening next connection
  if (ev == MG_EV_CLOSE) {
    if(current_size == total_size && total_size != 0){
      printf("Total size: %lu",total_size);
      printf("Current size: %lu",current_size);
      printf("OTA transfer complete!\n");
      // Ending OTA
      esp_err_t err = esp_ota_end(update_handle);
      if (err != ESP_OK) {
          ESP_LOGE("OTA", "esp_ota_end failed: %s", esp_err_to_name(err));
          return;
      }

      // Set boot partition to the new OTA
      err = esp_ota_set_boot_partition(update_partition);
      if (err != ESP_OK) {
          ESP_LOGE("OTA", "esp_ota_set_boot_partition failed: %s", esp_err_to_name(err));
          return;
      }
      // Disconnect from the server
      mg_wifi_disconnect();

      ESP_LOGI("OTA", "Update complete, restarting...");
      esp_restart();
    }
    else {
      printf("Reconnecting...\n");
      mg_http_connect(mgr, s_url, ev_handler, NULL);
    } 
  }

  if (ev == MG_EV_HTTP_MSG) {
    if(total_size == 0){        // Get total size only the first time
      struct mg_http_message *hm = (struct mg_http_message *) ev_data;
      struct mg_str *len_hdr = mg_http_get_header(hm, "Content-Length");
      if (len_hdr) {
        total_size = atoi(len_hdr->buf);
        printf("Total firmware size: %lu bytes\n", total_size);
        
        // Setup OTA
        update_partition = esp_ota_get_next_update_partition(NULL);
        if (!update_partition) {
            ESP_LOGE("OTA", "No update partition found");
            return;
        }

        ESP_LOGI("OTA", "Updating firmware to partition: %s", update_partition->label);

        esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
        if (err != ESP_OK) {
            ESP_LOGE("OTA", "esp_ota_begin failed: %s", esp_err_to_name(err));
            return;
        }
      }
    }
    else{
      struct mg_http_message *hm = (struct mg_http_message *) ev_data;
      // Update current size of binary file received
      current_size += hm->body.len;
      printf("Byte received: %lu\n", current_size);
      printf("OTA write...\n");

      // Send the progress to the queue
      int percent = (current_size * 100) / total_size;
      xQueueSend(lvgl_queue, &percent, 0);

      esp_err_t err = esp_ota_write(update_handle,(const uint8_t*)hm->body.buf, hm->body.len);
      if (err != ESP_OK) {
          ESP_LOGE("OTA", "esp_ota_write failed: %s", esp_err_to_name(err));
          return;
      }
    }
  }
}

void mg_sleep(uint8_t s){
  while(s>0){
    printf("%d\n",s--);
    fflush(stdout);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void ota_update_task(void *param) {

  // Wait for the notification
  ulTaskNotifyTake(1,portMAX_DELAY);


  printf("Starting Update task\n");

  struct mg_mgr mgr;
  mg_log_set(MG_LL_ERROR);  // Set log level to errror ie only log error
  mg_mgr_init(&mgr);                        // Init manager

  // Or disable globally

  //vTaskDelay(10000/portTICK_PERIOD_MS);
  //mg_sleep(10);     // Wait for 10 seconds
  mg_http_connect(&mgr, s_url, ev_handler, NULL);   // Create client connection
  
  // Poll for any event, waiting up to 1000 ms before returning — then immediately call it again.
  for (;;){
    mg_mgr_poll(&mgr, 1000);
  }  
  mg_mgr_free(&mgr);                        // Cleanup
}
