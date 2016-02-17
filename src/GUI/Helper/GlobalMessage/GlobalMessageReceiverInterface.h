#ifndef GLOBALMESSAGERECEIVERINTERFACE_H
#define GLOBALMESSAGERECEIVERINTERFACE_H

#include <QString>
#include "GlobalMessage.h"

/**
 * @brief The GlobalMessageReceiverInterface class\n
 * implement this class in order to have the possibility to show messages
 */
class GlobalMessageReceiverInterface {

private:
        QString _name;

public:
        GlobalMessageReceiverInterface(const QString& name);

        QString get_name() const;


        virtual GlobalMessage::Answer question_received(const QString& info, const QString& sender_name=QString(),GlobalMessage::QuestionType type=GlobalMessage::QuestionType::YesNo )=0;
        virtual GlobalMessage::Answer info_received(const QString& info, const QString& sender_name=QString())=0;
        virtual GlobalMessage::Answer warning_received(const QString& warning, const QString& sender_name=QString())=0;
        virtual GlobalMessage::Answer error_received(const QString& error, const QString& sender_name=QString())=0;
};


#endif // GLOBALMESSAGERECEIVERINTERFACE_H
