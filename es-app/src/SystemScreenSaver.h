#pragma once

#include "Window.h"

class VideoComponent;

// Screensaver implementation for main window
class SystemScreenSaver : public Window::ScreenSaver
{
public:
	SystemScreenSaver(gui::Context&	context);
	virtual ~SystemScreenSaver();

	virtual void startScreenSaver(bool updateBGMusicState = true);
	virtual void stopScreenSaver(bool updateBGMusicState = true);
	virtual void nextVideo();
	virtual void renderScreenSaver();
	virtual bool allowSleep();
	virtual void update(int deltaTime);
	virtual bool isScreenSaverActive();

	virtual FileData* getCurrentGame();
	virtual void launchGame();

private:
	void	countVideos();
	void	pickRandomVideo(std::string& path);

	void input(InputConfig* config, Input input);

	enum STATE {
		STATE_INACTIVE,
		STATE_FADE_OUT_WINDOW,
		STATE_FADE_IN_VIDEO,
		STATE_SCREENSAVER_ACTIVE
	};

private:
	bool			mCounted;
	unsigned long	mVideoCount;
	VideoComponent* mVideoScreensaver;
	Window*			mWindow;
	STATE			mState;
	float			mOpacity;
	int				mTimer;
	FileData*		mCurrentGame;
	std::string		mGameName;
	std::string		mSystemName;	
	gui::Context&	m_context;
	bool			m_wasBackgroundMusicPlaying;
};
