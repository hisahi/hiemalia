/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// msg.hh: message handler

#ifndef M_MSG_HH
#define M_MSG_HH

#include <memory>
#include <vector>

#include "defs.hh"
#include "helpers.hh"
#include "msg.hh"

namespace hiemalia {
template <typename T>
class MessageHandler {
   public:
    using MessageHandlerPtr = MessageHandler<T>*;
    using MessageHandlerPtrList = std::vector<MessageHandlerPtr>;

    MessageHandler() { list_.push_back(this); }
    virtual ~MessageHandler() { erase_remove(list_, this); }

    virtual void gotMessage(const T& msg) = 0;
    void enable() { _enabled = true; }
    void disable() { _enabled = false; }

   private:
    bool _enabled{true};
    static MessageHandlerPtrList list_;
    static void deliver(const T& msg) {
        for (auto& hook : list_)
            if (hook->_enabled) hook->gotMessage(msg);
    }
};

template <typename T>
typename MessageHandler<T>::MessageHandlerPtrList MessageHandler<T>::list_{};
};  // namespace hiemalia

#endif  // M_MSG_HH
