#include "Home.h"
#include "inc.h"
using namespace AutoMaple;
DWORD HookRet;
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
	if (L != NULL) {
		lua_close(L);
		L = NULL;
	}
	if(h != NULL) {
		CloseThread(h);
		h = NULL;
	}
}
System::Void Home::button1_Click(System::Object^  sender, System::EventArgs^  e) {
	clean();
	h = OpenThread(initLua);
}
System::Void Home::button2_Click(System::Object^  sender, System::EventArgs^  e) {
	clean();
}
System::Void Home::Home_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
	clean();
	FreeLibraryAndExitThread(mod, 0);
}
System::Void Home::Home_Load(System::Object^  sender, System::EventArgs^  e) {
	Hacks::HookFrame();
}