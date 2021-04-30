/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/demo.cc: implementation of demo mode

#include "game/demo.hh"

#include "file.hh"

namespace hiemalia {
static ControlInput nameToInput(const std::string& s) {
    for (const auto& pair : controlInputs) {
        if (pair.name == s) return pair.value;
    }
    throw std::runtime_error("invalid control");
}

static double getCommandTime(std::string& s) {
    auto t = s.substr(0, s.find(' '));
    if (t.size() < s.size()) {
        s = s.substr(s.find(' ') + 1);
    } else {
        throw std::runtime_error("no time for command");
    }
    return fromString<double>(t);
}

DemoFile DemoFile::loadDemo(const std::string& file) {
    DemoFile demo{};

    auto in = openAssetFileRead("demos", file, false);
    if (in.fail()) throw std::runtime_error("cannot open demo file " + file);
    std::vector<DemoCommand> commands;

    for (std::string line; std::getline(in, line);) {
        if (line.empty() || line[0] == '#') continue;
        auto command = line.substr(0, line.find(' '));
        std::string value;
        if (command.size() < line.size()) {
            value = line.substr(line.find(' ') + 1);
        }

        if (command == "d") {  // d <time> <button>
            double time = getCommandTime(value);
            ControlInput button = nameToInput(value);
            commands.push_back(
                DemoCommand{time, DemoCommandType::ButtonDown, button});
        } else if (command == "u") {  // u <time> <button>
            double time = getCommandTime(value);
            ControlInput button = nameToInput(value);
            commands.push_back(
                DemoCommand{time, DemoCommandType::ButtonUp, button});
        } else if (command == "x") {  // x <time>
            double time = getCommandTime(value);
            commands.push_back(DemoCommand{time, DemoCommandType::DemoEnd,
                                           ControlInput::Pause});
        } else if (command == "s") {  // s <stage>
            demo.stageNum_ = fromString<int>(value);
        } else if (command == "z") {  // z <offset>
            demo.checkpoint_ = fromString<coord_t>(value);
        }
    }

    std::sort(commands.begin(), commands.end(),
              [&](const DemoCommand& a, const DemoCommand& b) {
                  return a.time < b.time;
              });
    demo.commands_ = std::move(commands);
    return demo;
}

bool DemoFile::runDemo(float dt) {
    t_ += dt;
    size_t commandCount = commands_.size();
    while (commandIndex_ < commandCount &&
           t_ >= commands_[commandIndex_].time) {
        DemoCommand& cmd = commands_[commandIndex_++];
        switch (cmd.type) {
            case DemoCommandType::ButtonDown:
                input_[cmd.input] = true;
                break;
            case DemoCommandType::ButtonUp:
                input_[cmd.input] = false;
                break;
            case DemoCommandType::DemoEnd:
                return false;
        }
    }
    return true;
}

void DemoFile::reset() {
    commandIndex_ = 0;
    t_ = 0;
}

static auto demoFileNames = hiemalia::makeArray<std::string>({"demo.dem"});
static std::vector<std::shared_ptr<DemoFile>> loadedDemos{demoFileNames.size(),
                                                          nullptr};
static size_t nextDemo = 0;

std::shared_ptr<DemoFile> getNextDemo() {
    if (loadedDemos.empty()) return nullptr;
    if (!loadedDemos[nextDemo])
        loadedDemos[nextDemo] = std::make_shared<DemoFile>(
            DemoFile::loadDemo(demoFileNames[nextDemo]));
    nextDemo = (nextDemo + 1) % loadedDemos.size();
    loadedDemos[nextDemo].reset();
    return loadedDemos[nextDemo];
}
}  // namespace hiemalia
