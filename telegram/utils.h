#pragma once

#include <vector>
#include <string>
#include <optional>
#include <memory>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Array.h>

// ------------------------------------------------------------------------------------------------+
// Other entities                                                                                  |
// ------------------------------------------------------------------------------------------------+

struct Chat {
    Chat() = default;
    Chat(Poco::JSON::Object::Ptr chat_json);

    int64_t id;
    std::string type;
};

struct User {
    User() = default;
    User(Poco::JSON::Object::Ptr user_json);

    int64_t id;
    bool is_bot;
    std::string first_name;

    std::optional<std::string> last_name;
    std::optional<std::string> username;
};

struct ChatMember {
    ChatMember() = default;
    ChatMember(Poco::JSON::Object::Ptr chat_member_json);

    std::string status;
    User user;
};

struct ChatMemberUpdated {
    ChatMemberUpdated() = default;
    ChatMemberUpdated(Poco::JSON::Object::Ptr chat_member_updated_json);

    Chat chat;
    User from;
    int64_t date;
    ChatMember old_chat_member;
    ChatMember new_chat_member;
};

struct MessageEntity {
    MessageEntity() = default;
    MessageEntity(Poco::JSON::Object::Ptr entity_json);

    std::string type;
    int32_t offset;
    size_t length;
};

// ------------------------------------------------------------------------------------------------+
// Updates                                                                                         |
// ------------------------------------------------------------------------------------------------+

enum class UpdateType { Message, MyChatMember, Unknown };

struct Update {
    Update() = delete;
    Update(int64_t id, UpdateType type) : update_id(id), update_type(type){};

    virtual ~Update() = default;

    int64_t update_id;
    UpdateType update_type;
};

struct UpdateUnknown : Update {
    UpdateUnknown() = delete;
    UpdateUnknown(int64_t update_id) : Update(update_id, UpdateType::Unknown){};
};

struct UpdateMessage : Update {
    UpdateMessage() = delete;
    UpdateMessage(int64_t update_id, Poco::JSON::Object::Ptr message_json);

    int64_t message_id;
    int32_t date;
    Chat chat;

    std::optional<std::vector<MessageEntity>> entities;
    std::optional<std::string> text;
    std::optional<User> from;
    std::optional<bool> group_chat_created;
    std::optional<User> new_chat_member;
    std::optional<User> left_chat_member;
};

struct UpdateMyChatMember : Update {
    UpdateMyChatMember() = delete;
    UpdateMyChatMember(int64_t update_id, Poco::JSON::Object::Ptr chat_member_updated_json);

    Chat chat;
    User from;
    int64_t date;
    ChatMember old_chat_member;
    ChatMember new_chat_member;
};

std::shared_ptr<Update> MakeUpdate(Poco::JSON::Object::Ptr update_json);

// ------------------------------------------------------------------------------------------------+
// Responses                                                                                       |
// ------------------------------------------------------------------------------------------------+

enum class ResponseType { None, Message, Stop };

struct Response {
    Response() = default;
    Response(ResponseType type, int64_t update_id) : type(type), update_id(update_id){};
    virtual ~Response() = default;

    ResponseType type;
    int64_t update_id;
};

struct NoneResponse : Response {
    NoneResponse() = default;
    NoneResponse(int64_t update_id) : Response(ResponseType::None, update_id){};
};

struct StopResponse : Response {
    StopResponse() = default;
    StopResponse(int64_t update_id) : Response(ResponseType::Stop, update_id){};
};

struct ResponseMessage : Response {
    ResponseMessage() = default;
    ResponseMessage(int64_t update_id, int64_t chat_id, std::string_view text,
                    std::optional<int64_t> reply_to_message_id = std::nullopt)
        : Response(ResponseType::Message, update_id),
          chat_id(chat_id),
          text(text),
          reply_to_message_id(reply_to_message_id){};

    int64_t chat_id;
    std::string text;

    std::optional<int64_t> reply_to_message_id;
};
