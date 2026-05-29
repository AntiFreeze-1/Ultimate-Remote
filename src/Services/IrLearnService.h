#ifndef IR_LEARN_SERVICE_H
#define IR_LEARN_SERVICE_H

#include <vector>
#include <Contexts/GlobalContext.h>

using namespace contexts;

namespace services {

struct LearnedSignal {
    std::vector<uint16_t> rawData;
    int frequency;
    bool valid;
};

class IrLearnService {
public:
    void begin();
    void stop();
    LearnedSignal capture(unsigned long timeoutMs = 8000);

private:
    GlobalContext& globalContext = GlobalContext::getInstance();
};

}

#endif // IR_LEARN_SERVICE_H
