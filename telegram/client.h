#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/URI.h>

#include "utils.h"
#include "exception.h"
#include "bot_base.h"

class Client {
public:
    Client() = delete;
    Client(const std::string& base_url, std::unique_ptr<BotBase> bot,
           const std::string& offset_file_path, std::optional<int32_t> timeout = std::nullopt)
        : bot_(std::move(bot)),
          url_(base_url + "bot" + bot_->GetToken()),
          offset_file_path_(offset_file_path),
          timeout_(timeout){};

    void Run();
    void Stop();

    void Respond(std::shared_ptr<Update> update);
    std::optional<int64_t> GetOffset();
    void SetOffset(int64_t new_offset);

    User GetMe() const;

    std::vector<std::shared_ptr<Update>> GetUpdates(
        std::optional<int64_t> offset = std::nullopt,
        std::optional<int32_t> timeout = std::nullopt) const;

    void SendMessage(std::shared_ptr<ResponseMessage> message) const;

private:
    std::unique_ptr<BotBase> bot_;
    std::string url_;
    std::string offset_file_path_;
    std::optional<int32_t> timeout_;
    bool stop_ = false;
};

// ------------------------------------------------------------------------------------------------+
// Methods                                                                                         |
// ------------------------------------------------------------------------------------------------+

void Client::Run() {
    while (!stop_) {
        std::optional<int64_t> offset = GetOffset();
        std::vector<std::shared_ptr<Update>> updates = GetUpdates(offset, timeout_);
        for (std::shared_ptr<Update> update : updates) {
            Respond(update);
            SetOffset(update->update_id + 1);
        }
    }
}

void Client::Stop() {
    stop_ = true;
}

void Client::Respond(std::shared_ptr<Update> update) {
    std::shared_ptr<Response> response = bot_->ProcessUpdate(update);
    switch (response->type) {
        case ResponseType::Message:
            SendMessage(std::static_pointer_cast<ResponseMessage>(response));
            break;
        case ResponseType::Stop:
            Stop();
            break;
        default:
            break;
    }
}

// File operations
// -------------------------------------------------------------------------------------------------

std::optional<int64_t> Client::GetOffset() {
    std::ifstream offset_file(offset_file_path_);
    if (!offset_file.is_open()) {
        throw ClientError("offset file hasn't been opened");
    }

    if (offset_file.peek() == std::fstream::traits_type::eof()) {
        return std::nullopt;
    }

    int64_t offset;
    offset_file >> offset;
    offset_file.close();

    return offset;
}

void Client::SetOffset(int64_t new_offset) {
    std::ofstream offset_file(offset_file_path_);
    if (!offset_file.is_open()) {
        throw ClientError("offset file hasn't been opened");
    }

    offset_file << new_offset;
    offset_file.close();
};

// ------------------------------------------------------------------------------------------------+
// API Methods                                                                                     |
// ------------------------------------------------------------------------------------------------+

User Client::GetMe() const {
    std::string api_endpoint = url_ + "/getMe";

    Poco::URI uri(api_endpoint);
    Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPathAndQuery());

    session.sendRequest(request);
    Poco::Net::HTTPResponse response;
    std::istream& body = session.receiveResponse(response);

    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
        throw TelegramAPIError(response.getStatus(),
                               response.getReasonForStatus(response.getStatus()));
    }

    Poco::JSON::Parser parser;
    Poco::Dynamic::Var reply = parser.parse(body);

    Poco::JSON::Object::Ptr user_json =
        reply.extract<Poco::JSON::Object::Ptr>()->getObject("result");

    return User(user_json);
}

// -------------------------------------------------------------------------------------------------

std::vector<std::shared_ptr<Update>> Client::GetUpdates(std::optional<int64_t> offset,
                                                        std::optional<int32_t> timeout) const {
    std::string api_endpoint = url_ + "/getUpdates";

    Poco::URI uri(api_endpoint);
    if (offset.has_value()) {
        uri.addQueryParameter("offset", Poco::format("%ld", *offset));
    }
    if (timeout.has_value()) {
        uri.addQueryParameter("timeout", Poco::format("%d", *timeout));
    }
    Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPathAndQuery());

    session.sendRequest(request);
    Poco::Net::HTTPResponse response;
    std::istream& body = session.receiveResponse(response);

    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
        throw TelegramAPIError(response.getStatus(),
                               response.getReasonForStatus(response.getStatus()));
    }

    Poco::JSON::Parser parser;
    Poco::Dynamic::Var reply = parser.parse(body);

    Poco::JSON::Array::Ptr updates_arr =
        reply.extract<Poco::JSON::Object::Ptr>()->getArray("result");

    std::vector<std::shared_ptr<Update>> updates;
    for (const Poco::Dynamic::Var& update : *updates_arr) {
        Poco::JSON::Object::Ptr update_json = update.extract<Poco::JSON::Object::Ptr>();
        updates.push_back(MakeUpdate(update_json));
    }

    return updates;
}

// -------------------------------------------------------------------------------------------------

void Client::SendMessage(std::shared_ptr<ResponseMessage> message) const {
    std::string api_endpoint = url_ + "/sendMessage";
    Poco::URI uri(api_endpoint);

    Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
    Poco::Net::HTTPRequest request{Poco::Net::HTTPRequest::HTTP_POST, uri.getPathAndQuery(),
                                   Poco::Net::HTTPMessage::HTTP_1_1};
    Poco::JSON::Object::Ptr body = new Poco::JSON::Object();
    body->set("chat_id", message->chat_id);
    body->set("text", message->text);
    if (message->reply_to_message_id.has_value()) {
        body->set("reply_to_message_id", *message->reply_to_message_id);
    }
    std::ostringstream oss;
    body->stringify(oss, 2);
    request.setContentLength(oss.str().length());
    request.setContentType("application/json");

    session.sendRequest(request) << oss.str();

    Poco::Net::HTTPResponse response;
    session.receiveResponse(response);

    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
        throw TelegramAPIError(response.getStatus(),
                               response.getReasonForStatus(response.getStatus()));
    }
}
