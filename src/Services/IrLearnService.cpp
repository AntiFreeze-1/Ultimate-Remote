#define MICROS_PER_TICK 50
#include <IRProtocol.h>
#include <IRremoteInt.h>
#include "IrLearnService.h"

namespace services {

void IrLearnService::begin() {
    IrReceiver.begin(globalContext.getIrRxPin(), DISABLE_LED_FEEDBACK);
}

void IrLearnService::stop() {
    IrReceiver.end();
}

LearnedSignal IrLearnService::capture(unsigned long timeoutMs) {
    LearnedSignal result;
    result.frequency = 38; // default carrier frequency in kHz
    result.valid = false;

    IrReceiver.resume();

    unsigned long start = millis();
    while (millis() - start < timeoutMs) {
        if (IrReceiver.decode()) {
            bool overflow = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW;
            uint16_t rawLen = IrReceiver.decodedIRData.rawlen;
            if (!overflow && rawLen >= 10) {
                for (uint16_t i = 1; i < rawLen; i++) {
                    uint32_t us = (uint32_t)IrReceiver.irparams.rawbuf[i] * MICROS_PER_TICK;
                    result.rawData.push_back(us > 65535 ? 65535 : (uint16_t)us);
                }
                result.valid = true;
                IrReceiver.resume();
                break;
            }
            IrReceiver.resume(); // noise or overflow — keep waiting
        }
        delay(1);
    }

    return result;
}

} // namespace services
