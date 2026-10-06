#include <Events/EventDispatcher.hpp>
#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <memory>
#include <thread>
#include <vector>

// EventDispatcher::init() registers a Lua C function through the LuaFacade
// singleton, so these tests only use register/subscribe/dispatch.

class EventDispatcherTest : public ::testing::Test
{
protected:
  EventDispatcher dispatcher;
};

// ─── Delivery ─────────────────────────────────────────────────

TEST_F(EventDispatcherTest, TwoSubscribersBothReceivePayload)
{
  dispatcher.registerNewEvent(EventIdentifiers::MOUSE_MOVE);

  std::vector<MousePayload> first;
  std::vector<MousePayload> second;
  dispatcher.subscribe(EventIdentifiers::MOUSE_MOVE,
                       [&first](EventIdentifiers e, EventPayload p)
                       {
                         EXPECT_EQ(e, EventIdentifiers::MOUSE_MOVE);
                         ASSERT_TRUE(std::holds_alternative<MousePayload>(p));
                         first.push_back(std::get<MousePayload>(p));
                       });
  dispatcher.subscribe(EventIdentifiers::MOUSE_MOVE,
                       [&second](EventIdentifiers e, EventPayload p)
                       {
                         EXPECT_EQ(e, EventIdentifiers::MOUSE_MOVE);
                         ASSERT_TRUE(std::holds_alternative<MousePayload>(p));
                         second.push_back(std::get<MousePayload>(p));
                       });

  dispatcher.dispatchEvent(EventIdentifiers::MOUSE_MOVE, MousePayload{.x = 12.5, .y = -3.0, .scroll = 1.0});

  ASSERT_EQ(first.size(), 1u);
  ASSERT_EQ(second.size(), 1u);
  EXPECT_DOUBLE_EQ(first[0].x, 12.5);
  EXPECT_DOUBLE_EQ(first[0].y, -3.0);
  EXPECT_DOUBLE_EQ(first[0].scroll, 1.0);
  EXPECT_DOUBLE_EQ(second[0].x, 12.5);
  EXPECT_DOUBLE_EQ(second[0].y, -3.0);
  EXPECT_DOUBLE_EQ(second[0].scroll, 1.0);
}

TEST_F(EventDispatcherTest, SubscribersOnlyReceiveTheirOwnEvent)
{
  dispatcher.registerNewEvent(EventIdentifiers::MOUSE_MOVE);
  dispatcher.registerNewEvent(EventIdentifiers::KEY);

  int mouseCalls = 0;
  int keyCalls   = 0;
  dispatcher.subscribe(EventIdentifiers::MOUSE_MOVE, [&mouseCalls](EventIdentifiers, EventPayload) { mouseCalls++; });
  dispatcher.subscribe(EventIdentifiers::KEY, [&keyCalls](EventIdentifiers, EventPayload) { keyCalls++; });

  dispatcher.dispatchEvent(EventIdentifiers::KEY, KeyboardPayload{.key = 65, .action = 1});

  EXPECT_EQ(mouseCalls, 0);
  EXPECT_EQ(keyCalls, 1);
}

TEST_F(EventDispatcherTest, RegisteredEventWithoutSubscribersIsNoOp)
{
  dispatcher.registerNewEvent(EventIdentifiers::KEY);
  dispatcher.dispatchEvent(EventIdentifiers::KEY, KeyboardPayload{.key = 1, .action = 0});
  SUCCEED();
}

// ─── Unregistered events ──────────────────────────────────────

// BUG: WestEngine/Core/Events/EventDispatcher.cpp:27 `_nameToIdx[name]` default-inserts index 0 for an unregistered event, so its payload is delivered to the subscribers of whichever event was registered first (and with no events registered, `_events[0]` is out of bounds).
TEST_F(EventDispatcherTest, DISABLED_DispatchingUnregisteredEventCallsNobody)
{
  // GAME_EVENT ends up at slot 0, which is what an accidental default index points to
  dispatcher.registerNewEvent(EventIdentifiers::GAME_EVENT);

  int calls = 0;
  dispatcher.subscribe(EventIdentifiers::GAME_EVENT, [&calls](EventIdentifiers, EventPayload) { calls++; });

  dispatcher.dispatchEvent(EventIdentifiers::KEY, KeyboardPayload{.key = 65, .action = 1});

  EXPECT_EQ(calls, 0) << "KEY was never registered, nobody should be notified";
}

TEST_F(EventDispatcherTest, SubscribingBeforeRegisteringDoesNothing)
{
  int calls = 0;
  dispatcher.subscribe(EventIdentifiers::KEY, [&calls](EventIdentifiers, EventPayload) { calls++; });

  dispatcher.registerNewEvent(EventIdentifiers::KEY);
  dispatcher.dispatchEvent(EventIdentifiers::KEY, KeyboardPayload{.key = 65, .action = 1});

  EXPECT_EQ(calls, 0) << "the early subscription must have been rejected, not queued";
}

// ─── Re-entrancy ──────────────────────────────────────────────

// BUG: WestEngine/Core/Events/EventDispatcher.cpp:26 dispatchEvent holds the non-recursive _mutex while invoking subscribers, so a subscriber that dispatches (or subscribes) again locks it a second time and deadlocks the calling thread.
TEST(EventDispatcherReentrancy, DISABLED_DispatchFromInsideCallbackDoesNotDeadlock)
{
  // Runs on a detached thread so a deadlock turns into a timeout failure instead
  // of hanging the suite. The dispatcher is intentionally leaked: a deadlocked
  // thread keeps using it after the test returns. (A std::async future would
  // block in its destructor waiting for the stuck thread.)
  auto* dispatcher = new EventDispatcher();
  dispatcher->registerNewEvent(EventIdentifiers::ACTION_FINISHED);
  dispatcher->registerNewEvent(EventIdentifiers::GAME_EVENT);

  auto gameEventSeen = std::make_shared<std::atomic<int>>(0);
  dispatcher->subscribe(EventIdentifiers::GAME_EVENT,
                        [gameEventSeen](EventIdentifiers, EventPayload) { gameEventSeen->fetch_add(1); });
  dispatcher->subscribe(EventIdentifiers::ACTION_FINISHED,
                        [dispatcher](EventIdentifiers, EventPayload)
                        { dispatcher->dispatchEvent(EventIdentifiers::GAME_EVENT, GamePayload{.turn = 1}); });

  auto done          = std::make_shared<std::promise<void>>();
  std::future<void> f = done->get_future();
  std::thread(
    [dispatcher, done]()
    {
      dispatcher->dispatchEvent(EventIdentifiers::ACTION_FINISHED, ActionFinishedPayload{.entityId = 7});
      done->set_value();
    })
    .detach();

  ASSERT_EQ(f.wait_for(std::chrono::seconds(2)), std::future_status::ready)
    << "dispatchEvent deadlocked when called from inside a subscriber";
  EXPECT_EQ(gameEventSeen->load(), 1);
  delete dispatcher;
}
