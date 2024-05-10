#ifndef INPUT_H_
#define INPUT_H_
#include <SDL.h>
#include <SDL_Image.h>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <set>
#include "Vector2.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

//List of possible events
//Anything that is inherited from InputReactor can subscribe to them
enum class EVENTS
{
	QUIT,
	UP_P1,
	RIGHT_P1,
	DOWN_P1,
	LEFT_P1,
	START_P1,
	SELECT_P1,
	A_P1,
	B_P1,
	C_P1,
	S_P1,
	UP_P2,
	RIGHT_P2,
	DOWN_P2,
	LEFT_P2,
	START_P2,
	SELECT_P2,
	A_P2,
	B_P2,
	C_P2,
	S_P2,
	FN1,
	FN2,
	FN3,
	LMB,
	RMB,
	MOUSE_MOVEMENT,
	WHEEL_SCROLL,
	NONE
};

class InputReactor;

typedef InputReactor* subscriber;

class InputSystem
{
public:
	InputSystem();
	void handleInput();
	void subscribe(EVENTS ev_, subscriber sub_);
	void unsubscribe(EVENTS ev_, subscriber sub_);

private:
	void updateAxis(EVENTS ev_, float scale_);
	void send(EVENTS ev_, float val_);
	std::vector<subscriber> m_subscribers[(int)EVENTS::NONE];
	std::map<SDL_Keycode, EVENTS> KEY_EVENTS = {
		{SDLK_UP, EVENTS::UP_P2},
		{SDLK_w, EVENTS::UP_P1},
		{SDLK_RIGHT, EVENTS::RIGHT_P2},
		{SDLK_d, EVENTS::RIGHT_P1},
		{SDLK_DOWN, EVENTS::DOWN_P2},
		{SDLK_s, EVENTS::DOWN_P1},
		{SDLK_LEFT, EVENTS::LEFT_P2},
		{SDLK_a, EVENTS::LEFT_P1},
		{SDLK_RETURN, EVENTS::START_P1},
		{SDLK_BACKSPACE, EVENTS::SELECT_P1},
		{SDLK_i, EVENTS::A_P1},
		{SDLK_z, EVENTS::A_P2},
		{SDLK_o, EVENTS::B_P1},
		{SDLK_x, EVENTS::B_P2},
		{SDLK_l, EVENTS::C_P1},
		{SDLK_c, EVENTS::C_P2},
		{SDLK_k, EVENTS::S_P1},
		{SDLK_v, EVENTS::S_P1},
		{SDLK_MINUS, EVENTS::FN1},
		{SDLK_EQUALS, EVENTS::FN2},
		{SDLK_INSERT, EVENTS::FN3}
	};
};


class InputReactor
{
public:
	//Input reactor needs pointer to  InputSystem
	InputReactor(InputSystem *input_) :
		m_input(input_)
	{
	}

	virtual void receiveInput(EVENTS event, const float scale_);
	virtual void receiveMouseMovement(const Vector2<float> &offset_, const Vector2<float> &pos_);

	//All events reactor is subscribed at
	std::set<EVENTS> subscribed_at;

	void setInputEnabled(bool inputEnabled_);
	bool isInputEnabled();

	//InputReactor automatically removes itself from subscribers
	virtual ~InputReactor()
	{
		while(!subscribed_at.empty())
		{
			unsubscribe(*subscribed_at.begin());
		}
	}

protected:
	void subscribe(EVENTS ev_)
	{
		if (subscribed_at.contains(ev_))
			return;
		
		m_input->subscribe(ev_, this);
		subscribed_at.insert(ev_);
	}

	void unsubscribe(EVENTS ev_)
	{
		if (!subscribed_at.contains(ev_))
			return;
		
		m_input->unsubscribe(ev_, this);
		subscribed_at.erase(ev_);
	}

	void unsubscribeFromAll()
	{
		while(!subscribed_at.empty())
		{
			unsubscribe(*subscribed_at.begin());
		}
	}

	InputSystem * m_input;
	bool m_inputEnabled = false;

};

#endif