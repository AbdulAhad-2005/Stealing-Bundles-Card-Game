let moveallowed = true;
let difficulties = [];
let playernum = 0;
function adjustLeftCards(count) {
    const handDiv = document.getElementById('lefthand');
    const vertGap = handDiv.querySelector('.vertgap');
    const existingCards = [...handDiv.querySelectorAll('.sidecard')].filter(card => card.parentNode == handDiv);
    const currentCount = existingCards.length;
    if (count > currentCount) {
        for (let i = 0; i < count - currentCount; i++) {
            const newCard = document.createElement('img');
            newCard.className = 'sidecard';
            newCard.src = 'static/images/sideback.png';
            handDiv.insertBefore(newCard, vertGap);
        }
    } else if (count < currentCount) {
        for (let i = 0; i < currentCount - count; i++) {
            const cardToRemove = existingCards.pop();
            cardToRemove.remove();
        }
    }
}

function adjustRightCards(count) {
    const handDiv = document.getElementById('righthand');
    const vertGap = handDiv.querySelector('.vertgap');
    const existingCards = [...handDiv.querySelectorAll('.sidecard')].filter(card => card.parentNode == handDiv);
    const currentCount = existingCards.length;
    if (count > currentCount) {
        for (let i = 0; i < count - currentCount; i++) {
            const newCard = document.createElement('img');
            newCard.className = 'sidecard';
            newCard.src = 'static/images/sideback.png';
            vertGap.insertAdjacentElement("afterend", newCard);
        }
    } else if (count < currentCount) {
        for (let i = 0; i < currentCount - count; i++) {
            const cardToRemove = existingCards.pop();
            cardToRemove.remove();
        }
    }
}
function adjustTopCards(count) {

    const handDiv = document.getElementById('tophand');

    const existingCards = [...handDiv.querySelectorAll('.card')];

    const currentCount = existingCards.length;

    if (count > currentCount) {
        for (let i = 0; i < count - currentCount; i++) {
            const newCard = document.createElement('img');
            newCard.className = 'card';
            newCard.src = 'static/images/back.png';
            handDiv.appendChild(newCard);
        }
    } else if (count < currentCount) {
        for (let i = 0; i < currentCount - count; i++) {
            const cardToRemove = existingCards.pop();
            cardToRemove.remove();
        }
    }
}

function adjustBundles(bundles) {
    console.log("players: "+playernum);
    if (playernum == 4) {
        for (let i = 0; i < 4; i++) {
            const bundle = document.getElementById('bundle' + String(i));
            const imgElement = bundle.querySelector('img');
            imgElement.remove();
            const newCard = document.createElement('img');
            if (i == 0 || i == 2) {
                newCard.className = 'card';
                newCard.src = 'static/images/cards/' + String(bundles[i][0]) + '.png';
            }
            else {
                newCard.className = 'sidecard';
                newCard.src = 'static/images/leftcards/' + String(bundles[i][0]) + '.png';
            }
            if (String(bundles[i].length) == 0) {
                newCard.style.visibility = 'hidden';
            }
            bundle.appendChild(newCard);
        }
    }
    else if (playernum == 3) {
        document.getElementById('bundle2').style.visibility = 'hidden';
        for (let i = 0; i < 3; i++) {
            const bundle = document.getElementById('bundle' + String((i<2)?i:3));
            const imgElement = bundle.querySelector('img');
            imgElement.remove();
            const newCard = document.createElement('img');
            if (i == 0) {
                newCard.className = 'card';
                newCard.src = 'static/images/cards/' + String(bundles[i][0]) + '.png';
            }
            else {
                newCard.className = 'sidecard';
                newCard.src = 'static/images/leftcards/' + String(bundles[i][0]) + '.png';
            }
            if (String(bundles[i].length) == 0) {
                newCard.style.visibility = 'hidden';
            }
            bundle.appendChild(newCard);
        }
    }
    else if (playernum == 2) {
        document.getElementById('bundle1').style.visibility = 'hidden';
        document.getElementById('bundle3').style.visibility = 'hidden';
        for (let i = 0; i < 2; i++) {
            const bundle = document.getElementById('bundle' + String((i < 1) ? i : 2));
            const imgElement = bundle.querySelector('img');
            imgElement.remove();
            const newCard = document.createElement('img');
            newCard.className = 'card';
            newCard.src = 'static/images/cards/' + String(bundles[i][0]) + '.png';
            if (String(bundles[i].length) == 0) {
                newCard.style.visibility = 'hidden';
            }
            bundle.appendChild(newCard);
        }
    }
}

function adjustPool(pool) {

    const poolTop = document.getElementById('topcards');
    const poolBot = document.getElementById('botcards');

    poolTop.replaceChildren();
    poolBot.replaceChildren();

    for (let i = 0; i < pool.length; i++) {
        const newCard = document.createElement('img');
        const holder = document.createElement('div');
        const vertalign = document.createElement('div');
        vertalign.className = 'vertalign';
        holder.className = 'poolcard';
        newCard.className = 'poolimg';
        newCard.src = 'static/images/cards/' + String(pool[i]) + '.png';
        holder.appendChild(newCard);
        vertalign.appendChild(holder);
        if (i < 6) {
            poolTop.appendChild(vertalign);
        } else {
            poolBot.appendChild(vertalign);
        }
    }
}

function endGame() {
    window.location.replace("/results")
}

function playAgain() {
    window.location.replace("/")
}

async function play() {
    const response = await fetch('/getDifficulties', {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json'
        }
    });
    if (response.ok) {
        const data = await response.json();
        console.log(data);
        difficulties = data.difficulties;
        playernum = data.playernum;
    }
}

function adjustBotCards(count,hand) {

    const handDiv = document.getElementById('bothand');

    const existingCards = [...handDiv.querySelectorAll('.card')];

    let cards = existingCards.length;

    for (let i = 0; i < cards; i++) {
        const cardToRemove = existingCards.pop();
        cardToRemove.remove();
    }
    console.log(existingCards);
    for (let i = 0; i < count; i++) {
        const newCard = document.createElement('img');
        newCard.className = 'card';
        newCard.src = 'static/images/cards/' + String(hand[i]) + '.png';
        newCard.onclick = function () {
            playRound(i)
        };
        handDiv.appendChild(newCard);
    }
}

async function finish() {
    const response = await fetch('/updateView', {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json'
        }
    });
    if (response.ok) {
        const data = await response.json();
        console.log(data);
        if (data.gameOver) {
            const response = await fetch('/finish', {
                method: 'GET',
                headers: {
                    'Content-Type': 'application/json'
                }
            });
            if (response.ok) {
                const data = await response.json();
                const msg = document.getElementById('msg');
                const xmsg = document.getElementById('xmsg');
                let winner = 0;
                let max = 0
                for (let i = 0; i < data.bundle.length; i++) {
                    if (data.bundle[i].length > max) {
                        console.log(data.bundle[i].length)
                        max = data.bundle[i].length
                        winner = i;
                        console.log("Winner: "+winner)
                    }
                }
                if (winner == 0) {
                    msg.innerHTML = 'You Win!';
                    xmsg.innerHTML = 'Congratulations! You outsmarted the computer.';
                }
                else {
                    msg.innerHTML = 'Player ' + String(winner+1) + ' Wins!';
                    xmsg.innerHTML = 'Better Luck Next Time!'
                }
            }
            else {
                window.location.replace("/play")
            }
        }

    }
}


        async function updateView() {

            try {
                const response = await fetch('/updateView', {
                    method: 'GET',
                    headers: {
                        'Content-Type': 'application/json'
                    }
                });

                if (response.ok) {
                    const data = await response.json();
                    console.log(data);
                    handSizes = data.handSizes;

                    if (!data.gameOver) {
                        console.log(playernum);
                        if (playernum == 4) {
                            adjustBotCards(handSizes[0], data.hand);
                            adjustLeftCards(handSizes[1]);
                            adjustTopCards(handSizes[2]);
                            adjustRightCards(handSizes[3]);
                            adjustPool(data.pool);
                            adjustBundles(data.bundle);
                        }
                        else if (playernum == 3) {
                            adjustBotCards(handSizes[0], data.hand);
                            adjustLeftCards(handSizes[1]);
                            adjustTopCards(0);
                            adjustRightCards(handSizes[2]);
                            adjustPool(data.pool);
                            adjustBundles(data.bundle);
                        }
                        else if (playernum == 2) {
                            adjustBotCards(handSizes[0], data.hand);
                            adjustLeftCards(0);
                            adjustTopCards(handSizes[1]);
                            adjustRightCards(0);
                            adjustPool(data.pool);
                            adjustBundles(data.bundle);
                        }
                    }
                    else {
                        endGame();
                    }

                } else {
                    console.log('Error occurred');
                }
            } catch (error) {
                console.error('Error calling backend:', error);
                document.getElementById('result').textContent = 'Error calling backend';
            }
        }

        async function reDeal() {

            try {
                const response = await fetch('/reDeal', {
                    method: 'GET',
                    headers: {
                        'Content-Type': 'application/json'
                    }
                });
                if (response.ok) {
                    await response.json();
                }
            } catch (error) {
                console.log("Error calling backend");
            }
        }

        async function fillPool() {

            try {
                const response = await fetch('/fillPool', {
                    method: 'GET',
                    headers: {
                        'Content-Type': 'application/json'
                    }
                });

                if (response.ok) {
                    const data = await response.json();
                    console.log(data);
                } else {
                    console.log("Error occurred");
                }
            } catch (error) {
                console.error('Error calling backend:', error);
            }
        }

        async function sleep(ms) {
            return new Promise(resolve => setTimeout(resolve, ms));
        }

        async function playRound(card) {
            if (moveallowed) {
                moveallowed = false;
                console.log(playernum);
                for (let i = 0; i < playernum; i++) {
                    console.log(i)
                    console.log(card)
                    console.log(difficulties[i])
                    await playCard(i, card, difficulties[i]);
                    await fillPool();
                    await updateView();
                    await sleep(1000);
                }
                await reDeal();
                await updateView();
                moveallowed = true;
            }
        }

        async function playCard(id, card, diff) {

            const payload = {
                id: parseInt(id),
                card: parseInt(card),
                diff: parseInt(diff)
            };

            try {
                const response = await fetch('/playCard', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify(payload)
                });

                if (response.ok) {
                    await response.json();
                } else {
                    console.log("Error Occurred");
                }
            } catch (error) {
                console.error('Error calling backend:', error);
            }
        }