#pragma once

#include "bot_base.h"

class TelegramBotTest : public BotBase {
public:
    TelegramBotTest() = delete;
    TelegramBotTest(const std::string& token) : BotBase(token){};

    std::shared_ptr<Response> ProcessTextMessage(std::shared_ptr<UpdateMessage> text_message) {
        std::string response_text;
        if (text_message->text == "/привет") {
            if (text_message->from->username == "xypu04") {
                response_text = "Привет, Пусенька!";
            } else if (text_message->from->username == "evgurovv") {
                response_text = "Опа нихуя. Здарова Женёк!";
            } else {
                response_text = "Привет я Женин бот";
            }
        } else if (text_message->text == "/оскорбление") {
            response_text = "Пашел нахуй чертила";
        } else if (text_message->text == "/заебал") {
            response_text = "Сам заебал";
        } else if (text_message->text == "/стоп") {
            return std::make_shared<StopResponse>(text_message->update_id);
        } else {
            response_text = "Другалёк, такой команды нет";
        }

        return std::make_shared<ResponseMessage>(text_message->update_id, text_message->chat.id,
                                                 response_text);
    }

    std::shared_ptr<Response> ProcessNewChatMemberMessage(
        std::shared_ptr<UpdateMessage> new_chat_member_message) {

        std::string response_text;
        response_text = "Приветствуем @" + *new_chat_member_message->new_chat_member->username;

        return std::make_shared<ResponseMessage>(new_chat_member_message->update_id,
                                                 new_chat_member_message->chat.id, response_text);
    }
};