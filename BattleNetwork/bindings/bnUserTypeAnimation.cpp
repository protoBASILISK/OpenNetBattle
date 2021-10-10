#ifdef BN_MOD_SUPPORT
#include "bnUserTypeAnimation.h"
#include "../bnScriptResourceManager.h"

void DefineAnimationUserType(sol::state& state, sol::table& engine_namespace) {
  engine_namespace.new_usertype<AnimationWrapper>("Animation",
    sol::factories(
      [] (const std::string& path) {
        auto animation = std::make_shared<Animation>(path);

        auto animationWrapper = AnimationWrapper(animation, *animation);
        animationWrapper.OwnParent();

        return animationWrapper;
      },
      [] (AnimationWrapper& original) {
        auto animation = std::make_shared<Animation>(original.Unwrap());

        auto animationWrapper = AnimationWrapper(animation, *animation);
        animationWrapper.OwnParent();

        return animationWrapper;
      }
    ),
    sol::meta_function::index, []( sol::table table, const std::string key ) { 
      ScriptResourceManager::PrintInvalidAccessMessage( table, "Animation", key );
    },
    sol::meta_function::new_index, []( sol::table table, const std::string key, sol::object obj ) { 
      ScriptResourceManager::PrintInvalidAssignMessage( table, "Animation", key );
    },
    "load", [](AnimationWrapper& animation, const std::string& path) {
      animation.Unwrap().Load(path);
    },
    "update", [](AnimationWrapper& animation, double elapsed, sf::Sprite& target, double playbackSpeed = 1.0) {
      animation.Unwrap().Update(elapsed, target, playbackSpeed);
    },
    "refresh", [](AnimationWrapper& animation, sf::Sprite& target) {
      animation.Unwrap().Refresh(target);
    },
    "copy_from", [](AnimationWrapper& animation, AnimationWrapper& rhs) {
      animation.Unwrap().CopyFrom(rhs.Unwrap());
    },
    "set_state", [](AnimationWrapper& animation, const std::string& state) {
      animation.Unwrap().SetAnimation(state);
    },
    "get_state", [](AnimationWrapper& animation) -> std::string {
      return animation.Unwrap().GetAnimationString();
    },
    "point", [](AnimationWrapper& animation, const std::string& name) -> sf::Vector2f {
      return animation.Unwrap().GetPoint(name);
    },
    "set_playback", [](AnimationWrapper& animation, char mode) {
      animation.Unwrap() << mode;
    },

    // memory leak if animation created from lua is captured in these callback functions
    "on_complete", [] (AnimationWrapper& animation, sol::stack_object callbackObject) {
      sol::protected_function callback = callbackObject;
      animation.Unwrap() << [callback] { callback(); };
    },
    "on_frame", [](AnimationWrapper& animation, int frame, sol::stack_object callbackObject, bool doOnce) {
      sol::protected_function callback = callbackObject;
      animation.Unwrap().AddCallback(frame, [callback] { callback(); }, doOnce);
    },
    "on_interrupt", [](AnimationWrapper& animation, sol::stack_object callbackObject) {
      sol::protected_function callback = callbackObject;
      animation.Unwrap().SetInterruptCallback([callback] { callback(); });
    }
  );

  state.new_enum("Playback",
    "Once", Animator::Mode::NoEffect,
    "Loop", Animator::Mode::Loop,
    "Bounce", Animator::Mode::Bounce,
    "Reverse", Animator::Mode::Reverse
  );
}
#endif