#pragma once

#include "gargantuan/classes/InputObject.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/reflection/Enums.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

namespace gargantuan {
	G_ENUM(MouseBehavior);
	G_ENUM(PreferredInput);
	G_ENUM(KeyCodeStringFormat);
	G_ENUM(SwipeDirection);

	typedef std::string_view Content;
	typedef std::string_view ContentId;

	class UserInputService : public Instance {
	  public:
		static const ClassDefinition DEFINITION;

		Enums::MouseBehavior MouseBehavior;
		ContentId MouseIcon = "";
		Content MouseIconContent = "";
		bool MouseIconEnabled = false;

		bool KeyboardEnabled = false;
		bool OnScreenKeyboardVisible = false;
		Vector2 OnScreenKeyboardPosition = Vector2();
		Vector2 OnScreenKeyboardSize = Vector2();

		bool TouchEnabled = false;
		bool TouchScreenEnabled = false;

		bool AccelerometerEnabled = false;
		bool GamepadEnabled = false;
		bool GyroscopeEnabled = false;

		// std::shared_ptr<VirtualInput> CreateVirtualInput();
		bool GamepadSupports(Enums::UserInputType num, Enums::KeyCode keyCode);
		std::vector<Enums::UserInputType> GetConnectedGamepads();
		std::shared_ptr<InputObject> GetDeviceAcceleration();
		std::shared_ptr<InputObject> GetDeviceGravity();
		std::tuple<std::shared_ptr<InputObject>, CFrame> GetDeviceRotation();
		// std::shared_ptr<TextBox> GetFocusedTextBox();
		bool GetGamepadConnected(Enums::UserInputType num);
		std::vector<std::shared_ptr<InputObject>> GetGamepadState(Enums::UserInputType num);
		ContentId GetImageForKeyCode(Enums::KeyCode keyCode);
		std::vector<std::shared_ptr<InputObject>> GetKeysPressed();
		Enums::UserInputType GetLastInputType();
		std::vector<std::shared_ptr<InputObject>> GetMouseButtonsPressed();
		Vector2 GetMouseDelta();
		Vector2 GetMouseLocation();
		std::vector<Enums::UserInputType> GetNavigationGamepads();
		std::string GetStringForKeyCode(Enums::KeyCode keyCode, Enums::KeyCodeStringFormat format);
		std::vector<Enums::KeyCode> GetSupportedGamepadKeyCodes(Enums::UserInputType num);
		bool IsGamepadButtonDown(Enums::UserInputType num, Enums::KeyCode keyCode);
		bool IsKeyDown(Enums::KeyCode keyCode);
		bool IsMouseButtonPressed(Enums::UserInputType num);
		bool IsNavigationGamepad(Enums::UserInputType num);
		void SetNavigationGamepad(Enums::UserInputType num, bool enabled);

		typedef std::tuple<std::shared_ptr<InputObject>, CFrame> DeviceRotationChangedSignalType;
		G_SIGNAL(DeviceAccelerationChanged, std::shared_ptr<InputObject>);
		G_SIGNAL(DeviceGravityChanged, std::shared_ptr<InputObject>);
		G_SIGNAL(DeviceRotationChanged, DeviceRotationChangedSignalType);

		G_SIGNAL(GamepadConnected, std::shared_ptr<InputObject>);
		G_SIGNAL(GamepadDisconnected, std::shared_ptr<InputObject>);

		typedef std::tuple<std::shared_ptr<InputObject>, bool> InputSignalType;
		G_SIGNAL(InputBegan, InputSignalType);
		G_SIGNAL(InputChanged, InputSignalType);
		G_SIGNAL(InputEnded, InputSignalType);

		G_SIGNAL(JumpRequest, std::monostate);

		G_SIGNAL(LastInputTypeChanged, Enums::UserInputType);
		typedef std::tuple<float, Vector2, float, bool> PointerActionSignalType;
		G_SIGNAL(PointerAction, PointerActionSignalType);
		// G_SIGNAL(TextBoxFocused, std::shared_ptr<TextBox>);
		// G_SIGNAL(TextBoxReleased, std::shared_ptr<TextBox>);

		typedef std::tuple<Enums::SwipeDirection, int, bool> TouchDragSignalType;
		typedef std::tuple<std::vector<Vector2>, Enums::UserInputState, bool> TouchLongPressSignalType;
		typedef std::tuple<std::vector<Vector2>, Vector2, Vector2, Enums::UserInputState, bool> TouchPanSignalType;
		typedef std::tuple<std::vector<Vector2>, float, float, Enums::UserInputState, bool> TouchSwipeSignalType;
		typedef std::tuple<std::vector<Vector2>, bool> TouchTapSignalType;
		typedef std::tuple<Vector2, bool> TouchTapInWorldSignalType;
		G_SIGNAL(TouchDrag, TouchDragSignalType);
		G_SIGNAL(TouchEnded, InputSignalType);
		G_SIGNAL(TouchLongPress, TouchLongPressSignalType);
		G_SIGNAL(TouchMoved, InputSignalType);
		G_SIGNAL(TouchPan, TouchPanSignalType);
		G_SIGNAL(TouchPinch, TouchSwipeSignalType);
		G_SIGNAL(TouchRotate, TouchSwipeSignalType);
		G_SIGNAL(TouchStarted, InputSignalType);
		G_SIGNAL(TouchTap, TouchTapSignalType);
		G_SIGNAL(TouchTapInWorld, TouchTapInWorldSignalType);

		G_SIGNAL(WindowFocused, std::monostate)
		G_SIGNAL(WindowFocusReleased, std::monostate);
	};
}
