#include "Home.h"
#include "inc.h"
#include <msclr/marshal.h>
using namespace msclr::interop;
using namespace AutoMaple;
HWND hwnd;
void GUIWork()
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew Home);
	Application::Exit();
}
void Main(void)
{
	GUIWork(); // create new instance of managed application
}
HANDLE h = NULL;
void clean() {
	if(h != NULL) {
		CloseThread(h);
		h = NULL;
	}
	if (L != NULL) {
		lua_close(L);
		L = NULL;
	}
	Hacks::Reset();
}
#define LEN 32768
void PathFromTextbox(char buf[LEN], TextBox ^ t) {
	marshal_context ^ context = gcnew marshal_context();
	const char* str = context->marshal_as<const char*>(t->Text);
	GetFilePathExe(buf, str, LEN);
	delete context;
}
System::Void Home::button1_Click(System::Object^  sender, System::EventArgs^  e) {
	clean();
	//Hacks::HookFrame();
	char buf[LEN];
	PathFromTextbox(buf, textBox1);
	h = OpenThreadArg(initLua, buf);
}
System::Void Home::button2_Click(System::Object^  sender, System::EventArgs^  e) {
	clean();
}
System::Void Home::Home_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
	clean();
}
System::Void Home::Home_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
	#ifndef WIN
		FreeLibraryAndExitThread(mod, 0);
	#endif
}
System::Void Home::Home_Load(System::Object^  sender, System::EventArgs^  e) {
	MShwnd();
	Hacks::HookFrame();
	//timer1->Start();
}
System::Void Home::timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
	RECT rect;
	GetWindowRect(hwnd, &rect);
	Point * p = new Point();
	p->X = rect.left - this->Size.Width;
	p->Y = rect.top;
	this->Location = *p;
}
System::Void Home::button3_Click(System::Object^  sender, System::EventArgs^  e) {
	char buf[LEN];
	PathFromTextbox(buf, textBox1);
	ShellExecuteA(NULL, NULL, buf, NULL, NULL, SW_SHOWNORMAL);
}