#ifndef LEARN_CONTROLLER_H
#define LEARN_CONTROLLER_H

#include <string>
#include <sstream>
#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Services/SdService.h>
#include <Services/IrLearnService.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Selections/LearnSelection.h>

using namespace views;
using namespace inputs;
using namespace services;
using namespace contexts;
using namespace selections;

namespace controllers {

class LearnController {
public:
    LearnController(CardputerView& display, CardputerInput& input,
                    SdService& sdService, IrLearnService& irLearnService,
                    LearnSelection& learnSelection);

    void handleLearnRemote();

private:
    std::string buildIrFileContent(const std::string& remoteName,
                                   const std::vector<LearnedButton>& buttons);
    std::string buildRawDataString(const std::vector<uint16_t>& rawData);

    CardputerView& display;
    CardputerInput& input;
    SdService& sdService;
    IrLearnService& irLearnService;
    LearnSelection& learnSelection;
    GlobalContext& globalContext = GlobalContext::getInstance();
    SelectionContext& selectionContext = SelectionContext::getInstance();
};

} // namespace controllers

#endif // LEARN_CONTROLLER_H
