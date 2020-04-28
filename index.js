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

    // MM_RETURN_TO_FRONT - s3,s4,s7
    // MM_RETURN_TO_IC_POS- s1,s2,s3,s4,s7
    // MM_RETURN_TO_RF_POS - s1,s2,s3,s7
    // MM_CAPTURE_TO_BOX - 	s7
    // MM_EJECT_TO_FRONT - s4, s7
    // S7 - WHEN S7 IS NOT FOUND THERE IS NO CARD IN THE BOX
    checkSensorStatus() {
        return new Promise((resolve, reject) => {
            if (this.connected == 0) {
                const status = dispenser.sensorStatus();
                resolve(status);
            } else {
                resolve(status);
            }
        })
    }

    checkDispenserStatus() {
        return new Promise((resolve, reject) => {
            if (this.connected == 0) {
                const status = dispenser.checkDispenserStatus();
                console.log(status, typeof status)
                if(status && typeof status == 'object'){
                    let filterResult = Object.keys(status).map((key) => {
                        return status[key] = Util[key][status[key]]
                    });
                    resolve(filterResult);
                }else{
                    resolve(status);
                }
            } else {
                resolve(status);
            }
        })
    }

    allowInsertCard() {

    }

    notAllowInsertCard() {

    }

    readingPosition() {

    }

    finalPosition() {

    }

    stockStatus() {

    }

    discart() {

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