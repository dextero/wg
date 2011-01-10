#include "CLoadingScreen.h"
#include "CImageBox.h"
#include "CWindow.h"
#include "CRoot.h"
#include "CGUIObject.h"
#include <SFML/System.hpp>
#undef CreateWindow
#include "CButton.h"

using namespace GUI;

CGUIObject * CLoadingScreen::mLoadScreen = NULL;

CLoadingScreen::CLoadingScreen() : mThread(NULL), thread_running(false)
{
    Init();    
}

CLoadingScreen::~CLoadingScreen()
{
    Terminate();
}

void CLoadingScreen::Init()
{
    if ( mLoadScreen == NULL )
    {
        CWindow * mLoadingScreenBackground = gGUI.CreateWindow("ls-bkg");
        mLoadingScreenBackground->SetPosition(0.f, 0.f, 100.f, 100.f);
        mLoadingScreenBackground->SetBackgroundImage("data/GUI/loading-screen.jpg");

        //TODO: zrobic aby proporcje by³y zachowywane na innych rodzia³kach innego typu jak 4:3
        CWindow * mProgress = mLoadingScreenBackground->CreateWindow("ls-pgs");
        mProgress->SetPosition(30.0f ,20.f , 50.f, 50.f, UNIT_PERCENT, true);

        CImageBox * p1 = mProgress->CreateImageBox("p-ring0");
        p1->AddImageToSequence("data/GUI/loading-screen-p1.png");
        p1->SetPosition(0.f, 0.f, 100.f, 100.f);
        p1->SetRotationSpeed(-40.f);

        CImageBox * p2 = mProgress->CreateImageBox("p-ring1");
        p2->AddImageToSequence("data/GUI/loading-screen-p2.png");
        p2->SetPosition(0.f, 0.f, 100.f, 100.f);
        p2->SetRotationSpeed(70.f);

        CImageBox * p3 = mProgress->CreateImageBox("p-ring2");
        p3->AddImageToSequence("data/GUI/loading-screen-p3.png");
        p3->SetPosition(0.f, 0.f, 100.f, 100.f);
        p3->SetRotationSpeed(-80.f);

        CImageBox * p4 = mProgress->CreateImageBox("p-head");
        p4->AddImageToSequence("data/GUI/loading-screen-p4.png");
        p4->SetPosition(0.f, 0.f, 100.f, 100.f);

        mLoadScreen = (CGUIObject*) mLoadingScreenBackground;
    }
}

void CLoadingScreen::Show()
{
    Init();
    if ( mLoadScreen != NULL)
        mLoadScreen->SetVisible(true);
}

void CLoadingScreen::Hide()
{
    if ( mLoadScreen != NULL)
        mLoadScreen->SetVisible(false);
}

void CLoadingScreen::SetTask(loading_task function, void * params, bool start)
{
    mBackgroundFunction = function;
    mThreadVar.param = params;
    mThreadVar.ThreadExit = &CLoadingScreen::ThreadExit;
    if (start)
        DoTask();
}

void CLoadingScreen::Terminate()
{
    if ( mThread != NULL )
    {
        fprintf( stderr, "Forced to terminate thread...");
        mThread->Terminate();       

        delete mThread;
        mThread = NULL;
    }

    thread_running = false;
}

bool CLoadingScreen::DoTask()
{
    if ( mBackgroundFunction == NULL )
        return false;

    if ( mThread == NULL )
    {
        mThread = new sf::Thread( mBackgroundFunction, &mThreadVar );
        if ( mThread) {
            mThread->Launch();
            thread_running = true;
            Show();
        }
    }
    else if (thread_running == false) {
        delete mThread;
        mThread = NULL;
    }

    return thread_running;
}

void CLoadingScreen::ThreadExit()
{
    thread_running = false;
    Hide();
}
