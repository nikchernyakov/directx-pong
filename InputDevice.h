#pragma once
#include <queue>
#include <bitset>

class InputDevice
{
	friend class Game;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event()
			:
			type(Type::Invalid),
			code(0u)
		{}
		Event(Type type, unsigned char code) noexcept
			:
			type(type),
			code(code)
		{}
		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}
		unsigned char GetCode() const noexcept
		{
			return code;
		}
	};
public:
	InputDevice() = default;
	InputDevice(const InputDevice&) = delete;
	InputDevice& operator=(const InputDevice&) = delete;

	bool KeyIsPressed(unsigned char keycode) const noexcept;
	Event ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;

	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;

	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;

	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
private:
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;
};