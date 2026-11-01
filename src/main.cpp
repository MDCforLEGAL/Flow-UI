#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>

using namespace geode::prelude;

class $modify(FlowMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto mod = Mod::get();
        float darkness = 0.8f;
        if (mod->hasSetting("darkness")) {
            darkness = mod->getSettingValue<float>("darkness");
        }

        // Modern dark overlay
        auto overlay = CCLayerColor::create(ccc4(5, 5, 8, static_cast<GLubyte>(180 * darkness)));
        this->addChild(overlay, -20);

        // Flow-style custom button
        auto btn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
            this,
            menu_selector(FlowMenuLayer::onFlowButton)
        );
        btn->setID("flow-button"_spr);

        if (auto menu = this->getChildByID("bottom-menu")) {
            menu->addChild(btn);
            menu->updateLayout();
        }

        // Simple continuous soft pulse on the new button
        auto pulse = CCSequence::create(
            CCScaleTo::create(0.9f, 1.08f),
            CCScaleTo::create(0.9f, 1.0f),
            nullptr
        );
        btn->runAction(CCRepeatForever::create(pulse));

        log::info("MDC UI loaded - dark Flow theme active");
        return true;
    }

    void onFlowButton(CCObject*) {
        if (Mod::get()->getSettingValue<bool>("sound-effects")) {
            // Correct way to play a sound effect in Geode / Geometry Dash
            FMODAudioEngine::sharedEngine()->playEffect("click.ogg");
        }
        FLAlertLayer::create(
            "MDC UI",
            "Flow temalı dark arayüz aktif.\nAyarları Geode mod sayfasından değiştir.",
            "Tamam"
        )->show();
    }
};
