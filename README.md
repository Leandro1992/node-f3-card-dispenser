# F3 Card dispenser collector with RFID card reader


This module is an addons NodeJS that make a interface with lib of [F3 Card Dispenser].
 On npm i just publish an .node build but if you want have access to lib files and how i build this see [github repository] of this project.

# Features

  - **Connect**  - Open connection to dispenser
  - **Disconnect** - Close connection
  - **Move** - Control rfid card flow inside hardware
  - **Sensor Status** - Check status of 12 available sensors
  - **Dipenser Status** - Consult if has a card on read, if capture box is full and if card box is low, empty or enough
  - **Allow Insert Card** - Permit insert card on dispenser, this is an option and is true by default
  - **Deny Insert Card** - Forbidden insert of card
  - **Read RFID Card** - Just serial number of card mifare S50, but it is possible to extends for new type of reads if you want but is necessaty develop.
  - **Move Straight Out** - Move card full out
  - **Move To Capture Box** - Discard card
  - **Move To RF Position** - Move to reader RFID
  - **Move to IC Position** - Move to reader IC
  - **Move Out** - Move out but not fully
  - **Card Box Status** - Check if card box is low, empty or enough
  - **Capture Box Status** - Check if capture box is full
  - **Has Card** - Check if has a card in any reader position


You can also:
  - Configure to verify in a preset interval the card status, you can pass three callback functions on options, each one of this function are responsable to be called when event ocorrred. See hooks and exemple section to better understand.

### Options

- **port**  - Com port defualt 3
- **baudrate** - Default 9600
- **allowInsert** - this option allow insert card when a connection is instanced. Default is true
- **intervalTimeout** - in milissecond you can configure timeout interval of check of card position (NO_CARD, CART_AT_GATE, CARD_IN_THE_MIDLE), if you enable the action using enableCheckRfid() function
- **handleHasCard** - if you enable auto check tou also can pass a function on this options to call when event 'hasCard' ocurred
- **handleHasCardOnGate** - the same of 'handleHasCard' but event is 'hasCardOnGate'
- **handleNoCardIn** - the same of 'handleHasCard' but event is 'noCardIn'

### Github
Open source with a [Github repository]

### Libraries

| Source | Link |
| ------ | ------ |
| Lintechtt | [F3 Card Dispenser] |

### Installation

Install the dependencies and devDependencies and start the server.

```sh
$ npm i node-f3-card-dispenser
```

### Require and instance

On your code, remenber, this instance open a hardware connection, so we need use as a singleton, the follow exemple already send callback functions but it is not a mandatary options.

I recommend you see exemple.js file and index.js, on this files you can check how actions are called, it is very simple. I hope :)

```javascript
const Dispenser = require('node-f3-card-dispenser');

const handleHasCard = () => {
    //STOP INTERVAL CHECK
    myDispenser.disableCheckRfid();
    //READ CARD VALUE
   readRfid() //CONSULT exemple.js to see how to call this
    // MOVE OUT CARD
    move(1); //CONSULT exemple.js to see how to call this
    // START AGAIN INTERVAL CHECK
    myDispenser.enableCheckRfid();
}

const handleHasCardOnGate = () => {
    console.log("Has a card on the gate, please get your card!")
}

const handleNoCardIn = () => {
    console.log("There is no card in!")
}

// INSTANCE OF DISPENSER
const myDispenser = new Dispenser({ port: 3, baudrate: 9600, allowInsert: true, intervalTimeout: 1000, handleHasCard, handleHasCardOnGate, handleNoCardIn });
```

#### Building

| Dependecy  |
| ------  |
| nodeJS |
| node-gyp |

For build to another platforms i recommend clone deafult [github repository] and run the follow commands

Run on root folder of project:
```sh
$ node-gyp configure

$ node-gyp build
```

After build get file .node on folder /build/Release/ this will be your addon file who you shoud require.

### Todos

See file PDF doc on root folder, in there you can see all is possible to do and get better the current implementation

 - Tests
 - Add ICC reader
 - Read others block of cards
 - White data in mifare or ICC cards
 - Auto detect type of card

License
----

[MIT](https://choosealicense.com/licenses/mit/)

   [node.js]: <http://nodejs.org>
   [Github repository]: <https://github.com/Leandro1992/node-f3-card-dispenser>
   [F3 Card Dispenser]: <https://www.lintechtt.com/product/rfid-card-dispenser/>
