/* Ekran ladowania */
// istna improwizacja, pewnie bêdzie trzeba sporo rzeczy zmieniæ.
#ifndef _LOADING_SCREENS_H_
#define _LOADING_SCREENS_H_

namespace GUI 
{
    class CLoadingScreen;
};

struct thread_var
{
    void * param; // dowolny parametr
    void (GUI::CLoadingScreen::*ThreadExit) (); // f. która musi byæ wywo³ana na koñcu funkcji 
};

// wa¿ne: zak³adamy i¿ funkcja przyjmuje thread_var i wymagane jest wywo³anie
// na zakoñczeniu thread_var->ThreadExit(); które poinformuje
// "rodzica" i¿ w¹tek zakoñczy³ dzia³anie.
typedef void (*loading_task)(void *);

namespace sf
{
    class Thread;
};

namespace GUI
{
    class CGUIObject;

    class CLoadingScreen
    {
	    public:
		    static void Show();
		    static void Hide();

            /// przekazuje sie funkcje z kodem ktory ma sie wykonywac w tle
            void SetTask(loading_task = 0, void * param = 0, bool start = true);
            /// zwraca true jeœli watek pracuje w tle, w przeciwnym razie false.
            bool DoTask();            

            /// Konczy przy uzyciu brutalnej sily watek
            void Terminate();

		    CLoadingScreen();
		    virtual ~CLoadingScreen();

	    protected:
            static CGUIObject * mLoadScreen;
            loading_task mBackgroundFunction;
            thread_var mThreadVar;
            sf::Thread * mThread;
            bool thread_running;
    private:
        void ThreadExit();
        static void Init();
    };

};

#endif //_LOADING_SCREENS_//
