#include <tests/test_scenarios.h>

#include <catch2/catch_test_macros.hpp>
#include <string>

#include "telegram/client.h"
#include "telegram/bot_test.h"

void TestSingleGetMe(std::string_view url) {
    Client client(std::string(url), std::make_unique<TestBot>("123"),
                  "/home/evgurov/progs/shad/cpp/evgurov/bot/telegram/offset.txt");

    User user = client.GetMe();
    REQUIRE(user.id == 1234567);
    REQUIRE(user.username == "test_bot");
    REQUIRE(user.first_name == "Test Bot");
    REQUIRE(user.is_bot);
}

void TestGetMeErrorHandling(std::string_view url) {
    Client client(std::string(url), std::make_unique<TestBot>("123"),
                  "/home/evgurov/progs/shad/cpp/evgurov/bot/telegram/offset.txt");

    User user;
    try {
        user = client.GetMe();
    } catch (const TelegramAPIError& exception) {
        REQUIRE(exception.http_code == Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        REQUIRE(exception.reason == "Internal Server Error");
    }

    try {
        user = client.GetMe();
    } catch (const TelegramAPIError& exception) {
        REQUIRE(exception.http_code == Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED);
        REQUIRE(exception.reason == "Unauthorized");
    }
}

void TestSingleGetUpdatesAndSendMessages(std::string_view url) {
    Client client(std::string(url), std::make_unique<TestBot>("123"),
                  "/home/evgurov/progs/shad/cpp/evgurov/bot/telegram/offset.txt");

    std::vector<std::shared_ptr<Update>> updates = client.GetUpdates();
    for (auto update : updates) {
        client.Respond(update);
    }
}

void TestHandleGetUpdatesOffset(std::string_view url) {
    Client client(std::string(url), std::make_unique<TestBot>("123"),
                  "/home/evgurov/progs/shad/cpp/evgurov/bot/telegram/offset.txt", 5);

    std::vector<std::shared_ptr<Update>> updates = client.GetUpdates(client.GetOffset(), 5);
    REQUIRE(updates.size() == 2);

    int64_t max_update_id = std::numeric_limits<int64_t>::min();
    for (auto update : updates) {
        if (update->update_id > max_update_id) {
            max_update_id = update->update_id;
        }
    }
    client.SetOffset(max_update_id + 1);

    updates = client.GetUpdates(client.GetOffset(), 5);
    REQUIRE(updates.empty());

    updates = client.GetUpdates(client.GetOffset(), 5);
    REQUIRE(updates.size() == 1);
}
