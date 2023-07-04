#include "Input.h"
#include "Application.h"
#include "core/log/PLog.h"
namespace pengine
{
	unsigned int Input::keyPressed = 0;
	unsigned int Input::keyReleased = 0;
	unsigned int Input::keyHeld = 0;
	int Input::numKeysHeld = 0;
	unsigned int Input::mouseReleased = 0;
	unsigned int Input::mouseHeld = 0;
	unsigned int Input::mouseClicked = 0;
	int Input::numButtonsHeld = 0;
	glm::vec2 Input::mousePosition = { -1.0f,-1.0f };
	glm::vec2 Input::mouseMotion = { 0.0f,0.0f }; 
	glm::vec2 Input::scrollOffset = { 0.0f,0.0f };
	bool Input::mouseMoving = false;
	bool Input::get_key(int key)
	{
		unsigned int keyMask = 1 << key;
		return (keyHeld & keyMask) != 0;
	}

	bool Input::get_key_pressed(int key)
	{
		unsigned int keyMask = 1 << key;
		return (keyPressed & keyMask) != 0;
	}

	void Input::set_key_pressed(int key)
	{
		if (!get_key(key))
		{
			unsigned int keyMask = 1 << key;
			keyPressed |= keyMask;
			keyHeld |= keyMask;
			++numKeysHeld;
		}
		else
		{
			PLOGE("wrong event : click a held key !");
		}
	}

	bool Input::get_key_released(int key)
	{
		unsigned int keyMask = 1 << key;
		return (keyReleased & keyMask) != 0;
	}

	void Input::set_key_released(int key)
	{
		if (get_key(key))
		{
			unsigned int keyMask = 1 << key;
			keyReleased |= keyMask;
			keyHeld &= ~keyMask;
			--numKeysHeld;
		}
		else
		{
			PLOGE("wrong event : release a unheld key !");
		}	
	}

	bool Input::get_mouse(int mouse)
	{
		unsigned int keyMask = 1 << mouse;
		return (mouseHeld & keyMask) != 0;
	}

	bool Input::get_mouse_clicked(int mouse)
	{
		unsigned int keyMask = 1 << mouse;
		return (mouseClicked & keyMask) != 0;
	}

	void Input::set_mouse_clicked(int mouse)
	{
		
		if (!get_mouse(mouse))
		{
			unsigned int keyMask = 1 << mouse;
			mouseClicked |= keyMask;
			mouseHeld |= keyMask;
			++numKeysHeld;
		}
		else
		{
			PLOGE("wrong event : click a held mouse !");
		}
	}

	bool Input::get_mouse_released(int mouse)
	{
		unsigned int keyMask = 1 << mouse;
		return (mouseReleased & keyMask) != 0;
	}

	void Input::set_mouse_released(int mouse)
	{

		if (get_mouse(mouse))
		{
			unsigned int keyMask = 1 << mouse;
			mouseReleased |= keyMask;
			mouseHeld &= ~keyMask;
			--numKeysHeld;
		}
		else
		{
			PLOGE("wrong event : release a unheld key !");
		}
	}

	void Input::set_mouse_move(float x, float y)
	{
		glm::vec2 newPos = { x,y };
		if (mousePosition.x >= 0 && mousePosition.y >= 0)
			mouseMotion = newPos - mousePosition;
		mousePosition = newPos;
		mouseMoving = true;
	}

	void Input::set_mouse_scroll(float xOffset, float yOffset)
	{
		scrollOffset = { xOffset ,yOffset };
	}

	void Input::reset_state()
	{
		keyPressed = 0;
		keyReleased = 0;
		mouseClicked = 0;
		mouseReleased = 0;
		mouseMoving = false;
		scrollOffset.x = 0;
		scrollOffset.y = 0;
	}
};