#include <WiFi.h>

#include <cstdio>
#include <cstring>

// JPEG encoder (provided by bitbank2/JPEGENC in platformio.ini)
#include <JPEGENC.h>

extern "C" {
#include "esp_http_server.h"
}

#include "config.h"
#include "wifi_config.h"
#include "web_server.h"

using namespace thermocam;

namespace thermocam::web_server {

namespace {

// C-style handlers which forward to the C++ WebServer instance via user_ctx.
esp_err_t stats_get_handler(httpd_req_t *req)
{
    auto *self = static_cast<WebServer *>(req->user_ctx);
    return self ? self->handle_stats(req) : ESP_FAIL;
}

esp_err_t snapshot_get_handler(httpd_req_t *req)
{
    auto *self = static_cast<WebServer *>(req->user_ctx);
    return self ? self->handle_snapshot(req) : ESP_FAIL;
}

esp_err_t stream_get_handler(httpd_req_t *req)
{
    auto *self = static_cast<WebServer *>(req->user_ctx);
    return self ? self->handle_stream(req) : ESP_FAIL;
}

} // namespace

// ---- WebServer methods ----------------------------------------------------

bool WebServer::fill_rgb_buffer_from_frame()
{
    if (!has_frame_) {
        return false;
    }

    if (JPEG_MAX_WIDTH != UPSCALED_IMAGE_WIDTH ||
        JPEG_MAX_HEIGHT != UPSCALED_IMAGE_HEIGHT) {
        return false;
    }

    std::size_t index = 0;
    for (std::size_t row = 0; row < latest_frame_.rows(); ++row) {
        for (std::size_t col = 0; col < latest_frame_.cols(); ++col) {
            const auto rgb = latest_frame_(row, col).rgb_array();
            if (index + 3 > JPEG_RGB_BUFFER_SIZE) {
                return false;
            }
            rgb_buffer_[index++] = rgb[0];
            rgb_buffer_[index++] = rgb[1];
            rgb_buffer_[index++] = rgb[2];
        }
    }
    return true;
}

bool WebServer::encode_latest_frame_to_jpeg(std::size_t &jpeg_size)
{
    if (!fill_rgb_buffer_from_frame()) {
        return false;
    }

    JPEGENC jpeg;
    JPEGENCODE state{};

    if (jpeg.open(jpeg_buffer_, static_cast<int>(JPEG_OUTPUT_BUFFER_SIZE)) != JPEGE_SUCCESS) {
        return false;
    }

    if (jpeg.encodeBegin(&state,
                         JPEG_MAX_WIDTH,
                         JPEG_MAX_HEIGHT,
                         JPEGE_PIXEL_RGB888,
                         JPEGE_SUBSAMPLE_420,
                         JPEGE_Q_MED) != JPEGE_SUCCESS) {
        return false;
    }

    if (jpeg.addFrame(&state, rgb_buffer_, JPEG_RGB_STRIDE) != JPEGE_SUCCESS) {
        return false;
    }

    int size = jpeg.close();
    if (size <= 0 || static_cast<std::size_t>(size) > JPEG_OUTPUT_BUFFER_SIZE) {
        return false;
    }

    jpeg_size = static_cast<std::size_t>(size);
    return true;
}

int WebServer::handle_stats(httpd_req *req)
{
    ThermoImageStats local_stats = latest_stats_;

    char buffer[160];
    int len = std::snprintf(buffer, sizeof(buffer),
                            "{\"min\":%.2f,\"max\":%.2f,\"avg\":%.2f,\"frame\":%u}",
                            static_cast<double>(local_stats.min_temp),
                            static_cast<double>(local_stats.max_temp),
                            static_cast<double>(local_stats.average_temp),
                            static_cast<unsigned int>(local_stats.frame_index));

    if (len < 0 || len >= static_cast<int>(sizeof(buffer))) {
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, buffer, len);
    return ESP_OK;
}

int WebServer::handle_snapshot(httpd_req *req)
{
    if (!has_frame_) {
        return ESP_FAIL;
    }

    std::size_t jpeg_size = 0;
    if (!encode_latest_frame_to_jpeg(jpeg_size)) {
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_send(req,
                    reinterpret_cast<const char *>(jpeg_buffer_),
                    static_cast<ssize_t>(jpeg_size));
    return ESP_OK;
}

int WebServer::handle_stream(httpd_req *req)
{
    ++stream_client_count_;

    httpd_resp_set_type(req, "multipart/x-mixed-replace; boundary=frame");

    uint32_t last_frame_index = 0;

    while (true) {
        if (!has_frame_) {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }

        ThermoImageStats local_stats = latest_stats_;
        if (local_stats.frame_index == last_frame_index) {
            vTaskDelay(pdMS_TO_TICKS(30));
            continue;
        }
        last_frame_index = local_stats.frame_index;

        std::size_t jpeg_size = 0;
        if (!encode_latest_frame_to_jpeg(jpeg_size)) {
            break;
        }

        static const char *STREAM_BOUNDARY = "\r\n--frame\r\n";
        if (httpd_resp_send_chunk(req, STREAM_BOUNDARY, std::strlen(STREAM_BOUNDARY)) != ESP_OK) {
            break;
        }

        char header[128];
        static const char *STREAM_PART_HEADER = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";
        int hlen = std::snprintf(header, sizeof(header),
                                 STREAM_PART_HEADER,
                                 static_cast<unsigned int>(jpeg_size));
        if (hlen < 0 || hlen >= static_cast<int>(sizeof(header))) {
            break;
        }

        if (httpd_resp_send_chunk(req, header, hlen) != ESP_OK) {
            break;
        }

        if (httpd_resp_send_chunk(req,
                                  reinterpret_cast<const char *>(jpeg_buffer_),
                                  static_cast<ssize_t>(jpeg_size)) != ESP_OK) {
            break;
        }
    }

    --stream_client_count_;
    httpd_resp_send_chunk(req, nullptr, 0);
    return ESP_OK;
}

void WebServer::connect_wifi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_config::WIFI_SSID, wifi_config::WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if constexpr (DEBUG_OUTPUT) {
            Serial.print(".");
        }
    }

    if constexpr (DEBUG_OUTPUT) {
        Serial.println();
        Serial.print("WiFi connected, IP address: ");
        Serial.println(WiFi.localIP());
    }
}

void WebServer::start_http_server()
{
    if (httpd_ != nullptr) {
        return;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;

    if (httpd_start(&httpd_, &config) == ESP_OK) {
        // Allocate URI descriptors once and associate this as user_ctx.
        static httpd_uri_t stats_uri = {
            .uri = "/stats",
            .method = HTTP_GET,
            .handler = stats_get_handler,
            .user_ctx = this,
        };
        static httpd_uri_t snapshot_uri = {
            .uri = "/snapshot.jpg",
            .method = HTTP_GET,
            .handler = snapshot_get_handler,
            .user_ctx = this,
        };
        static httpd_uri_t stream_uri = {
            .uri = "/stream",
            .method = HTTP_GET,
            .handler = stream_get_handler,
            .user_ctx = this,
        };

        stats_uri_ = &stats_uri;
        snapshot_uri_ = &snapshot_uri;
        stream_uri_ = &stream_uri;

        httpd_register_uri_handler(httpd_, stats_uri_);
        httpd_register_uri_handler(httpd_, snapshot_uri_);
        httpd_register_uri_handler(httpd_, stream_uri_);
    }
}

void WebServer::init()
{
    connect_wifi();
    start_http_server();
}

void WebServer::update_frame(const UpscaledRGBThermoImage &frame, const ThermoImageStats &stats)
{
    latest_frame_ = frame;
    latest_stats_ = stats;
    has_frame_ = true;
}

bool WebServer::is_streaming() const
{
    return stream_client_count_ != 0;
}

} // namespace thermocam::web_server

