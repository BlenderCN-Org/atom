#include "input_service.h"

#include "math.h"
#include "log.h"

namespace atom {

Key sdl_key_to_key(unsigned sdl_key)
{
  switch (sdl_key) {
    case SDLK_0:          return Key::KEY_0;
    case SDLK_1:          return Key::KEY_1;
    case SDLK_2:          return Key::KEY_2;
    case SDLK_3:          return Key::KEY_3;
    case SDLK_4:          return Key::KEY_4;
    case SDLK_5:          return Key::KEY_5;
    case SDLK_6:          return Key::KEY_6;
    case SDLK_7:          return Key::KEY_7;
    case SDLK_8:          return Key::KEY_8;
    case SDLK_9:          return Key::KEY_9;
    case SDLK_a:          return Key::KEY_A;
    case SDLK_b:          return Key::KEY_B;
    case SDLK_c:          return Key::KEY_C;
    case SDLK_d:          return Key::KEY_D;
    case SDLK_e:          return Key::KEY_E;
    case SDLK_f:          return Key::KEY_F;
    case SDLK_g:          return Key::KEY_G;
    case SDLK_h:          return Key::KEY_H;
    case SDLK_i:          return Key::KEY_I;
    case SDLK_j:          return Key::KEY_J;
    case SDLK_k:          return Key::KEY_K;
    case SDLK_l:          return Key::KEY_L;
    case SDLK_m:          return Key::KEY_M;
    case SDLK_n:          return Key::KEY_N;
    case SDLK_o:          return Key::KEY_O;
    case SDLK_p:          return Key::KEY_P;
    case SDLK_q:          return Key::KEY_Q;
    case SDLK_r:          return Key::KEY_R;
    case SDLK_s:          return Key::KEY_S;
    case SDLK_t:          return Key::KEY_T;
    case SDLK_u:          return Key::KEY_U;
    case SDLK_v:          return Key::KEY_V;
    case SDLK_w:          return Key::KEY_W;
    case SDLK_x:          return Key::KEY_X;
    case SDLK_y:          return Key::KEY_Y;
    case SDLK_z:          return Key::KEY_Z;
    case SDLK_UP:         return Key::KEY_UP;
    case SDLK_DOWN:       return Key::KEY_DOWN;
    case SDLK_LEFT:       return Key::KEY_LEFT;
    case SDLK_RIGHT:      return Key::KEY_RIGHT;
    case SDLK_RETURN:     return Key::KEY_ENTER;
    case SDLK_ESCAPE:     return Key::KEY_ESCAPE;
    case SDLK_BACKSPACE:  return Key::KEY_BACKSPACE;
    case SDLK_TAB:        return Key::KEY_TAB;
    case SDLK_INSERT:     return Key::KEY_INSERT;
    case SDLK_DELETE:     return Key::KEY_DELETE;
    case SDLK_HOME:       return Key::KEY_HOME;
    case SDLK_END:        return Key::KEY_END;
    case SDLK_PAGEUP:     return Key::KEY_PAGEUP;
    case SDLK_PAGEDOWN:   return Key::KEY_PAGEDOWN;
    case SDLK_LSHIFT:     return Key::KEY_LSHIFT;
    case SDLK_RSHIFT:     return Key::KEY_RSHIFT;
    case SDLK_LALT:       return Key::KEY_LALT;
    case SDLK_RALT:       return Key::KEY_RALT;
    case SDLK_LCTRL:      return Key::KEY_LCTRL;
    case SDLK_RCTRL:      return Key::KEY_RCTRL;
    case SDLK_F1:         return Key::KEY_F1;
    case SDLK_F2:         return Key::KEY_F2;
    case SDLK_F3:         return Key::KEY_F3;
    case SDLK_F4:         return Key::KEY_F4;
    case SDLK_F5:         return Key::KEY_F5;
    case SDLK_F6:         return Key::KEY_F6;
    case SDLK_F7:         return Key::KEY_F7;
    case SDLK_F8:         return Key::KEY_F8;
    case SDLK_F9:         return Key::KEY_F9;
    case SDLK_F10:        return Key::KEY_F10;
    case SDLK_F11:        return Key::KEY_F11;
    case SDLK_F12:        return Key::KEY_F12;
    case SDLK_KP0:        return Key::KEY_NUM_0;
    case SDLK_KP1:        return Key::KEY_NUM_1;
    case SDLK_KP2:        return Key::KEY_NUM_2;
    case SDLK_KP3:        return Key::KEY_NUM_3;
    case SDLK_KP4:        return Key::KEY_NUM_4;
    case SDLK_KP5:        return Key::KEY_NUM_5;
    case SDLK_KP6:        return Key::KEY_NUM_6;
    case SDLK_KP7:        return Key::KEY_NUM_7;
    case SDLK_KP8:        return Key::KEY_NUM_8;
    case SDLK_KP9:        return Key::KEY_NUM_9;
//    case SDLK_: return Key::KEY_;
  }

  return Key::KEY_UNKNOWN;
}

InputService::InputService()
{
  clear_key_state();
  int joystick_count = SDL_NumJoysticks();
  for (int i = 0; i < joystick_count; ++i) {
    log::debug(DEBUG_INPUT, "Opening joystick %i", i);
    my_joysticks[i] = SDL_JoystickOpen(i);
  }
  SDL_JoystickEventState(SDL_ENABLE);
  center_mouse();
}

InputService::~InputService()
{
}

void InputService::poll()
{
  my_mouse.delta.x = 0;
  my_mouse.delta.y = 0;
  // clear key edge state
//  for (u32 i = 0; i < to_size(Key::COUNT); ++i) {
//    my_keys[i].up = false;
//    my_keys[i].down = false;
//  }

//  if (my_is_mouse_captured) {
//    int mouse_x, mouse_y;
//    SDL_GetMouseState(&mouse_x, &mouse_y);
//    my_mouse.set_dx(mouse_x - config.get_screen_width() / 2);
//    my_mouse.set_dy(mouse_y - config.get_screen_height() / 2);
//  }

//  update_controller();
//  process_sdl_events();
  process_event_queue();

  // vycentruj mys
}

bool InputService::is_key_up(Key key)
{
  return my_keys[to_size(key)].up;
}

bool InputService::is_key_down(Key key)
{
  return my_keys[to_size(key)].down;
}

bool InputService::is_key_pressed(Key key)
{
  return my_keys[to_size(key)].pressed;
}

void InputService::center_mouse()
{
  my_mouse.position = Vec2f(0, 0);
  my_mouse.delta = Vec2f(0, 0);
}

Event make_button_event(Button btn, float value)
{
  Event e;
  e.type = Event::Type::BUTTON;
  e.button.button = btn;
  e.button.value  = value;
  return e;
}

Event make_axis_event(Axis axis, float value)
{
  Event e;
  e.type = Event::Type::AXIS;
  e.axis.axis = axis;
  e.axis.value = value;
  return e;
}

Event make_key_event(Key key, bool pressed)
{
  Event e;
  e.type = Event::Type::KEY;
  e.key.key = key;
  e.key.pressed = pressed;
  return e;
}


Event make_mouse_event(f32 x, f32 y)
{
  Event e;
  e.type = Event::Type::MOUSE;
  e.mouse.x = x;
  e.mouse.y = y;
  return e;
}

void InputService::process_sdl_events()
{
  SDL_Event event;
  // spracuj/preposli jednotlive event-y
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_KEYUP:
      case SDL_KEYDOWN:
//        if (!event.key.repeat) {
        push_event(
          make_key_event(sdl_key_to_key(event.key.keysym.sym), event.type == SDL_KEYDOWN));
//        }
        break;

      case SDL_JOYAXISMOTION:
        if (event.jaxis.axis < to_size(Axis::COUNT))
          push_event(make_axis_event(Axis(event.jaxis.axis), event.jaxis.value / 32768.0));
        break;

      /// @todo mouse events
      case SDL_MOUSEMOTION:
        break;

      case SDL_MOUSEBUTTONDOWN:
        break;

      case SDL_MOUSEBUTTONUP:
        break;

      /// @todo joystick events
//      case SDL_JOYBUTTONUP:
//      case SDL_JOYBUTTONDOWN:
//      case SDL_JOYBALLMOTION:
//      case SDL_JOYHATMOTION:
//        log::info("Joystick event");
//        break;
    }
  }
}

void InputService::process_event_queue()
{
  for (const Event &e : my_event_queue) {
    switch (e.type) {
      case Event::Type::AXIS:
        process_axis_event(e);
        break;

      case Event::Type::BUTTON:
        process_button_event(e);
        break;

      case Event::Type::KEY:
        process_key_event(e);
        break;

      case Event::Type::MOUSE:
        process_mouse_event(e.mouse);
        break;
    }
  }

  my_event_queue.clear();
}

void InputService::process_axis_event(const Event &e)
{
  my_controller.set_axis(e.axis.axis, e.axis.value);
}

void InputService::process_button_event(const Event &e)
{
  my_controller.set_button(e.button.button, e.button.value);
}

void InputService::process_key_event(const Event &e)
{
  KeyState &key_state = my_keys[to_size(e.key.key)];

  if (e.key.pressed) {
    if (!key_state.pressed) {
      key_state.up = false;
      key_state.down = true;
      key_state.pressed = true;
    }
  } else {
    if (key_state.pressed) {
      key_state.up = true;
      key_state.down = false;
      key_state.pressed = false;
    }
  }
}

void InputService::process_mouse_event(const MouseEvent &e)
{
  my_mouse.position.x = clamp(my_mouse.position.x + e.x, -1.0f, 1.0f);
  my_mouse.position.y = clamp(my_mouse.position.y + e.y, -1.0f, 1.0f);
  my_mouse.delta.x = e.x;
  my_mouse.delta.y = e.y;
}

void InputService::push_event(const Event &e)
{
  my_event_queue.push_back(e);
}

void InputService::clear_key_state()
{
  for (uint i = 0; i < to_size(Key::COUNT); ++i) {
    my_keys[i].pressed = false;
    my_keys[i].up = false;
    my_keys[i].down = false;
  }
}

Controller::Controller()
{
  for (unsigned i = 0; i < to_size(Axis::COUNT); ++i) {
    my_axis[i] = 0;
  }

  for (unsigned i = 0; i < to_size(Button::COUNT); ++i) {
    my_buttons[i] = 0;
  }
}

}
