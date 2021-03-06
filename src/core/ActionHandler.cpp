#include "ActionHandler.hpp"
#include "stdafx.hpp"

#include "core/GearsEngine.hpp"
#include "misc/conversions.hpp"

namespace ge {

using namespace sf;
using namespace thor;

ActionHandler::ActionHandler()
{
    /// BASIC EVENTS

    // Quit
    m_actions[act::Quit] = Action(Event::Closed);
    m_events.connect0(act::Quit, std::bind(&GearsEngine::onQuit, std::ref(GE_INST)));

    // Resize
    m_actions[act::Resize] = Action(Event::Resized);
    m_events.connect0(act::Resize, std::bind(&GearsEngine::onResize, std::ref(GE_INST)));

    /// KEY NAVIGATION

    // define keys that can be holded down
    m_holdableKeys.insert(HK_UP);
    m_holdableKeys.insert(HK_DOWN);
    m_holdableKeys.insert(HK_LEFT);
    m_holdableKeys.insert(HK_RIGHT);


    for(int i = 0; i < HK_COUNT; ++i) {
        HotkeyId id = static_cast<HotkeyId>(i);

        updateHotkey(id, GE_INST.settings().keys.at(id).getBoth());

        GE_INST.settings().keys.at(id).getProperty().on_change().connect(
            [this, id](HotkeyPair hotkeys) { updateHotkey(id, hotkeys); });
    }


    // Enter / Accept
    m_actions[act::Enter] = Action(Keyboard::Return, Action::ReleaseOnce);
    m_events.connect0(act::Enter, std::bind(&GearsEngine::onEnter, std::ref(GE_INST)));

    // Escape / Back
    m_actions[act::Escape] = Action(Keyboard::Escape, Action::ReleaseOnce);
    m_events.connect0(act::Escape, std::bind(&GearsEngine::onEscape, std::ref(GE_INST)));

    // Direction keys
    m_actions[act::NavUp]    = realtimeAction([this]() { return m_keyMap.isActive(ge::HK_UP); });
    m_actions[act::NavDown]  = realtimeAction([this]() { return m_keyMap.isActive(ge::HK_DOWN); });
    m_actions[act::NavLeft]  = realtimeAction([this]() { return m_keyMap.isActive(ge::HK_LEFT); });
    m_actions[act::NavRight] = realtimeAction([this]() { return m_keyMap.isActive(ge::HK_RIGHT); });

    m_events.connect(act::NavUp, std::bind(&GearsEngine::onNav, std::ref(GE_INST), ge::UP));
    m_events.connect(act::NavDown, std::bind(&GearsEngine::onNav, std::ref(GE_INST), ge::DOWN));
    m_events.connect(act::NavLeft, std::bind(&GearsEngine::onNav, std::ref(GE_INST), ge::LEFT));
    m_events.connect(act::NavRight, std::bind(&GearsEngine::onNav, std::ref(GE_INST), ge::RIGHT));

    /// MOUSE NAVIGATION

    m_actions[act::MouseMove]    = Action(Event::MouseMoved);
    m_actions[act::MouseSelect]  = Action(sf::Mouse::Left);
    m_actions[act::MouseCommand] = Action(sf::Mouse::Right);
    m_actions[act::MousePan]     = Action(sf::Mouse::Right) && m_actions[act::MouseMove];
    m_actions[act::MouseScroll]  = Action(Event::MouseWheelScrolled);

    m_events.connect(act::MouseMove,
                     [](const act::Context& context) { GE_INST.onMouseMove(context); });
    m_events.connect(act::MouseSelect,
                     [](const act::Context& context) { GE_INST.onMouseSelect(context); });
    m_events.connect(act::MouseCommand,
                     [](const act::Context& context) { GE_INST.onMouseCommand(context); });
    m_events.connect(act::MousePan,
                     [](const act::Context& context) { GE_INST.onMousePan(context); });
    m_events.connect(act::MouseScroll,
                     [](const act::Context& context) { GE_INST.onMouseScroll(context); });
}


ActionHandler::~ActionHandler()
{
}

void ActionHandler::clearEvents()
{
    m_actions.clearEvents();
}

void ActionHandler::pushEvent(const sf::Event& event)
{
    m_actions.pushEvent(event);
}

void ActionHandler::invokeCallbacks(sf::Window* window)
{
    m_actions.invokeCallbacks(m_events, window);
}

void ActionHandler::updateHotkey(ge::HotkeyId id, ge::HotkeyPair keys)
{
    Action::ActionType type;

    if(m_holdableKeys.find(id) != m_holdableKeys.end())
        type = Action::Hold;
    else
        type = Action::ReleaseOnce;

    m_keyMap[id] = toAction(keys, type);
}

} // namespace ge
