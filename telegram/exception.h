#pragma once

#include <stdexcept>

struct ClientError : std::runtime_error {
    ClientError(const std::string& reason)
        : std::runtime_error{"Client error occured: " + reason}, reason(reason){};

    std::string reason;
};

struct TelegramAPIError : std::runtime_error {
    TelegramAPIError(int http_code, const std::string& reason)
        : std::runtime_error{"Api error: code = " + std::to_string(http_code) +
                             ". Reason: " + reason},
          http_code{http_code},
          reason(std::move(reason)){};

    int http_code;
    std::string reason;
};
