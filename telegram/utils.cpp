#include "utils.h"

// ------------------------------------------------------------------------------------------------+
// Other entities                                                                                  |
// ------------------------------------------------------------------------------------------------+

// Chat
// -------------------------------------------------------------------------------------------------

Chat::Chat(Poco::JSON::Object::Ptr chat_json) {
    id = chat_json->getValue<int64_t>("id");
    type = chat_json->getValue<std::string>("type");
}

// User
// -------------------------------------------------------------------------------------------------

User::User(Poco::JSON::Object::Ptr user_json) {
    id = user_json->getValue<int64_t>("id");
    is_bot = user_json->getValue<bool>("is_bot");
    first_name = user_json->getValue<std::string>("first_name");
    if (user_json->has("last_name")) {
        last_name = user_json->getValue<std::string>("last_name");
    }
    if (user_json->has("username")) {
        username = user_json->getValue<std::string>("username");
    }
}

// Chat Member
// -------------------------------------------------------------------------------------------------

ChatMember::ChatMember(Poco::JSON::Object::Ptr chat_member_json) {
    status = chat_member_json->getValue<std::string>("status");
    Poco::JSON::Object::Ptr user_json = chat_member_json->getObject("user");
    user = User(user_json);
}

// Chat Member Updated
// -------------------------------------------------------------------------------------------------

ChatMemberUpdated::ChatMemberUpdated(Poco::JSON::Object::Ptr chat_member_updated_json) {
    date = chat_member_updated_json->getValue<int64_t>("date");
    Poco::JSON::Object::Ptr chat_json = chat_member_updated_json->getObject("chat");
    chat = Chat(chat_json);
    Poco::JSON::Object::Ptr from_json = chat_member_updated_json->getObject("from");
    from = User(from_json);
    Poco::JSON::Object::Ptr new_chat_member_json =
        chat_member_updated_json->getObject("new_chat_member");
    new_chat_member = ChatMember(new_chat_member_json);
    Poco::JSON::Object::Ptr old_chat_member_json =
        chat_member_updated_json->getObject("old_chat_member");
    old_chat_member = ChatMember(old_chat_member_json);
}

// Message Entity
// -------------------------------------------------------------------------------------------------

MessageEntity::MessageEntity(Poco::JSON::Object::Ptr entity_json) {
    type = entity_json->getValue<std::string>("type");
    offset = entity_json->getValue<int32_t>("offset");
    length = entity_json->getValue<int32_t>("length");
}

// ------------------------------------------------------------------------------------------------+
// Updates                                                                                         |
// ------------------------------------------------------------------------------------------------+

// UpdateMessage
// -------------------------------------------------------------------------------------------------

UpdateMessage::UpdateMessage(int64_t update_id, Poco::JSON::Object::Ptr message_json)
    : Update(update_id, UpdateType::Message) {
    message_id = message_json->getValue<int64_t>("message_id");
    date = message_json->getValue<int64_t>("date");
    Poco::JSON::Object::Ptr chat_json = message_json->getObject("chat");
    chat = Chat(chat_json);
    if (message_json->has("from")) {
        Poco::JSON::Object::Ptr from_json = message_json->getObject("from");
        from = User(from_json);
    }
    if (message_json->has("text")) {
        text = message_json->getValue<std::string>("text");
    }
    if (message_json->has("entities")) {
        entities = std::vector<MessageEntity>();
        Poco::JSON::Array::Ptr entities_json = message_json->getArray("entities");
        for (const Poco::Dynamic::Var& entity : *entities_json) {
            Poco::JSON::Object::Ptr entity_json = entity.extract<Poco::JSON::Object::Ptr>();
            entities->push_back(MessageEntity(entity_json));
        }
    }
    if (message_json->has("group_chat_created")) {
        group_chat_created = message_json->getValue<bool>("group_chat_created");
    }
    if (message_json->has("new_chat_member")) {
        Poco::JSON::Object::Ptr new_chat_member_json = message_json->getObject("new_chat_member");
        new_chat_member = User(new_chat_member_json);
    }
    if (message_json->has("left_chat_member")) {
        Poco::JSON::Object::Ptr left_chat_member_json = message_json->getObject("left_chat_member");
        left_chat_member = User(left_chat_member_json);
    }
}

// Update My Chat Member
// -------------------------------------------------------------------------------------------------

UpdateMyChatMember::UpdateMyChatMember(int64_t update_id,
                                       Poco::JSON::Object::Ptr chat_member_updated_json)
    : Update(update_id, UpdateType::MyChatMember) {

    Poco::JSON::Object::Ptr chat_json = chat_member_updated_json->getObject("chat");
    chat = Chat(chat_json);
    Poco::JSON::Object::Ptr from_json = chat_member_updated_json->getObject("from");
    from = User(from_json);
    date = chat_member_updated_json->getValue<int64_t>("date");
    Poco::JSON::Object::Ptr old_chat_member_json =
        chat_member_updated_json->getObject("old_chat_member");
    old_chat_member = ChatMember(old_chat_member_json);
    Poco::JSON::Object::Ptr new_chat_member_json =
        chat_member_updated_json->getObject("new_chat_member");
    new_chat_member = ChatMember(new_chat_member_json);
}

// Make Update Function
// -------------------------------------------------------------------------------------------------

std::shared_ptr<Update> MakeUpdate(Poco::JSON::Object::Ptr update_json) {
    int64_t update_id = update_json->getValue<int64_t>("update_id");
    if (update_json->has("message")) {
        Poco::JSON::Object::Ptr message_json = update_json->getObject("message");
        return std::make_shared<UpdateMessage>(update_id, message_json);
    }

    if (update_json->has("my_chat_member")) {
        Poco::JSON::Object::Ptr chat_member_updated_json = update_json->getObject("my_chat_member");
        return std::make_shared<UpdateMyChatMember>(update_id, chat_member_updated_json);
    }

    return std::make_shared<UpdateUnknown>(update_id);
}
