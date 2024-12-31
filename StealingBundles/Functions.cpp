#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include <string>
#include <stack>
#include <chrono>
#include <set>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include "crow.h"
using namespace std;

enum Rank
{
    ACE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING
};

enum Suit
{
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES
};

enum Color
{
    RED,
    BLACK
};

struct Card {
    Rank rank;
    Suit suit;

    Card(Rank r, Suit s) : rank(r), suit(s) {}

    int getIndex() {
        return suit * 13 + rank;
    }
};

class Player {
public:
    int id;
    vector<Card> hand;
    stack<Card> bundle;
    bool isAgent;

    explicit Player(int playerId, bool isAgent) : id(playerId), isAgent(isAgent) {}
    virtual ~Player() {}
};

class GameState {
public: 
    int stockSize;
    vector<int> handSizes;
    vector<Card> hand;
    vector<Card> pool;
    vector<stack<Card>> bundle;
    bool gameOver;

    GameState(
        int stockSize,
        const vector<int>& handSizes,
        const vector<Card>& hand,
        const vector<Card>& pool,
        const vector<stack<Card>>& bundle,
        bool gameOver
    ) : stockSize(stockSize),
        handSizes(handSizes),
        hand(hand),
        pool(pool),
        bundle(bundle),
        gameOver(gameOver) {
    }

    crow::json::wvalue jsonifyState() {
        crow::json::wvalue state;
        cout << "Over: " << gameOver << endl;
        state["stockSize"] = stockSize;
        state["handSizes"] = handSizes;
        vector<int> handIndices;
        for (Card& card : hand) {
            handIndices.push_back(card.getIndex());
        }
        state["hand"] = handIndices;
        vector<int> poolIndices;
        for (Card& card : pool) {
            poolIndices.push_back(card.getIndex());
        }
        state["pool"] = poolIndices;
        std::vector<std::vector<int>> bundleIndices;
        for (const std::stack<Card>& cardStack : bundle) {
            std::vector<int> stackIndices;
            std::stack<Card> copyStack = cardStack; 
            while (!copyStack.empty()) {
                stackIndices.push_back(copyStack.top().getIndex());
                copyStack.pop();
            }
            bundleIndices.push_back(stackIndices);
        }
        state["bundle"] = bundleIndices;
        cout << "Over: " << gameOver << endl;
        if (gameOver) {
            state["gameOver"] = true;
        }
        else {
            state["gameOver"] = false;
        }

        return state;
    }
};

class Game {
public:
    stack<Card> stock;
    vector<Card> pool;
    vector<Player> players;
    map<int, int> scores;
    vector<int> agentDifficultyLevel;
    int lastMatchPlayer = -1;

    void shuffleDeck() {
        vector<Card> tempDeck;

        while (!stock.empty()) {
            tempDeck.push_back(stock.top());
            stock.pop();
        }

        random_device rd;
        mt19937 g(static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count()));
        shuffle(tempDeck.begin(), tempDeck.end(), g);

        for (auto it = tempDeck.rbegin(); it != tempDeck.rend(); ++it) {
            stock.push(*it);
        }
    }

    void dealToPlayer(Player& player, int count) {
        for (int i = 0; i < count && !stock.empty(); i++) {
            player.hand.push_back(stock.top());
            stock.pop();
        }
    }

    void dealToPool(int count) {
        for (int i = 0; i < count && !stock.empty(); i++) {
            pool.push_back(stock.top());
            stock.pop();
        }
    }

    bool playCard(Player& player, int cardIndex) {
        if (cardIndex < 0 || cardIndex >= player.hand.size()) {
            return false;
        }

        Card selectedCard = player.hand[cardIndex];
        player.hand.erase(player.hand.begin() + cardIndex);
        if (checkMatch(selectedCard, player)) {
            player.bundle.push(selectedCard);
            lastMatchPlayer = player.id;
        }
        else {
            pool.push_back(selectedCard);
        }
        return true;
    }

    bool checkMatch(Card& trickCard, Player& currentPlayer) {
        bool matchFound = false;
        for (Player& player : players) {
            if (player.id != currentPlayer.id && !player.bundle.empty()) {
                Card topBundleCard = player.bundle.top();
                if (topBundleCard.rank == trickCard.rank) {
                    stack<Card> tempBundle;
                    while (!player.bundle.empty()) {
                        tempBundle.push(player.bundle.top());
                        player.bundle.pop();
                    }
                    while (!tempBundle.empty()) {
                        currentPlayer.bundle.push(tempBundle.top());
                        tempBundle.pop();
                    }
                    matchFound = true;
                }
            }
        }

        auto it = remove_if(pool.begin(), pool.end(),
            [&trickCard, &currentPlayer, &matchFound](Card& poolCard) {
                if (poolCard.rank == trickCard.rank) {
                    currentPlayer.bundle.push(poolCard);
                    matchFound = true;
                    return true;
                }
                return false;
            });
        pool.erase(it, pool.end());

        return matchFound;
    }

    void addRemainingPoolCardsToBundle() {
        if (lastMatchPlayer != -1 && stock.empty()) {
            Player& lastPlayer = players[lastMatchPlayer];
            for (const Card& card : pool) {
                lastPlayer.bundle.push(card);
            }
            pool.clear();
        }
    }

    GameState getGameState(Player thisPlayer) {
        vector<int> handSizes; vector<stack<Card>> bundles;
        for (Player& player : players) {
            handSizes.push_back(player.hand.size());
            bundles.push_back(player.bundle);
        }
        bool gameOver = isGameOver();
        return GameState(stock.size(), handSizes, thisPlayer.hand, pool, bundles, gameOver);
    }

public:
    Game(int numAgents, vector<int> difficultyLevel) : agentDifficultyLevel(difficultyLevel) {
        for (Suit suit : {HEARTS, DIAMONDS, CLUBS, SPADES}) {
            for (Rank rank = ACE; rank <= KING; rank = static_cast<Rank>(rank + 1)) {
                stock.push(Card(rank, suit));
            }
        }

        players.emplace_back(Player(0, false));
        for (int i = 1; i <= numAgents; i++) {
            players.emplace_back(i, true);
        }
    }

    void initialDeal() {
        shuffleDeck();
        dealToPool(4);
        for (Player& player : players) {
            dealToPlayer(player, 4);
        }
    }

    int checkForNewCards() {
        for (Player& player : players) {
            if (player.hand.empty() && !stock.empty()) {
                dealToPlayer(player, 4);
            }
        }
		return 1;
    }

    int checkPool() {
        if (pool.empty() && !stock.empty()) {
            dealToPool(4);
        }
        return 1;
    }

    bool isGameOver() {
        return stock.empty() && all_of(players.begin(), players.end(), [](const Player& player) { return player.hand.empty(); });
    }

    void finishing() {
        addRemainingPoolCardsToBundle();
        for (Player& player : players) {
            scores[player.id] = player.bundle.size();
        }

        vector<pair<int, int>> sortedScores(scores.begin(), scores.end());
        sort(sortedScores.rbegin(), sortedScores.rend(),
            [](const pair<int, int>& a, const pair<int, int>& b) {
                return a.second < b.second;
            });

        cout << "Game Over! Scores:" << endl;
        for (const auto& scorePair : sortedScores) {
            cout << "Player " << scorePair.first << ": " << scorePair.second << endl;
        }
        cout << "Winner: Player " << sortedScores.begin()->first << endl;
    }

    class SimpleAgent {
        int playerId;
    public:
        SimpleAgent(int id) : playerId(id) {}

        int chooseMove(const vector<Card>& hand, const vector<Player>& players, const vector<Card>& pool, int playercurrentID) {
            int handSize = hand.size();
            vector<int> count(handSize, 0);
            for (int i = 0; i < handSize; i++) {
                for (const Player& player : players) {
                    if (player.id != playercurrentID && !player.bundle.empty()) {
                        if (hand[i].rank == player.bundle.top().rank) {
                            count[i]++;
                        }
                    }
                }
                for (const Card& poolCard : pool) {
                    if (hand[i].rank == poolCard.rank)
                        count[i]++;
                }
            }
            return distance(count.begin(), max_element(count.begin(), count.end()));
        }

        int getAIMove(Player& player, const vector<Player>& players, const vector<Card>& pool) {
            return chooseMove(player.hand, players, pool, player.id);
        }
    };

    class AdvancedAgent {
    public:
        int playerId;

        explicit AdvancedAgent(int id) : playerId(id) {}

        int getAIMove(Player& player, vector<Player>& players, vector<Card>& pool) {
            int bestMove = -1;
            int maxScore = -1;

            for (int i = 0; i < player.hand.size(); ++i) {
                Card potentialCard = player.hand[i];
                int score = 0;

                for (const Player& otherPlayer : players) {
                    if (otherPlayer.id != player.id && !otherPlayer.bundle.empty()) {
                        if (potentialCard.rank == otherPlayer.bundle.top().rank) {
                            score += otherPlayer.bundle.size();
                        }
                        for (const Card& poolCard : pool) {
                            if (poolCard.rank == otherPlayer.bundle.top().rank && poolCard.rank == potentialCard.rank) {
                                score += 3;
                            }
                        }
                    }
                }

                for (const Card& poolCard : pool) {
                    if (potentialCard.rank == poolCard.rank) {
                        score++;
                    }
                }

                if (score > maxScore) {
                    maxScore = score;
                    bestMove = i;
                }
            }

            if (bestMove == -1) {
                return rand() % player.hand.size();
            }

            return bestMove;
        }
    };

    class MinimaxAgent {
        int playerId;

    public:
        explicit MinimaxAgent(int id) : playerId(id) {}

        int getAIMove(const vector<Card>& hand, const vector<Player>& players,
            const vector<Card>& pool, stack<Card>& stock,
            int currentPlayerID, int depth) {
            int bestMoveIndex = -1;
            int bestScore = -INT_MAX;

            for (int i = 0; i < hand.size(); i++) {
                vector<Player> simulatedPlayers = players;
                vector<Card> simulatedPool = pool;
                stack<Card> simulatedStock = stock;

                Card playedCard = hand[i];
                simulatedPool.push_back(playedCard);
                simulatedPlayers[currentPlayerID].hand.erase(
                    simulatedPlayers[currentPlayerID].hand.begin() + i);

                if (checkMatch(playedCard, simulatedPlayers[currentPlayerID], simulatedPlayers, simulatedPool)) {
                    simulatedPlayers[currentPlayerID].bundle.push(playedCard);
                }

                int score = minimax(simulatedPlayers, simulatedPool, simulatedStock,
                    (currentPlayerID + 1) % simulatedPlayers.size(),
                    depth - 1, false);

                if (score > bestScore) {
                    bestScore = score;
                    bestMoveIndex = i;
                }
            }

            return bestMoveIndex;
        }

    private:
        int minimax(vector<Player>& players, vector<Card>& pool, stack<Card>& stock,
            int currentPlayerID, int depth, bool maximizingPlayer) {
            if (depth == 0 || (stock.empty() && allHandsEmpty(players))) {
                return players[playerId].bundle.size();
            }

            if (players[currentPlayerID].hand.empty() && !stock.empty()) {
                dealToPlayer(players[currentPlayerID], stock, 4);
            }

            int bestScore = maximizingPlayer ? -INT_MAX : INT_MAX;

            for (int i = 0; i < players[currentPlayerID].hand.size(); i++) {
                vector<Player> simulatedPlayers = players;
                vector<Card> simulatedPool = pool;
                stack<Card> simulatedStock = stock;

                Card playedCard = simulatedPlayers[currentPlayerID].hand[i];
                simulatedPool.push_back(playedCard);
                simulatedPlayers[currentPlayerID].hand.erase(
                    simulatedPlayers[currentPlayerID].hand.begin() + i);

                if (checkMatch(playedCard, simulatedPlayers[currentPlayerID], simulatedPlayers, simulatedPool)) {
                    simulatedPlayers[currentPlayerID].bundle.push(playedCard);
                }

                int score = minimax(simulatedPlayers, simulatedPool, simulatedStock,
                    (currentPlayerID + 1) % simulatedPlayers.size(),
                    depth - 1, !maximizingPlayer);

                if (maximizingPlayer) {
                    bestScore = max(bestScore, score);
                }
                else {
                    bestScore = min(bestScore, score);
                }
            }

            return bestScore;
        }

        void dealToPlayer(Player& player, stack<Card>& stock, int count) {
            for (int i = 0; i < count && !stock.empty(); i++) {
                player.hand.push_back(stock.top());
                stock.pop();
            }
        }

        bool allHandsEmpty(const vector<Player>& players) {
            return all_of(players.begin(), players.end(), [](const Player& player) {
                return player.hand.empty();
                });
        }

        bool checkMatch(const Card& card, Player& currentPlayer, vector<Player>players, vector<Card>& pool) {
            bool matchFound = false;

            for (Player& player : players) {
                if (player.id != currentPlayer.id && !player.bundle.empty()) {
                    if (player.bundle.top().rank == card.rank) {
                        stack<Card> tempBundle;
                        while (!player.bundle.empty()) {
                            tempBundle.push(player.bundle.top());
                            player.bundle.pop();
                        }
                        while (!tempBundle.empty()) {
                            currentPlayer.bundle.push(tempBundle.top());
                            tempBundle.pop();
                        }
                        matchFound = true;
                    }
                }
            }

            auto it = remove_if(pool.begin(), pool.end(), [&card, &currentPlayer, &matchFound](Card& poolCard) {
                if (poolCard.rank == card.rank) {
                    currentPlayer.bundle.push(poolCard);
                    matchFound = true;
                    return true;
                }
                return false;
                });

            pool.erase(it, pool.end());
            return matchFound;
        }
    };
};