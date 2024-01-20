#pragma once

#include "bot_base.h"

class TestBot : public BotBase {
public:
    TestBot() = delete;
    TestBot(const std::string& token) : BotBase(token){};

    std::shared_ptr<Response> ProcessTextMessage(
        std::shared_ptr<UpdateMessage> text_message) override {

        if (text_message->chat.type == "private") {
            if (text_message->message_id == 1) {
                return std::make_shared<ResponseMessage>(text_message->update_id,
                                                         text_message->chat.id, "Hi!");
            }
            if (text_message->message_id == 2) {
                return std::make_shared<ResponseMessage>(text_message->update_id,
                                                         text_message->chat.id, "Reply",
                                                         text_message->message_id);
            }
        }

        return std::make_shared<NoneResponse>(text_message->update_id);
    }
};
