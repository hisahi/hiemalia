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

#include "abase.hh"
#include "hbase.hh"
#include "ibase.hh"
#include "logger.hh"
#include "vbase.hh"

namespace hiemalia {

Hiemalia::Hiemalia(const std::string &command)
    : command_(command), host_(std::move(getHostModule())) {}

Hiemalia::~Hiemalia() {}

Hiemalia::Hiemalia(Hiemalia &&move)
    : command_(move.command_),
      state_(std::move(move.state_)),
      host_(std::move(move.host_)),
      input_(std::move(move.input_)),
      video_(std::move(move.video_)),
      audio_(std::move(move.audio_)),
      logic_(std::move(move.logic_)) {}

Hiemalia &Hiemalia::operator=(Hiemalia &&move) {
    command_ = move.command_;
    state_ = std::move(move.state_);
    host_ = std::move(move.host_);
    input_ = std::move(move.input_);
    video_ = std::move(move.video_);
    audio_ = std::move(move.audio_);
    logic_ = std::move(move.logic_);
    return *this;
}

void Hiemalia::args(std::vector<std::string> args) {
    // TODO
}

void Hiemalia::run() {
    state_.config.load(configFileName);
    SplinterBuffer &sbuf = state_.sbuf;

    input_ = std::move(getInputModule(host_));
    video_ = Module::create_unique<VideoEngine>(host_);
    audio_ = Module::create_unique<AudioEngine>(host_);
    logic_ = Module::create_unique<LogicEngine>();

    host_->begin();
    logger.debug("Entering main game loop");
    while (host_->proceed()) {
        sbuf.clear();
        input_->update(state_.controls, controlsmenu_);
        // TODO: controlsmenu_ to menu messages
        audio_->tick();
        logic_->run(state_, tickInterval);
        video_->frame(sbuf);
        video_->sync();
    }
    logger.debug("Finishing up");
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
    stream << "Unhandled exception: " << e.what();
    return stream.str();
}

void never_(const std::string &file, unsigned line, const std::string &msg) {
    if (logger_ok)
        logger.fail(fail_never_(file, line, msg));
    else
        std::cerr << fail_never_(file, line, msg) << std::endl;
    std::terminate();
}

void dynamic_assert_(const std::string &file, unsigned line, bool condition,
                     const std::string &msg) {
    if (!condition) {
        if (logger_ok)
            logger.fail(fail_assert_(file, line, msg));
        else
            std::cerr << fail_assert_(file, line, msg) << std::endl;
        std::terminate();
    }
}

static int main(int argc, char *argv[]) {
    logger.addHandler<StdLogHandler>(LogLevel::TRACE);
    Hiemalia game{std::string(argv[0])};
    std::vector<std::string> args;
    if (argc > 1) args.assign(argv + 1, argv + argc);
    game.args(args);
    try {
        game.run();
    } catch (std::exception &e) {
        logger.fail(fail_exception_(e));
        throw;
    }
    return 0;
}

}  // namespace hiemalia

int main(int argc, char *argv[]) { return hiemalia::main(argc, argv); }
