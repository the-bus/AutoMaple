#include "Home.h"
#include "inc.h"
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
void setDefaults() {

}
void end() {
	Hacks::DisableAutoPortal();
	Hacks::UnHookMove();
	Hacks::UnHookSP();
	Hacks::ResetKeys();
	//Hacks::UnHookFrame();
	setDefaults();
}
void clean() {
	if(h != NULL) {
		CloseThread(h);
		h = NULL;
	}
	if (L != NULL) {
		lua_close(L);
		L = NULL;
	}
	end();
}
System::Void Home::button1_Click(System::Object^  sender, System::EventArgs^  e) {
	clean();
	//Hacks::HookFrame();
	h = OpenThread(initLua);
}
System::Void Home::button2_Click(System::Object^  sender, System::EventArgs^  e) {
	clean();
}
System::Void Home::Home_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
	clean();
}
System::Void Home::Home_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
	FreeLibraryAndExitThread(mod, 0);
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