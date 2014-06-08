#pragma once

#include <Box2D/Dynamics/b2Fixture.h>

namespace atom {
namespace utils {

inline bool is_fixture_in_contact(const b2Contact *contact, const b2Fixture *fixture)
{
  assert(contact != nullptr);
  assert(fixture != nullptr);

  return contact->GetFixtureA() == fixture || contact->GetFixtureB() == fixture;
}

inline bool collide(const b2Contact *contact, const b2Body *body1, const b2Body *body2)
{
  assert(contact != nullptr);
  assert(body1 != nullptr);
  assert(body2 != nullptr);
  return
    (contact->GetFixtureA()->GetBody() == body1 && contact->GetFixtureB()->GetBody() == body2) ||
    (contact->GetFixtureA()->GetBody() == body2 && contact->GetFixtureB()->GetBody() == body1);
}

inline bool is_body_in_contact(const b2Contact *contact, const b2Body *body)
{
  assert(contact != nullptr);
  assert(body != nullptr);
  return (contact->GetFixtureA()->GetBody() == body || contact->GetFixtureB()->GetBody() == body);
}

}
}
