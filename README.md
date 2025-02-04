
# Stealing Bundles Card Game

Stealing Bundles is not a very popular card game, also known as "Daketi" in India/Pakistan.The game i have built allows only one player play against multiple agents(1-3). It is not a multiplayer game. The game rules are as follows:

## Rules
#### Ages: any age
#### Number of players: 2-4
#### Materials: Deck of cards
#### Skill Practice: Number recognition, matching

#### Goal: To gather the largest bundle of cards

### Set up:

Dealer deals 4 cards to each player. This becomes the players’ hand.
Next, the dealer deals 4 cards face up in the center of the table.  The cards need to be in a row and visible to each player.
The remaining cards are placed in a pile, face down, near the dealer to use later.
### Rule for play:

1. The player on the left of the dealer begins the play. Play continues clockwise.
2. At each player’s turn, the player must play 1 card from their hand.
3. If the card played matches the one of the face up cards, the player takes the cards. The cards are placed in front of the player. This begins the player’s bundle. The last card the player played must be on top.
4. If a player plays a card that does not match on a pile, the card stays for the next player’s turn.
5. If a player plays a card that matches the top card of an opponent’s bundle, the player steals that bundle.  The bundle is placed on top of the stealing player’s bundle.
6. After the players use up their hand of 4 cards, the dealer deals 4 more cards to each player and the play continues. The card layout remains the same.
7. Play continues as described above.
8. After all the cards have been dealt and the last match made, the remaining cards on the table are added the bundle of the player who made the last match.
9. The winner of the game is the player with the largest bundle or stack of cards.
    
# Getting Started
**Clone the Repository**
   ```sh
   git clone https://github.com/AbdulAhad-2005/Stealing-Bundles-Card-Game.git
   cd Voting-System

## Requirements (to run code)
1. visual studio installed with C++ build tools.
2. install VCPKG package manager (if not already installed) by following these steps

Step 1: Clone the vcpkg Repository
Open your terminal or command prompt.
Clone the vcpkg repository from GitHub:
```bash
git clone https://github.com/microsoft/vcpkg.git
```
Step 2: Build the vcpkg Executable
Navigate to the vcpkg directory:
```bash
cd vcpkg
```
Build the vcpkg executable:
```bash
.\bootstrap-vcpkg.bat
```
Step 3: Integrate vcpkg with your project
```bash
.\vcpkg integrate install
```
This will make vcpkg work seamlessly with your IDE (in our case viusal studio).

Note: You can install vcpkg for vs code, but with visual studio, it will be easier as you will just have download libraries from vcpkg and visual studio will automatically link them to your code due to build in tools.
But if you use vs code, you will have to make your own cmake file for crow and associate it with mingw compiler, which is a hassle.

3. Install crow library using vcpkg
First of all, go to the directory in which vcpkg is downloaded and write
``` bash
cd vcpkg
```
You can search for crow using command
``` bash
.\vcpkg search crow
```
It will be listed, you can install it using
```bash
.\vcpkg install crow
```
It will install crow and all other libraries required for crow to work(like cmake, asio and boost-asio) if not already installed

You can check if crow is installed using 
``` bash
.\vcpkg list
```
4. You need to install another library "nlohmann/json" for json work using command
``` bash
.\vcpkg install nlohmann-json
```
5. After everything set up, you should run the program in visual studio.

If it runs successfully without any errors, the command prompt will appear telling crow app is running at http://0.0.0.0:18080. 

You should open google or any other browser you like and type "localhost:18080" and click enter, the menu box will appear, prompting you to select number of players. Now, enjoy playing the game.
 
## How to play
1. First, select the number of players(computer agents) you want to play against.
2. Then you can select agents with varying difficulty levels with 1 being the easy and 3 being the hard agent. Well, there is not much of a difference in difficulty level of agents as it is a luck based game.
3. The game will start by pressing play button. The pool cards will be in center faced up and if not, refersh the page. The cards to deal will be in pool's area faced down in a stack. You can see only your hand cards and not of other players.
4. Click a card of your hand to play. As the game continues and you find a match in pool, it will start your bundle. Your can see the top most card of every player's bundle is any.
5. The game will continue until there is a winner. The winner will be displayed at the end.

Better luck playing.

## About code
### Functions.cpp
The main game logic and agents implementation is in this file.
#### Simple Agent(Agent 1)
simply chooses that card which has highest matches and if not choose randomly out of hand.
#### Advanced Agent(Agent 2)
assign heuristics to every card in hand based on many factors like bundle match or pool match.
#### Minimax Agent(Agent 3)
applies minimax search algorithm for every card in hand, that plays the game in advance up to a certain depth (in our case, depth is 5) and choose out of all the possible moves for next 5 rounds for a hand's card and ultimately chooses the card in hand, that would increase the score of agent for long term.

### StealingBundles.cpp
It is the main code connecting frontend and backend. It runs the main functions in functions.cpp for the game to run and transfers the data between main.js and functions.cpp

### main.js
implements all the functionality of frontend like adding and removing cards out of hands pool and bundles, and also transferring data to backend and updating data on the frontend.

### HTML and CSS files
1. menu.html: for menu(starting page).
2. index.html: for whole playing part of game.
3. results.html: for the result page.
4. main.css: for styling of game.

## Improvements
-> if you want to implement multiplayer, where players can play against each other instead of playing against computer, you can add.

-> you can improve the game's beauty and smoothness, like the result page and allowing user to drag the card to another card (if there is a match) or to pool(if there's not)

-> if you want to enhance the game for large scale like hosting on the web, you can add users to the game, by making accounts and storing user's credentials. A database connectivity will be required for backend.

# Thank you!
