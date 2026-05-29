#include "LearnSelection.h"

namespace selections {

static const std::vector<std::string> PRESET_NAMES = {
    "Power", "Mute", "Vol+", "Vol-", "Ch+", "Ch-",
    "OK", "Up", "Down", "Left", "Right",
    "Menu", "Back", "Home", "Exit",
    "Play", "Pause", "Stop", "Forward", "Rewind",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "Input", "HDMI", "Netflix", "Info", "Sleep"
};

LearnSelection::LearnSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input), stringPromptSelection(display, input) {}

const std::vector<std::string>& LearnSelection::getPresetNames() {
    return PRESET_NAMES;
}

std::string LearnSelection::selectRemoteName() {
    display.displayTopBar("Learn Remote");
    return stringPromptSelection.select("Name your remote");
}

std::string LearnSelection::selectCustomButtonName() {
    display.displayTopBar("Learn Remote", true);
    return stringPromptSelection.select("Button name");
}

LearnButtonAction LearnSelection::waitForButtonChoice() {
    char key = KEY_NONE;
    while (key == KEY_NONE) {
        key = input.handler();
    }
    if (key == KEY_DEL) {
        return LearnButtonAction::DONE;
    }
    if (key == KEY_OK) {
        return LearnButtonAction::PRESET;
    }
    // any printable character triggers custom name entry
    if (isprint(key)) {
        return LearnButtonAction::CUSTOM;
    }
    return LearnButtonAction::DONE;
}

std::string LearnSelection::selectPresetName() {
    const std::vector<std::string>& names = getPresetNames();
    uint16_t selectionIndex = 0;
    int8_t lastIndex = -1;
    char key = KEY_NONE;

    display.displayTopBar("Pick Button", true);

    while (key != KEY_OK) {
        if (lastIndex != (int8_t)selectionIndex) {
            display.displaySelection(names, selectionIndex);
            lastIndex = selectionIndex;
        }

        key = input.handler();
        switch (key) {
            case KEY_ARROW_DOWN:
                selectionIndex = (selectionIndex < (uint16_t)(names.size() - 1)) ? selectionIndex + 1 : 0;
                break;
            case KEY_ARROW_UP:
                selectionIndex = (selectionIndex > 0) ? selectionIndex - 1 : (uint16_t)(names.size() - 1);
                break;
            case KEY_RETURN:
                return "";
            default:
                break;
        }
    }

    return names[selectionIndex];
}

bool LearnSelection::captureButton(const std::string& buttonName, IrLearnService& learnService, LearnedButton& out) {
    display.displayTopBar("Learn Remote", true);
    display.displayLearnCapturing(buttonName);

    LearnedSignal signal = learnService.capture(8000);

    if (!signal.valid) {
        // Show timeout UI — user can retry (any key) or skip (OK)
        display.displayLearnTimeout();
        char key = KEY_NONE;
        while (key == KEY_NONE) {
            key = input.handler();
        }
        if (key == KEY_OK) {
            return false; // skip this button
        }
        // retry
        display.displayTopBar("Learn Remote", true);
        display.displayLearnCapturing(buttonName);
        signal = learnService.capture(8000);
        if (!signal.valid) {
            return false;
        }
    }

    out.name = buttonName;
    out.rawData = signal.rawData;
    out.frequency = signal.frequency;

    display.displayTopBar("Learn Remote", true);
    display.displayLearnCaptured(buttonName);
    delay(900);

    return true;
}

} // namespace selections
