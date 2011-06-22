/* Ekran ladowania */
// istna improwizacja, pewnie b�dzie trzeba sporo rzeczy zmieni�.
#ifndef _LOADING_SCREENS_H_
#define _LOADING_SCREENS_H_

namespace GUI 
{
    class CLoadingScreen;
};

struct thread_var
{
    void * param; // dowolny parametr
    void (GUI::CLoadingScreen::*ThreadExit) (); // f. kt�ra musi by� wywo�ana na ko�cu funkcji 
};

// wa�ne: zak�adamy i� funkcja przyjmuje thread_var i wymagane jest wywo�anie
// na zako�czeniu thread_var->ThreadExit(); kt�re poinformuje
// "rodzica" i� w�tek zako�czy� dzia�anie.
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
            /// zwraca true je�li watek pracuje w tle, w przeciwnym razie false.
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
