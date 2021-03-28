/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// hiemalia.cc: implementation of main game

#include "hiemalia.hh"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "assets.hh"
#include "debugger.hh"
#include "hbase.hh"
#include "logger.hh"
#include "logic.hh"
#include "menumain.hh"
#include "mholder.hh"

namespace hiemalia {

Hiemalia::Hiemalia(const std::string &command)
    : command_(command), host_(std::move(getHostModule())) {
    getAssets();
}

Hiemalia::~Hiemalia() {}

Hiemalia::Hiemalia(Hiemalia &&move)
    : command_(move.command_),
      state_(std::move(move.state_)),
      host_(std::move(move.host_)),
      modules_(std::move(move.modules_)) {}

Hiemalia &Hiemalia::operator=(Hiemalia &&move) {
    command_ = move.command_;
    state_ = std::move(move.state_);
    host_ = std::move(move.host_);
    modules_ = std::move(move.modules_);
    return *this;
}

void Hiemalia::gotMessage(const HostMessage &msg) {
    switch (msg.type) {
        case HostMessageType::MainMenu:
            sendMessage(LogicMessage::mainMenu(modules_));
            break;
        case HostMessageType::Quit:
            host_->quit();
            break;
    }
}

void Hiemalia::args(std::vector<std::string> args) {
    // TODO
}

void Hiemalia::run() {
    SplinterBuffer &sbuf = state_.sbuf;

    state_.config.load(configFileName);
    modules_ = std::make_unique<ModuleHolder>(host_, state_);
    ModuleHolder &m = *modules_;
    m.loadAssets();

    host_->begin();
    sendMessage(HostMessage::mainMenu());
    LOG_DEBUG("Entering main game loop");
    while (host_->proceed()) {
        m.video->frame(sbuf);
        m.video->sync();
        sbuf.clear();
        m.audio->tick();
        m.input->update(state_);
        m.logic->run(state_, tickInterval);
    }
    LOG_DEBUG("Finishing up");
    host_->finish();

    state_.config.save(configFileName);
}

static std::string fail_assert_(const std::string &file, unsigned line,
                                const std::string &msg) {
    std::ostringstream stream;
    stream << "Assertion failure (" << file << ":" << line << "): " << msg;
    return stream.str();
}

static std::string fail_never_(const std::string &file, unsigned line,
                               const std::string &msg) {
    std::ostringstream stream;
    stream << "The fabric of time has been altered (" << file << ":" << line
           << "): " << msg;
    return stream.str();
}

static std::string fail_exception_(const std::exception &e) {
    std::ostringstream stream;
    stream << "Unhandled exception (" << typeid(e).name() << "): " << e.what();
    return stream.str();
}

static std::string fail_exception_u_() {
    std::ostringstream stream;
    stream << "Unhandled exception of unusual type";
    return stream.str();
}

void never_(const std::string &file, unsigned line, const std::string &msg) {
    if (logger_ok)
        LOG_FAIL(fail_never_(file, line, msg));
    else
        std::cerr << fail_never_(file, line, msg) << std::endl;
    debugger();
    std::terminate();
}

void dynamic_assert_(const std::string &file, unsigned line, bool condition,
                     const std::string &msg) {
    if (!condition) {
        if (logger_ok)
            LOG_FAIL(fail_assert_(file, line, msg));
        else
            std::cerr << fail_assert_(file, line, msg) << std::endl;
        debugger();
        std::terminate();
    }
}

static int main(int argc, char *argv[]) {
    LOG_ADD_HANDLER(StdLogHandler, LogLevel::TRACE);
    Hiemalia game{std::string(argv[0])};
    std::vector<std::string> args;
    if (argc > 1) args.assign(argv + 1, argv + argc);
    game.args(args);
    try {
        game.run();
    } catch (std::exception &e) {
        LOG_FAIL(fail_exception_(e));
        debugger();
        throw;
    } catch (...) {
        LOG_FAIL(fail_exception_u_());
        debugger();
        throw;
    }
    return 0;
}

}  // namespace hiemalia

int main(int argc, char *argv[]) { return hiemalia::main(argc, argv); }
