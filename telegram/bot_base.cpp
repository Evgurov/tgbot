#include "bot_base.h"

// GetToken
// -------------------------------------------------------------------------------------------------

std::string BotBase::GetToken() const {
    return token_;
}

// Process Update
// -------------------------------------------------------------------------------------------------

std::shared_ptr<Response> BotBase::ProcessUpdate(const std::shared_ptr<Update> update) {
    switch (update->update_type) {
        case UpdateType::Message:
            return ProcessMessage(std::static_pointer_cast<UpdateMessage>(update));
        case UpdateType::MyChatMember:
            return ProcessMyChatMember(std::static_pointer_cast<UpdateMyChatMember>(update));
        default:
            return ProcessUnknownUpdate(std::static_pointer_cast<UpdateUnknown>(update));
    }
};

// Messages
// -------------------------------------------------------------------------------------------------

std::shared_ptr<Response> BotBase::ProcessMessage(const std::shared_ptr<UpdateMessage> message) {
    if (message->text.has_value()) {
        return ProcessTextMessage(message);
    }

    if (message->new_chat_member.has_value()) {
        return ProcessNewChatMemberMessage(message);
    }

    if (message->left_chat_member.has_value()) {
        return ProcessLeftChatMemberMessage(message);
    }

    if (message->group_chat_created.has_value()) {
        return ProcessChatCreatedMessage(message);
    }

    return std::make_shared<NoneResponse>(message->update_id);
}

std::shared_ptr<Response> BotBase::ProcessTextMessage(
    const std::shared_ptr<UpdateMessage> text_message) {

    return std::make_shared<NoneResponse>(text_message->update_id);
}

std::shared_ptr<Response> BotBase::ProcessChatCreatedMessage(
    const std::shared_ptr<UpdateMessage> chat_created_message) {

    return std::make_shared<NoneResponse>(chat_created_message->update_id);
}
std::shared_ptr<Response> BotBase::ProcessNewChatMemberMessage(
    const std::shared_ptr<UpdateMessage> new_chat_member_message) {

    return std::make_shared<NoneResponse>(new_chat_member_message->update_id);
}
std::shared_ptr<Response> BotBase::ProcessLeftChatMemberMessage(
    const std::shared_ptr<UpdateMessage> left_chat_member_message) {

    return std::make_shared<NoneResponse>(left_chat_member_message->update_id);
}

// Chat member status
// -------------------------------------------------------------------------------------------------

std::shared_ptr<Response> BotBase::ProcessMyChatMember(
    const std::shared_ptr<UpdateMyChatMember> new_chat_member) {

    return std::make_shared<NoneResponse>(new_chat_member->update_id);
}

// Unknown update
// -------------------------------------------------------------------------------------------------

std::shared_ptr<Response> BotBase::ProcessUnknownUpdate(
    const std::shared_ptr<UpdateUnknown> unknown_update) {

    return std::make_shared<NoneResponse>(unknown_update->update_id);
}
