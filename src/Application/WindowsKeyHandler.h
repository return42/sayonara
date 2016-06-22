#ifndef WINDOWSKEYHANDLER_H
#define WINDOWSKEYHANDLER_H

#include <windows.h>
#include <QThread>
class WindowsKeyHandler : public QThread
{
public:
	WindowsKeyHandler(QObject* parent=nullptr);
	void stop();

private:
	bool _may_run;

private:
	void run() override;
	void register_hotkey(UINT modifiers, UINT key);
};

#endif // WINDOWSKEYHANDLER_H
