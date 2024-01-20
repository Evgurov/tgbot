#include "client.h"
#include "telegram_bot_test.h"

int main() {
    std::string base_url = "https://api.telegram.org/";
    std::string bot_token = "6887905160:AAFxp6NHdbWnneAxCk68R1A_kOgtgZNNZJQ";
    std::string offset_file_path = "/home/evgurov/progs/shad/cpp/evgurov/bot/telegram/offset.txt";
    int32_t timeout = 5;

    Client client(base_url, std::make_unique<TelegramBotTest>(bot_token), offset_file_path,
                  timeout);

    client.Run();
}
