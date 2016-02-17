#ifndef MESSAGE_H
#define MESSAGE_H

#include "GlobalMessage.h"
#include <QString>

/**
 * Message namespace. The instance of GlobalMessage is called
 */
namespace Message {

        GlobalMessage::Answer info(const QString& warning, const QString& sender_name=QString());
        GlobalMessage::Answer warning(const QString& warning, const QString& sender_name=QString());
        GlobalMessage::Answer error(const QString& warning, const QString& sender_name=QString());
        GlobalMessage::Answer question_yn(const QString& question, const QString& sender_name=QString());
        GlobalMessage::Answer question_ok(const QString& question, const QString& sender_name=QString());

}


#endif // MESSAGE_H
