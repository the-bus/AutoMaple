#include "Home.h"
#include "inc.h"
#include <msclr/marshal.h>
using namespace msclr::interop;
using namespace AutoMaple;
ref class ManagedGlobals {
	public:
		static Home ^ myHome = nullptr;
		static marshal_context ^ context = gcnew marshal_context();
};
volatile bool logfuncs = true;
void GUIWork()
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	ManagedGlobals::myHome = gcnew Home;
	Application::Run(ManagedGlobals::myHome);
	delete ManagedGlobals::myHome;
	delete ManagedGlobals::context;
	Application::Exit();
}
void Main(void)
{
	GUIWork(); // create new instance of managed application
}
#define LEN 32768
char buf[LEN];
void PathFromTextbox(char buf[LEN], TextBox ^ t) {
	const char* str = ManagedGlobals::context->marshal_as<const char*>(t->Text);
	GetFilePathExe(buf, str, LEN);
}
System::Void Home::button1_Click(System::Object^  sender, System::EventArgs^  e) {
	PathFromTextbox(buf, textBox1);
	//Hacks::HookFrame();
	OpenThreadArg(initLua, buf);
}
System::Void Home::button2_Click(System::Object^  sender, System::EventArgs^  e) {
	Hacks::Interrupt();
	OpenThread(cleanwait);
}
System::Void Home::Home_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
	timer1->Stop();
}
System::Void Home::Home_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
	Hacks::Interrupt();
	OpenThread(cleanwait);
	#ifndef WIN
		FreeLibraryAndExitThread(mod, 0);
	#endif
}
System::Void Home::checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	if (checkBox1->Checked) {
		timer1->Start();
		ShowInTaskbar = false;
	}
	else {
		timer1->Stop();
		ShowInTaskbar = true;
	}
}
System::Void Home::Home_Load(System::Object^  sender, System::EventArgs^  e) {
	#ifndef WIN
		Hacks::HookFrame();
	#endif
}
RECT old;
System::Void Home::timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
	RECT rect;
	HWND foreground = GetForegroundWindow();
	HWND ms = GetMShwnd();
	HWND my = (HWND)Handle.ToPointer();
	if (foreground == ms || foreground == my) {
		ShowWindow(my, SW_SHOWNOACTIVATE);
		SetWindowPos(my, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		GetWindowRect(ms, &rect);
		if (rect.left != old.left || rect.top != old.top) {
			Point * p = new Point();
			p->X = rect.left - this->Size.Width;
			p->Y = rect.top;
			this->Location = *p;
		}
		old = rect;
	}
	else {
		SetWindowPos(my, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		ShowWindow(my, SW_HIDE);
	}
}
System::Void Home::button3_Click(System::Object^  sender, System::EventArgs^  e) {
	char buf[LEN];
	PathFromTextbox(buf, textBox1);
	ShellExecuteA(NULL, NULL, buf, NULL, NULL, SW_SHOWNORMAL);
}
template<typename T1>
ref class CallbackToAction
{
public:
	typedef void (CallbackHandler)(T1);

	// Converts function pointer and parameters to an action. Note that
	// the arguments need to be either pointers, values types, or managed pointers.
	// You cannot pass objects by value.
	static System::Action^ Convert(CallbackHandler* handler, T1 arg1)
	{
		CallbackToAction<T1>^ callbackToAction = gcnew CallbackToAction<T1>();
		callbackToAction->_handler = handler;
		callbackToAction->_arg1 = arg1;
		return callbackToAction->ToAction();
	}

private:

	void DoCallback()
	{
		_handler(_arg1);
	}

	System::Action^ ToAction()
	{
		return gcnew Action(this, &CallbackToAction::DoCallback);
	}

	CallbackHandler* _handler;
	T1 _arg1;
};
System::Void Home::Log(const char * c) {
	while (listBox1->Items->Count > 100)
		listBox1->Items->RemoveAt(listBox1->Items->Count - 1);
	listBox1->Items->Insert(0, gcnew String(c));
}
System::Void Home::checkBox2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	logfuncs = checkBox2->Checked;
}
void DoLog(const char * c) {
	ManagedGlobals::myHome->Log(c);
}
void Log(const char * c) {
	ManagedGlobals::myHome->Invoke(CallbackToAction<const char *>::Convert(&DoLog, c));
}