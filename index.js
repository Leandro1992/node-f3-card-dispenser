const dispenser = require('./build/Release/f3_dispenser');

//Exemple call funcrion from c++ addon
console.log(dispenser.hello("hello"));

//Connect on dispenser
console.log(dispenser.connect());

//Move out the card
setTimeout(() => {
    console.log("Movendo para fora", dispenser.move("MM_EJECT_TO_FRONT"));
}, 2000)

//Set free COM connection 
setTimeout(() => {
    console.log(dispenser.disconnect())
}, 4000)