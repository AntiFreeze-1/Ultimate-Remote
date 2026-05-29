#include "LearnController.h"

namespace controllers {

LearnController::LearnController(CardputerView& display, CardputerInput& input,
                                 SdService& sdService, IrLearnService& irLearnService,
                                 LearnSelection& learnSelection)
    : display(display), input(input), sdService(sdService),
      irLearnService(irLearnService), learnSelection(learnSelection) {}

void LearnController::handleLearnRemote() {
    char key = KEY_NONE;

    // Intro screen
    display.displayLearnInfo();
    while (key != KEY_OK) {
        key = input.handler();
        if (key == KEY_RETURN) {
            selectionContext.setIsModeSelected(false);
            return;
        }
    }

    // Ask for remote name
    display.displayTopBar("Learn Remote");
    std::string remoteName = learnSelection.selectRemoteName();
    if (remoteName.empty()) {
        selectionContext.setIsModeSelected(false);
        return;
    }

    // Check SD card early so the user doesn't lose work
    sdService.begin();
    if (!sdService.getSdState()) {
        display.displayLearnNoSd();
        key = KEY_NONE;
        while (key != KEY_OK) {
            key = input.handler();
        }
        sdService.close();
        selectionContext.setIsModeSelected(false);
        return;
    }

    std::vector<LearnedButton> buttons;

    // Button capture loop
    while (true) {
        display.displayTopBar("Learn Remote", true);
        display.displayLearnButtonChoice(remoteName, (int)buttons.size());

        LearnButtonAction action = learnSelection.waitForButtonChoice();

        if (action == LearnButtonAction::DONE) {
            break;
        }

        std::string buttonName;

        if (action == LearnButtonAction::PRESET) {
            buttonName = learnSelection.selectPresetName();
        } else {
            buttonName = learnSelection.selectCustomButtonName();
        }

        if (buttonName.empty()) {
            continue;
        }

        LearnedButton btn;
        bool captured = learnSelection.captureButton(buttonName, irLearnService, btn);
        if (captured) {
            buttons.push_back(btn);
        }
    }

    if (buttons.empty()) {
        sdService.close();
        selectionContext.setIsModeSelected(false);
        return;
    }

    sdService.makeDir("/learned");
    std::string filePath = "/learned/" + remoteName + ".ir";
    std::string fileContent = buildIrFileContent(remoteName, buttons);
    sdService.writeFile(filePath.c_str(), fileContent);
    sdService.close();

    // Show success
    display.displayLearnSaved(remoteName);
    key = KEY_NONE;
    while (key != KEY_OK) {
        key = input.handler();
    }

    selectionContext.setIsModeSelected(false);
}

std::string LearnController::buildIrFileContent(const std::string& remoteName,
                                                 const std::vector<LearnedButton>& buttons) {
    std::ostringstream oss;
    oss << "Filetype: IR signals file\n";
    oss << "Version: 1\n";

    for (const auto& btn : buttons) {
        oss << "#\n";
        oss << "name: " << btn.name << "\n";
        oss << "type: raw\n";
        oss << "frequency: " << (btn.frequency * 1000) << "\n";
        oss << "duty_cycle: 0.330000\n";
        oss << "data: " << buildRawDataString(btn.rawData) << "\n";
    }

    return oss.str();
}

std::string LearnController::buildRawDataString(const std::vector<uint16_t>& rawData) {
    std::ostringstream oss;
    for (size_t i = 0; i < rawData.size(); i++) {
        if (i > 0) oss << " ";
        oss << rawData[i];
    }
    return oss.str();
}

} // namespace controllers
