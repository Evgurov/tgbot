#pragma once

#include "utils.h"

class BotBase {
public:
    BotBase() = delete;
    BotBase(const std::string& token) : token_(token){};

    virtual ~BotBase() = default;

    std::string GetToken() const;

    virtual std::shared_ptr<Response> ProcessUpdate(const std::shared_ptr<Update> update);

    // Messages
    // ---------------------------------------------------------------------------------------------

    virtual std::shared_ptr<Response> ProcessMessage(std::shared_ptr<UpdateMessage> message);
    virtual std::shared_ptr<Response> ProcessTextMessage(
        std::shared_ptr<UpdateMessage> text_message);
    virtual std::shared_ptr<Response> ProcessChatCreatedMessage(
        std::shared_ptr<UpdateMessage> chat_created_message);
    virtual std::shared_ptr<Response> ProcessNewChatMemberMessage(
        std::shared_ptr<UpdateMessage> new_chat_member_message);
    virtual std::shared_ptr<Response> ProcessLeftChatMemberMessage(
        std::shared_ptr<UpdateMessage> left_chat_member_message);

    // Chat member status change
    // ---------------------------------------------------------------------------------------------

    virtual std::shared_ptr<Response> ProcessMyChatMember(
        std::shared_ptr<UpdateMyChatMember> new_chat_member);

    // Unknown update
    // ---------------------------------------------------------------------------------------------

    virtual std::shared_ptr<Response> ProcessUnknownUpdate(
        std::shared_ptr<UpdateUnknown> unknown_update);

private:
    const std::string token_;
};
