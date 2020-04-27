const dispenser = require('./build/Release/f3_dispenser');

const Util = require('./util');

class F3Dipenser {

    constructor(options) {
        let default_options = {
            port: 3,
            baudrate: 9600
        }
        this.MOVE = {
            MM_RETURN_TO_FRONT: 1,
            MM_RETURN_TO_IC_POS: 2,
            MM_RETURN_TO_RF_POS: 3,
            MM_CAPTURE_TO_BOX: 4,
            MM_EJECT_TO_FRONT: 5,
        }

        this.connected = 1;
        this.options = Object.assign(default_options, options)
    }

    log(msg, error = false) {
        if (this.options.debug) {
            error ? console.error(msg) : console.log(msg);
        }
    }

    connect() {
        return new Promise((resolve, reject) => {
            if (this.connected == 0) {
                resolve(Util.msgMapper[this.connected]);
            } else {
                this.connected = dispenser.connect(this.options.port, this.options.baudrate);
                if (this.connected == 0) {
                    resolve(Util.msgMapper[this.connected]);
                } else {
                    console.log(this.connected)
                    reject({ error: Util.msgMapper[this.connected] });
                }
            }
        })
    }

    disconnect() {
        return new Promise((resolve, reject) => {
            if (this.connected == 0) {
                const disconnect = dispenser.disconnect();
                resolve(Util.msgMapper[disconnect]);
                this.connected = 1;
            } else {
                resolve(Util.msgMapper[0]);
            }
        })
    }

    move(movement) {
        return new Promise((resolve, reject) => {
            if (this.connected == 0) {
                const moved = dispenser.move(movement);
                resolve(Util.msgMapper[moved]);
            } else {
                resolve(Util.msgMapper[12304]);
            }
        })
    }

    checkStock() {

    }

    readingPosition() {

    }

    finalPosition() {

    }

    stockStatus() {

    }

    discard() {

    }

    checkTypeCard() {

    }

    allowInsertCard() {
        
    }

    inicializeCardReader() {

    }

    readCard() {

    }

    handleReturn() {

    }
}

module.exports = F3Dipenser