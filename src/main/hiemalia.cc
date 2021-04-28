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
#include "file.hh"
#include "game/gamemsg.hh"
#include "hbase.hh"
#include "hholder.hh"
#include "logger.hh"
#include "logic.hh"
#include "mholder.hh"
#include "scores.hh"
#include "sys.hh"

namespace hiemalia {

static bool keepConsoleOpen = false;

Hiemalia::Hiemalia(const std::string &command)
    : command_(command), host_(getHostModule()) {}

Hiemalia::~Hiemalia() {}

Hiemalia::Hiemalia(Hiemalia &&move) noexcept
    : command_(move.command_),
      state_(std::move(move.state_)),
      host_(std::move(move.host_)),
      modules_(std::move(move.modules_)) {}

Hiemalia &Hiemalia::operator=(Hiemalia &&move) noexcept {
    command_ = move.command_;
    state_ = std::move(move.state_);
    host_ = std::move(move.host_);
    modules_ = std::move(move.modules_);
    return *this;
}

void Hiemalia::resetArcade() {
    overlay_->visible(state_.arcade);
    credits_ = 0;
    overlay_->redraw(credits_, modules_->gconfig->arcadeFreePlay);
}

void Hiemalia::gotMessage(const HostMessage &msg) {
    switch (msg.type) {
        case HostMessageType::MainMenu:
            sendMessage(LogicMessage::mainMenu(modules_, state_.arcade));
            if (state_.arcade) resetArcade();
            break;
        case HostMessageType::GotHighScore:
            sendMessage(LogicMessage::highScore(
                modules_,
                HighScoreSubmit{&state_.highScores, msg.highScoreEntry()}));
            break;
        case HostMessageType::EnteredHighScore:
            if (state_.arcade) resetArcade();
            break;
        case HostMessageType::GameStarted:
            if (state_.arcade) {
                if (modules_->gconfig->arcadeFreePlay) {
                    sendMessage(GameMessage::addCredits(
                        modules_->gconfig->maxContinues));
                } else {
                    dynamic_assert(credits_ > 0, "must have credits");
                    if (--credits_ > 0)
                        sendMessage(GameMessage::addCredits(credits_));
                }
            }
            overlay_->visible(false);
            break;
        case HostMessageType::Quit:
            host_->quit();
            break;
    }
}

int Hiemalia::addCredits(int cr) {
    if (!state_.arcade || modules_->gconfig->arcadeFreePlay) return 0;
    int maxCredits = modules_->gconfig->arcadeMaxCredits;
    int oldCredits = credits_;
    dynamic_assert(credits_ <= maxCredits && cr > 0, "invalid configuration");
    credits_ = std::min(maxCredits, credits_ + cr);
    int diff = credits_ - oldCredits;
    if (diff > 0) {
        sendMessage(AudioMessage::playSound(SoundEffect::Credit));
        sendMessage(GameMessage::addCredits(diff));
        overlay_->redraw(credits_, modules_->gconfig->arcadeFreePlay);
    }
    return diff;
}

void Hiemalia::args(std::vector<std::string> args) {
    bool console = isDebugMode;
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string &arg = args[i];
        if (arg == "-?" || arg == "-h" || arg == "--help") {
            std::ostringstream ss;
            ss << gameTitle << " -- Tunnel flying rail shoot 'em up\n\n";
            ss << "  --help\n";
            ss << "        Help message\n\n";
            ss << "  --console\n";
            ss << "        display console on supported systems\n\n";
            ss << "  --keepconsole\n";
            ss << "        keep console open if supported\n\n";
            ss << "  --config <filename>\n";
            ss << "        use alternative config\n\n";
            ss << "  --log <filename>\n";
            ss << "        sets up log file\n\n";
            ss << "  --debuglog <filename>\n";
            ss << "        sets up debug log file\n\n";
            ss << "  --assets <folder>\n";
            ss << "        use another asset folder\n\n";
            ss << "  --arcade\n";
            ss << "        arcade mode (full screen, no main menu,\n";
            ss << "            no options menu (configure beforehand),\n";
            ss << "            takes credits, quits only on Alt+F4)\n\n";
            sysDisplayHelp(ss.str());
            std::exit(EXIT_SUCCESS);
        } else if (arg == "--console") {
            console = true;
        } else if (arg == "--keepconsole") {
            keepConsoleOpen = true;
        } else if (arg == "--arcade") {
            state_.arcade = true;
        } else if (arg == "--config") {
            if (++i >= args.size())
                LOG_WARN("no argument for --config");
            else
                configFileName = args[i];
        } else if (arg == "--log") {
            if (++i >= args.size())
                LOG_WARN("no argument for --log");
            else {
                auto log = openLogFileWrite(args[i]);
                if (log)
                    LOG_ADD_HANDLER(FileLogHandler, std::move(log),
                                    LogLevel::INFO);
            }
        } else if (arg == "--debuglog") {
            if (++i >= args.size())
                LOG_WARN("no argument for --debuglog");
            else {
                auto log = openLogFileWrite(args[i]);
                if (log)
                    LOG_ADD_HANDLER(FileLogHandler, std::move(log),
                                    LogLevel::TRACE);
            }
        } else if (arg == "--assets") {
            if (++i >= args.size())
                LOG_WARN("no argument for --assets");
            else {
                useAlternativeAssetsFolder(args[i]);
            }
        } else if (!arg.empty() && arg[0] == '-') {
            LOG_WARN("unrecognized flag '" + arg + "'");
        }
    }

    keepConsoleOpen &= console;
    if (console) {
        keepConsoleOpen &= sysOpenConsole();
    }
}

void Hiemalia::run() {
    SplinterBuffer &sbuf = state_.sbuf;
    LOG_DEBUG("Loading assets");
    if (openAssetFileRead("logo.2d", false).fail())
        throw std::runtime_error(
            "Cannot load 'logo.2d'. You might be missing the game assets. "
            "Please redownload.");
    getAssets();

    state_.config.load(configFileName);
    modules_ = std::make_shared<ModuleHolder>(host_, state_);
    overlay_ = std::make_shared<ArcadeOverlay>(modules_);
    ModuleHolder &m = *modules_;
    if (state_.arcade) {
        m.video->setFullScreenOrElse();
        host_->arcade();
    }
    m.loadAssets();
    state_.highScores = loadHighscores();

    host_->begin();
    gotMessage(HostMessage::mainMenu());
    LOG_DEBUG("Entering main game loop");
    while (host_->proceed()) {
        m.video->frame(sbuf);
        m.video->sync();
        sbuf.clear();
        m.audio->tick();
        m.input->update(state_, tickInterval);
        m.logic->run(state_, tickInterval);
        overlay_->run(state_, tickInterval);
    }
    LOG_DEBUG("Finishing up");
    host_->finish();
    saveHighscores(state_.highScores);

    state_.config.save(configFileName);
}

[[noreturn]] void fail(const char *s) {
    debugger();
    std::string f = stringFormat(
        "Fatal error occurred!\n\n%s\n\nThe game cannot continue.", s);
    sysDisplayError(f);
    if (keepConsoleOpen) sysCloseConsole();
    std::exit(EXIT_FAILURE);
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
    fail(msg.c_str());
}

void dynamic_assert_(const std::string &file, unsigned line, bool condition,
                     const std::string &msg) {
    if (!condition) {
        if (logger_ok)
            LOG_FAIL(fail_assert_(file, line, msg));
        else
            std::cerr << fail_assert_(file, line, msg) << std::endl;
        fail(msg.c_str());
    }
}

static HiemaliaExternal external;

HiemaliaRef::~HiemaliaRef() {
    for (HiemaliaExternal *x : ext) *x = nullptr;
}

void HiemaliaRef::bind(HiemaliaExternal *p) { ext.push_back(p); }

void HiemaliaRef::unbind(HiemaliaExternal *p) { eraseRemove(ext, p); }

HiemaliaExternal::~HiemaliaExternal() {
    if (p) p->unbind(this);
}

int HiemaliaExternal::addCredits(int c) {
    return p ? (*this)->addCredits(c) : 0;
}

int main(int argc, char *argv[]) {
#if NDEBUG
    LOG_ADD_HANDLER(StdLogHandler, LogLevel::WARN);
#else
    LOG_ADD_HANDLER(StdLogHandler, LogLevel::TRACE);
#endif
    std::vector<std::string> args;
    if (argc > 1) args.assign(argv + 1, argv + argc);
    try {
        Hiemalia game{std::string(argv[0])};
        HiemaliaRef gref(game);
        game.args(args);
        external = HiemaliaExternal(gref);
        game.run();
    } catch (std::exception &e) {
        external = nullptr;
        LOG_FAIL(fail_exception_(e));
        fail(e.what());
    } catch (...) {
        external = nullptr;
        LOG_FAIL(fail_exception_u_());
        fail("<unknown exception>");
    }
    external = nullptr;
    if (keepConsoleOpen) sysCloseConsole();
    return EXIT_SUCCESS;
}

}  // namespace hiemalia

extern "C" int hiemalia_tryAddCredits(int c) {
    return hiemalia::external.addCredits(c);
}

int main(int argc, char *argv[]) { return hiemalia::main(argc, argv); }
