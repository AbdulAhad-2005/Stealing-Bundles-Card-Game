#include "crow.h"
#include <nlohmann/json.hpp>
#include "Functions.cpp"
#include <map>
#include <vector>
using namespace std;

class LiveGame {
public:
    static LiveGame& instance() {
        static LiveGame instance;
        return instance;
    }

    Game *game;
    bool started = false;

private:
    LiveGame() {}
};

string readFile(const std::string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        return "<h1>Error: Unable to open file</h1>";
    }

    ostringstream content;
    content << file.rdbuf();
    return content.str(); 
}

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/play") ([]() {
        auto page = crow::mustache::load("index.html");
        return page.render();
        });

    CROW_ROUTE(app, "/start-game").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        auto json_data = crow::json::load(req.body);
        if (!json_data) {
            return crow::response(400, "Invalid JSON data");
        }
        int numPlayers = json_data["numPlayers"].i();
        std::vector<int> difficulties;
        difficulties.push_back(0);
        for (int i = 2; i <= numPlayers; ++i) {
            std::string key = "difficultyPlayer" + std::to_string(i);
            if (json_data.has(key)) {
                difficulties.push_back(json_data[key].i());
            }
        }
        LiveGame::instance().game = new Game(numPlayers-1, difficulties);
        LiveGame::instance().game->initialDeal();
        return crow::response(200, "Game started successfully");
        });

    CROW_ROUTE(app, "/results") ([]() {
        auto page = crow::mustache::load("results.html");
        return page.render();
        });

    CROW_ROUTE(app, "/") ([]() {
        auto page = crow::mustache::load("menu.html");
        return page.render();
        });

    CROW_ROUTE(app, "/updateView")([]() {
        return LiveGame::instance().game->getGameState(LiveGame::instance().game->players[0]).jsonifyState();
            });

    CROW_ROUTE(app, "/reDeal")([]() {
        int result = LiveGame::instance().game->checkForNewCards();
        return crow::json::wvalue{ {"result", result} };
        });

    CROW_ROUTE(app, "/fillPool")([]() {
        int result = LiveGame::instance().game->checkPool();
        return crow::json::wvalue{ {"result", result} };
        });

    CROW_ROUTE(app, "/finish")([]() {
        LiveGame::instance().game->finishing();
        return LiveGame::instance().game->getGameState(LiveGame::instance().game->players[0]).jsonifyState(); 
        });

    CROW_ROUTE(app, "/getDifficulties")([]() {
        crow::json::wvalue data;
        cout << LiveGame::instance().game->agentDifficultyLevel.size();
        data["difficulties"] = LiveGame::instance().game->agentDifficultyLevel;
        data["playernum"] = LiveGame::instance().game->agentDifficultyLevel.size();
        return data;
        });

    CROW_ROUTE(app, "/playCard")
        .methods("POST"_method)
        ([](const crow::request& req) {
        auto json = crow::json::load(req.body);
        int id = json["id"].i();
        int card = json["card"].i();
        int diff = json["diff"].i();
        int result = 0;

        if (LiveGame::instance().game->players[id].isAgent) {
            int cardIndex;
            cout << LiveGame::instance().game->players[id].hand.size();
            if (diff == 1) {
                Game::SimpleAgent beginnerAgent(id);
                cardIndex = beginnerAgent.getAIMove(LiveGame::instance().game->players[id], LiveGame::instance().game->players, LiveGame::instance().game->pool);
            }
            else if (diff == 2) {
                Game::AdvancedAgent advancedAgent(id);
                cardIndex = advancedAgent.getAIMove(LiveGame::instance().game->players[id], LiveGame::instance().game->players, LiveGame::instance().game->pool);
            }
            else if (diff == 3) {
                Game::MinimaxAgent minimaxAgent(id);
                cardIndex = minimaxAgent.getAIMove(LiveGame::instance().game->players[id].hand, LiveGame::instance().game->players, LiveGame::instance().game->pool, LiveGame::instance().game->stock, id, 5);
            }
            cout << "Player " << id << " plays card at index " << cardIndex << endl;
            result = LiveGame::instance().game->playCard(LiveGame::instance().game->players[id], cardIndex);
            cout << "Hand Size: " << LiveGame::instance().game->players[id].hand.size() << endl;
        }
        else {
            int cardIndex = card;
            cout << "Player " << id << " plays card at index " << cardIndex << endl;
            result = LiveGame::instance().game->playCard(LiveGame::instance().game->players[id], cardIndex);
            cout << "Hand Size: " << LiveGame::instance().game->players[id].hand.size() << endl;
        }
        return crow::json::wvalue{ {"result", result} };
            });

    app.port(18080).run();

    return 0;
}