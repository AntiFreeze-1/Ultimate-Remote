#ifndef LEARN_SELECTION_H
#define LEARN_SELECTION_H

#include <string>
#include <vector>
#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>
#include <Selections/StringPromptSelection.h>
#include <Services/IrLearnService.h>

using namespace views;
using namespace inputs;
using namespace contexts;
using namespace services;

namespace selections {

struct LearnedButton {
    std::string name;
    std::vector<uint16_t> rawData;
    int frequency;
};

enum class LearnButtonAction {
    PRESET,
    CUSTOM,
    DONE
};

class LearnSelection {
public:
    LearnSelection(CardputerView& display, CardputerInput& input);

    std::string selectRemoteName();
    std::string selectCustomButtonName();
    LearnButtonAction waitForButtonChoice();
    std::string selectPresetName();
    bool captureButton(const std::string& buttonName, IrLearnService& learnService, LearnedButton& out);

private:
    CardputerView& display;
    CardputerInput& input;
    StringPromptSelection stringPromptSelection;
    GlobalContext& globalContext = GlobalContext::getInstance();

    static const std::vector<std::string>& getPresetNames();
};

}

#endif // LEARN_SELECTION_H
