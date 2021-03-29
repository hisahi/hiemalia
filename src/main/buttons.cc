/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// buttons.cc: implementation of ButtonSetup

#include "buttons.hh"

namespace hiemalia {
void ButtonSetup::load(ConfigSectionStore store) {
    std::string key;
    for (const auto &input : controlInputs) {
        if (store.has<int>(input.name))
            control(input.value) = store.get<int>(input.name);
    }
}

void ButtonSetup::save(ConfigSectionStore store) const {
    for (const auto &input : controlInputs) {
        control_t c = control(input.value);
        if (c != controlNone) store.set<int>(input.name, c);
    }
}
}  // namespace hiemalia
