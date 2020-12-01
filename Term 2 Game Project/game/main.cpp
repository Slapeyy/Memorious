#include "stdafx.h"
#include "MyGame.h"

CMyGame game;
CGameApp app;

int main(int argc, char* argv[])
{
	//app.DebugMemoryLeaks();
	//app.OpenWindow(1920, 1080, "My Game!");
	app.OpenFullScreen(1920, 1080, 24);
	//app.OpenConsole();
	app.SetClearColor(CColor::Red());
	app.Run(&game);
	return(0);
}
