#include "InputDevice.h"

bool InputDevice::KeyIsPressed(unsigned char keycode) const noexcept
{
	return keystates[keycode];
}

InputDevice::Event InputDevice::ReadKey() noexcept
{
	if (keybuffer.size() > 0u)
	{
		InputDevice::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	return {};
}

bool InputDevice::KeyIsEmpty() const noexcept
{
	return keybuffer.empty();
}

char InputDevice::ReadChar() noexcept
{
	if (charbuffer.size() > 0u)
	{
		unsigned char charcode = charbuffer.front();
		charbuffer.pop();
		return charcode;
	}
	else
	{
		return 0;
	}
}

bool InputDevice::CharIsEmpty() const noexcept
{
	return charbuffer.empty();
}

void InputDevice::FlushKey() noexcept
{
	keybuffer = std::queue<Event>();
}

void InputDevice::FlushChar() noexcept
{
	charbuffer = std::queue<char>();
}

void InputDevice::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void InputDevice::EnableAutorepeat() noexcept
{
	autorepeatEnabled = true;
}

void InputDevice::DisableAutorepeat() noexcept
{
	autorepeatEnabled = false;
}

bool InputDevice::AutorepeatIsEnabled() const noexcept
{
	return autorepeatEnabled;
}

void InputDevice::OnKeyPressed(unsigned char keycode) noexcept
{
	keystates[keycode] = true;
	keybuffer.push(InputDevice::Event(InputDevice::Event::Type::Press, keycode));
	TrimBuffer(keybuffer);
}

void InputDevice::OnKeyReleased(unsigned char keycode) noexcept
{
	keystates[keycode] = false;
	keybuffer.push(InputDevice::Event(InputDevice::Event::Type::Release, keycode));
	TrimBuffer(keybuffer);
}

void InputDevice::OnChar(char character) noexcept
{
	charbuffer.push(character);
	TrimBuffer(charbuffer);
}

void InputDevice::ClearState() noexcept
{
	keystates.reset();
}

template<typename T>
void InputDevice::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}