#include "Message.h"


GlobalMessage::Answer Message::info(const QString& info, const QString& sender_name){
        return GlobalMessage::getInstance()->info(info, sender_name);
}

GlobalMessage::Answer Message::warning(const QString& warning, const QString& sender_name){
        return GlobalMessage::getInstance()->warning(warning, sender_name);
}

GlobalMessage::Answer Message::error(const QString& error, const QString& sender_name){
        return GlobalMessage::getInstance()->error(error, sender_name);
}

GlobalMessage::Answer Message::question_yn(const QString& error, const QString& sender_name){
        return GlobalMessage::getInstance()->question(error, sender_name, GlobalMessage::QuestionType::YesNo);
}

GlobalMessage::Answer Message::question_ok(const QString& error, const QString& sender_name){
        return GlobalMessage::getInstance()->question(error, sender_name, GlobalMessage::QuestionType::OkCancel);
}

