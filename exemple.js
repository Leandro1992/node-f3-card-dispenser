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
        // checkSensorStatus()
        checkDispenserStatus();
    }).catch((err) => {
        console.log(err)
    })
}

const checkDispenserStatus = () => {
    myDispenser.checkDispenserStatus().then((status) => {
        console.log(status, "status!");
    }).catch((err) => {
        console.log(err)
    })
    
}

const init = () => {
    myDispenser.connect().then((bew) => {
        console.log(bew);
        checkDispenserStatus();
        move(4)
    }).catch((err1) => {
        console.log(err1)
    })
}

init()