#include <stdafx.h>

#include <Ogre.h>

#include <DataManager.h>
#include <Renderer.h>
#include <InputManager.h>




//using namespace Ogre;

bool Init()
{


    return true;
}

void CleanUp()
{

}

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
        Ogre::LogManager* logMgr = new Ogre::LogManager;
        Ogre::Log *log = Ogre::LogManager::getSingleton().createLog("Khazad Log.log", true, true, false);
        //Ogre::LogManager::getSingletonPtr()->logMessage("NEW LOG MESSAGE");

        // XML Data loading

        printf("-=DATA INITIALIZING=- ... \n");
        DATA->CreateInstance();
        DATA->Init();
        printf("DATA DONE\n");

        // OGRE Renderer
        printf("-=DATA INITIALIZING=- ... \n");
        RENDERER->CreateInstance();
        RENDERER->Init();
        printf("DATA DONE\n");


        //Renderer* RenderEngine = new Renderer();
        //RenderEngine->Init();


        // MyGUI with OGRE Platform
        GUI* GuiManager = new GUI();
        GuiManager->Init(RENDERER->getRoot(), RENDERER->getSceneManager());

        // OIS injection into GUI
        BufferedInputHandler* Input = new BufferedInputHandler();
        Input->Init(RENDERER->getRoot(), GuiManager);

        //return 0; // Errors durring Initialization, Abort



    bool Continue = true;

    printf("Starting timers ...");

    /*
	Timer* FPSTimer = new Timer(20);
    Timer* GameTimer = new Timer(20);
    Timer* UITimer = new Timer(20);
    Timer* RenderTimer = new Timer(20);

    //uint8_t FrameCounter = 0;
    uint32_t FrameRate = 0;
    uint32_t GameRate = 0;

    GameTimer->Start();
    RenderTimer->Start();
	*/

    //RENDERER->WipeScreen();

    printf("Init done. Entering main loop.\n");
	while(Continue) // While program isn't done
	{
		//FPSTimer->Start();

            Continue = Input->frameStarted();

            //GameTimer->Unpause();
                //GAME->Run();
            //GameTimer->Pause();

            //RenderTimer->Unpause();
                //RENDERER->Render();
				RENDERER->RenderFrame();

            //UITimer->Unpause();
                //UI->Draw();
                //UI->PrintFrameRate(FrameRate);
            //UITimer->Pause();

                //RENDERER->CaptureScreenShot();
                //RENDERER->Flip();

            //RenderTimer->Pause();

		//FPSTimer->Pause(); // FrameRate Captures whole loop

        //FrameRate = (uint32_t) FrameRateControl(FPSTimer);
        //GameRate = (uint32_t) GameRateControl(FPSTimer);

	} // Program done, exiting

	CleanUp();

	return 0;
}



/*
void GeomCore::run( void )
{
   bShutdownSignal = false;
   unsigned long lTimeLastFrame = 0;

   mRoot->getRenderSystem()->_initRenderTargets();

   Ogre::LogManager::getSingletonPtr()->logMessage("Starting Geom main routine...");

    while( !bShutdownSignal )
     {
        // Calculate time since last frame and remember current time for next frame
        unsigned long lTimeCurrentFrame = mRoot->getTimer()->getMilliseconds();
        unsigned long lTimeSinceLastFrame = lTimeCurrentFrame - lTimeLastFrame;
        lTimeLastFrame = lTimeCurrentFrame;

        // Update inputmanager
        //mInputMgr->capture();

      Ogre::WindowEventUtilities::messagePump();

        // Render next frame
        if(!mRoot->_fireFrameStarted())
         fireShutdownSignal();

        mRoot->getRenderSystem()->_updateAllRenderTargets();

        if(!mRoot->_fireFrameEnded())
         fireShutdownSignal();
    }
}
*/