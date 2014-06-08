#pragma once

#include <SDL/SDL.h>
#include "core/corefwd.h"
#include "core/noncopyable.h"
#include "core/input/mouse.h"
#include "core/utils/utils.h"

namespace atom {

enum class Key {
  KEY_UNKNOWN = 0,
  KEY_0,
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,
  KEY_A,
  KEY_B,
  KEY_C,
  KEY_D,
  KEY_E,
  KEY_F,
  KEY_G,
  KEY_H,
  KEY_I,
  KEY_J,
  KEY_K,
  KEY_L,
  KEY_M,
  KEY_N,
  KEY_O,
  KEY_P,
  KEY_Q,
  KEY_R,
  KEY_S,
  KEY_T,
  KEY_U,
  KEY_V,
  KEY_X,
  KEY_Y,
  KEY_Z,
  KEY_W,
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_ENTER,
  KEY_ESCAPE,
  KEY_BACKSPACE,
  KEY_TAB,
  KEY_INSERT,
  KEY_DELETE,
  KEY_HOME,
  KEY_END,
  KEY_PAGEUP,
  KEY_PAGEDOWN,
  KEY_LSHIFT,
  KEY_RSHIFT,
  KEY_LCTRL,
  KEY_RCTRL,
  KEY_LALT,
  KEY_RALT,
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_F11,
  KEY_F12,
  KEY_NUM_0,
  KEY_NUM_1,
  KEY_NUM_2,
  KEY_NUM_3,
  KEY_NUM_4,
  KEY_NUM_5,
  KEY_NUM_6,
  KEY_NUM_7,
  KEY_NUM_8,
  KEY_NUM_9,
  KEY_LMB,
  KEY_MMB,
  KEY_RMB,
  COUNT
//  KEY_,
};

enum class Axis {
  LX,
  LY,
  RY,
  RX,
  COUNT
};

enum class Button {
  TRIANGLE,
  SQUARE,
  CIRCLE,
  CROSS,
  L1,
  L2,
  L3,
  R1,
  R2,
  R3,
  LEFT,
  RIGHT,
  UP,
  DOWN,
  COUNT
};

class Controller {
public:
  Controller();

  float axis(Axis axis) const
  {
    return my_axis[to_size(axis)];
  }

  float button(Button btn) const
  {
    return my_buttons[to_size(btn)];
  }

  bool is_button_pressed(Button btn) const
  {
    return button(btn) > 0.5;
  }

  void set_axis(Axis axis, float value)
  {
    my_axis[to_size(axis)] = value;
  }

  void set_button(Button button, float value)
  {
    my_buttons[to_size(button)] = value;
  }

private:
  float my_axis[to_size(Axis::COUNT)];
  float my_buttons[to_size(Button::COUNT)];
};

struct ButtonEvent {
  Button button;
  float  value;
};

struct AxisEvent {
  Axis  axis;
  float value;
};

struct KeyEvent {
  Key  key;
  bool pressed;
};

struct MouseEvent {
  f32 x;
  f32 y;
};

struct Event {
  enum class Type {
    AXIS,
    BUTTON,
    KEY,
    MOUSE
  };

  Type type;
  union {
    AxisEvent   axis;
    ButtonEvent button;
    KeyEvent    key;
    MouseEvent  mouse;
  };
};

class Mouse {
public:
  Vec2f position;
  Vec2f delta;
};

Event make_key_event(Key key, bool pressed);

Event make_mouse_event(f32 x, f32 y);

typedef std::vector<Event> EventQueue;

class InputService : private NonCopyable {
public:
  InputService();

  ~InputService();

  void poll();

  const Mouse& mouse() const
  { return my_mouse; }

  const Controller& controller() const
  { return my_controller; }

  /**
   * Zisti ci od predchadzajuceho volania poll bola stlacena dana klavesa. Tato metoda zistuje len
   * zmenu stavu stlacenia klavesy.
   */
  bool is_key_up(Key key);

  /**
   * Zisti ci od predchadzajuceho volania poll bola uvolnena dana klavesa. Tato metoda zistuje len
   * zmenu stavu stlacenia klavesy.
   */
  bool is_key_down(Key key);

  /**
   * Zisti ci je prave teraz stlacena dana klavesa.
   */
  bool is_key_pressed(Key key);

  void center_mouse();

  void push_event(const Event &e);

private:
  void process_sdl_events();
  void process_event_queue();
  void process_axis_event(const Event &e);
  void process_button_event(const Event &e);
  void process_key_event(const Event &e);
  void process_mouse_event(const MouseEvent &e);


  void clear_key_state();

  struct KeyState {
    bool pressed : 1;
    bool up      : 1;
    bool down    : 1;
  };

public:
  Mouse         my_mouse;
  Controller    my_controller;
  EventQueue    my_event_queue;
  SDL_Joystick *my_joysticks[2];
  KeyState      my_keys[to_size(Key::COUNT)]; ///< stav vsetkych klaves
};

}
