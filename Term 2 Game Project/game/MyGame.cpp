#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) : 
	player(200, 200, "Single Neuron.png", 0),
	enemy(960, -150, "Enemy Neuron.png", 0), // Need code to delete this sprite from the first level, for now its existing outside game screen
	startScreen(960, 540, "Title Screen.png", 0)
	// to initialise more sprites here use a comma-separated list
{	
	spawnCap = 10;
	orbCap = 2;
	player.SetOmega(rand() % 20 - 10);
	enemy.SetOmega(rand() % 20 - 10);
}

CMyGame::~CMyGame(void)
{
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	if (IsMenuMode()) return;
	
	Uint32 t = GetTime();

	player.Update(t);	// this will update the sample player sprite
	enemy.Update(t);	
	background.Update(t);
	
	for (CSprite* neuron : neurons) neuron->Update(t);
	for (CSprite* pReplacement : replacementList) pReplacement->Update(t);
	for (CSprite* orb : orbsList) orb->Update(t);
	for (CSprite* pShot : shots) pShot->Update(t);
	
	if (neurons.size() == 0 && level == 1)
	{

		level = 2;
		numberofpoints = 0;
		SetupLevel2();
	}

	for (CSprite* shot : shots)
	{
		if (shot->GetY() > 1080 || shot->GetX() > 1920) shot->Delete();
		if (shot->HitTest(orb, 1))
		{
			orb->Delete();
			shot->Delete();
			//score++;
			orbCollected.Play("soul.wav");
		}
	}
	shots.remove_if(deleted);

}

void CMyGame::enemyControl()
{

}

void CMyGame::OnDraw(CGraphics* g)
{
	if (IsMenuMode())
	{
		startScreen.Draw(g);
		return;
	}
	background.Draw(g);	

	for (int n = 0; n < numberofpoints / 2; n++)
	{
		g->DrawLine(points[2*n], points[(2*n)+1], 8, CColor::Magenta()); // Previous code: g->DrawLine(player.GetPos(), GetMouseCoords(), 8, CColor::Red());
	}
	
	enemy.Draw(g);
	player.Draw(g);	

	for (CSprite* shot : shots) shot->Draw(g);
	for (CSprite* pReplacement : replacementList) pReplacement->Draw(g);
	for (CSprite* neuron : neurons) neuron->Draw(g);
	for (CSprite* orb : orbsList) orb->Draw(g);

	// if is game over
	if (IsGameOverMode())
	{
		*g << center << vcenter << font(50) << "Game Over";
	}
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	background.LoadImage("background.jpg", "map1", CColor::Red());
	background.SetImage("map1"); background.SetPosition(960, 540);
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{   
	level = 1;	
	worldMusic.Play("Music.wav");
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{	
	neurons.clear();

	if (level == 1) SetupLevel1();
	if (level == 2) SetupLevel2();
}

void CMyGame::SetupLevel1()
{
	
	player.SetPosition(200, 200);
	CSprite* neuron = new CSprite(1300, 600, "Neutral Neuron.png", 0);
	neurons.push_back(neuron);
	neuron->SetOmega(rand() % 20 - 10);
	
	neuron = new CSprite(800, 300, "Neutral Neuron.png", 0);
	neurons.push_back(neuron);
	neuron->SetOmega(rand() % 20 - 10);

	startPoint = player.GetPosition();


}

void CMyGame::SetupLevel2()
{
	neurons.clear();
	replacementList.clear();
	shots.clear();
	
	startPoint = player.GetPosition();

	enemy.SetPosition(960, 540);
	
	for (int n = 0; n < 100; n++)
	{
		if (neurons.size() < spawnCap)
		{
			CSprite* neuron = new CSprite(float(100 + rand() % 1750), float(100 + rand() % 850), "Neutral Neuron.png", GetTime()); //Remove sprite dimensions to remove unwanted black box from background
			neuron->SetOmega(rand() % 20 - 10);
			for (CSprite* neuro : neurons)
			{
				if (neuro->HitTest(neuron, 1) || (player.HitTest(neuron, 1) || (enemy.HitTest(neuron, 1))))
				{
					delete neuron; neuron = 0;
					break;
				}
			}
			if (neuron > 0) neurons.push_back(neuron);
		}
	}

	for (int n = 0; n < 100; n++)
	{

		if (orbsList.size() < orbCap)
		{
			CSprite* orb = new CSprite(float(100 + rand() % 1750), float(100 + rand() % 850), "Memory orb.bmp", CColor::Black(), GetTime()); //Remove sprite dimensions to remove unwanted black box from background
			orb->SetOmega(rand() % 10 - 5);
			
			for (CSprite* neuro : neurons)
			{
				if (neuro->HitTest(orb, 1) || (player.HitTest(orb, 1)) || (enemy.HitTest(orb, 1)))
				{
					delete orb; orb = 0;
					break;
				}
			}

			if (orb > 0)
			{

				for (CSprite* orbs : orbsList)
				{
					if (orb->HitTest(orbs, 1))
					{
						delete orb; orb = 0;
						break;
					}
				}
				if (orb > 0) orbsList.push_back(orb);
			}
		}
	}

}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{	
}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();	
	if (sym == SDLK_ESCAPE)
		StopGame();
	
	if (IsMenuMode())
	{
		if (sym == SDLK_1)
		{
			level = 1;
		}
		if (sym == SDLK_2)
		{
			level = 2;
		}
	}

}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{

}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	if (IsMenuMode()) {
		StartGame(); return;
	}

	for (CSprite* neuro : neurons)
	{
		if (neuro->HitTest(x, y))
		{
			points[numberofpoints++] = startPoint;
			points[numberofpoints++] = neuro->GetPosition();

			neuro->Delete();

			// add new player neuron to player neuron list at the position of the current neuron
			CSprite* pReplacement = new CSprite(neuro->GetPosition(), "Single Neuron.png", 0);
			replacementList.push_back(pReplacement);
			pReplacement->SetOmega(rand() % 20 - 10); // I've attempted to debug this so that it retrieves the omega set for the neurons but without success
			startPoint = neuro->GetPosition();
			bubbles.Play("Bubbles.wav");

			//CSprite* newShot = new CSprite(neuro->GetPosition(), "shot.bmp",  CColor::White(), 0);
			//newShot->SetMotion(neuro->GetPosition());
			//shots.push_back(newShot);
		}

	}
	neurons.remove_if(deleted);
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
	
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
