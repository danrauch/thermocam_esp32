#pragma once

#include <cstddef>

#include "types/common_types.h"
#include "types/container_types.h"

// Forward-declare ESP-IDF HTTP types without forcing the header on all users.
extern "C" {
struct httpd_req;
struct httpd_data;
struct httpd_uri;
}
typedef httpd_data *httpd_handle_t;
typedef httpd_uri httpd_uri_t;

namespace thermocam::web_server {

class WebServer
{
public:
    WebServer() = default;

    // Initialize WiFi and start the HTTP server with /stats, /snapshot.jpg and /stream endpoints.
    void init();

    // Provide the latest upscaled thermal frame and stats to the web server.
    void update_frame(const UpscaledRGBThermoImage &frame, const ThermoImageStats &stats);

    // True while at least one client is connected to the MJPEG /stream endpoint.
    bool is_streaming() const;

    // HTTP request handlers used by the C callbacks.
    int handle_stats(httpd_req *req);
    int handle_snapshot(httpd_req *req);
    int handle_stream(httpd_req *req);

private:
    bool fill_rgb_buffer_from_frame();
    bool encode_latest_frame_to_jpeg(std::size_t &jpeg_size);
    void connect_wifi();
    void start_http_server();

private:
    UpscaledRGBThermoImage latest_frame_{};
    ThermoImageStats latest_stats_{};
    bool has_frame_ = false;

    // JPEG encoder buffers and helpers
    static constexpr int JPEG_MAX_WIDTH = UPSCALED_IMAGE_WIDTH;
    static constexpr int JPEG_MAX_HEIGHT = UPSCALED_IMAGE_HEIGHT;
    static constexpr int JPEG_RGB_STRIDE = JPEG_MAX_WIDTH * 3;
    static constexpr std::size_t JPEG_RGB_BUFFER_SIZE = JPEG_MAX_WIDTH * JPEG_MAX_HEIGHT * 3;
    static constexpr std::size_t JPEG_OUTPUT_BUFFER_SIZE = 64 * 1024; // 64 KB output buffer

    uint8_t rgb_buffer_[JPEG_RGB_BUFFER_SIZE]{};
    uint8_t jpeg_buffer_[JPEG_OUTPUT_BUFFER_SIZE]{};

    httpd_handle_t httpd_ = nullptr;
    volatile int stream_client_count_ = 0;

    httpd_uri_t *stats_uri_ = nullptr;
    httpd_uri_t *snapshot_uri_ = nullptr;
    httpd_uri_t *stream_uri_ = nullptr;
};

} // namespace thermocam::web_server
