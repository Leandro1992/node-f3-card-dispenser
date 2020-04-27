const Dispenser = require('./index');

const myDispenser = new Dispenser();

const connect = () => {
    myDispenser.connect().then((bew) => {
        console.log(bew)
    }).catch((err1) => {
        console.log(err1)
    })
}

const disconnect = () => {
    myDispenser.disconnect().then((bew) => {
        console.log(bew)
    }).catch((err1) => {
        console.log(err1)
    })
}

connect()
setTimeout(() => {disconnect()},2000)