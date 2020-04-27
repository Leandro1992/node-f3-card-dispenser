const Dispenser = require('./index');

const myDispenser = new Dispenser();

const disconnect = () => {
    console.log("chamei para desconectar")
    myDispenser.disconnect().then((bew) => {
        console.log(bew)
    }).catch((err1) => {
        console.log(err1)
    })
}

const checkSensorStatus = () => {
    myDispenser.checkSensorStatus().then((status) => {
        console.log(status, "status!");
        disconnect()
    }).catch((err) => {
        console.log(err)
    })
}

const move = (position) => {
    myDispenser.move(position).then((move) => {
        console.log(move);
        checkSensorStatus()
    }).catch((err) => {
        console.log(err)
    })
}

const init = () => {
    myDispenser.connect().then((bew) => {
        console.log(bew)
        move(5)
    }).catch((err1) => {
        console.log(err1)
    })
}

init()