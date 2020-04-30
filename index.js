const dispenser = require('./build/Release/f3_dispenser');

const Util = require('./util');

class F3Dipenser {

    constructor(options) {
        let default_options = {
            port: 3,
            baudrate: 9600,
            cardReaderRFC: false,
            cardReaderICC: false
        }
        this.connected = 1;
        this.allowInsert = false;
        this.options = Object.assign(default_options, options)
    }

    //DLL CALL METHODS

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
                reject({ error: Util.msgMapper[12304] });
            }
        })
    }

    //Get an overview about the 12 sensor on the hardware, check docs to see what is each one
    checkSensorStatus() {
        return new Promise((resolve, reject) => {
            if (this.connected == 0) {
                const status = dispenser.sensorStatus();
                resolve(status);
            } else {
                reject({ error: Util.msgMapper[12304] });
            }
        })
    }

    checkDispenserStatus() {
        return new Promise((resolve, reject) => {
            if (this.connected == 0) {
                const status = dispenser.checkDispenserStatus();
                if (status && typeof status == 'object') {
                    let filterResult = Object.keys(status).map((key) => status[key] = Util[key][status[key]]);
                    resolve(filterResult);
                } else {
                    resolve(status);
                }
            } else {
                reject({ error: Util.msgMapper[12304] });
            }
        })
    }

    allowInsertCard() {
        return new Promise((resolve, reject) => {
            if (this.connected == 0) {
                const status = dispenser.allowInsertion();
                if (status == 0) this.allowInsert = true;
                resolve(Util.msgMapper[status]);
            } else {
                reject({ error: Util.msgMapper[12304] });
            }
        })
    }

    denyInsertCard() {
        return new Promise((resolve, reject) => {
            if (this.connected == 0) {
                const status = dispenser.denyInsertion();
                if (status == 0) this.allowInsert = false;
                resolve(Util.msgMapper[status]);
            } else {
                reject({ error: Util.msgMapper[12304] });
            }
        })
    }

    //CARD_READER METHODS

    inicializeCard(action){

    }

    detectRFCType(){

    }

    detectICCType(){
        
    }

    //HELPER METHODS

    //MM_EJECT_TO_FRONT
    moveStraightOut() {
        return this.move(5)
    }
    //MM_CAPTURE_TO_BOX
    moveToCaptureBox() {
        return this.move(4)
    }
    //MM_RETURN_TO_RF_POS
    moveToRFPosition() {
        return this.move(3)
    }
    // MM_RETURN_TO_IC_POS
    moveToICPosition() {
        return this.move(2)
    }
    //MM_RETURN_TO_FRONT
    moveOut() {
        return this.move(1)
    }

    cardBoxStatus() {
        const scope = this;
        return new Promise((resolve, reject) => {
            scope.checkDispenserStatus().then(result => {
                if (Array.isArray(result)) resolve(result[1]);
                reject({ error: 'An error ocurred!' });
            }).catch(reject)
        })
    }

    captureBoxIsFull() {
        const scope = this;
        return new Promise((resolve, reject) => {
            scope.checkDispenserStatus().then(result => {
                if (Array.isArray(result)) resolve(result[2]);
                reject({ error: 'An error ocurred!' });
            }).catch(reject)
        })
    }

    hasCard() {
        const scope = this;
        return new Promise((resolve, reject) => {
            scope.checkDispenserStatus().then(result => {
                if (Array.isArray(result)) resolve(result[0]);
                reject({ error: 'An error ocurred!' });
            }).catch(reject)
        })
    }
}

module.exports = F3Dipenser