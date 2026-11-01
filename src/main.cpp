#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>

using namespace geode::prelude;

// ===== MAIN MENU =====
class $modify(FlowMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto mod = Mod::get();
        float darkness = 0.85f;
        if (mod->hasSetting("darkness")) {
            darkness = mod->getSettingValue<float>("darkness");
        }

        // Strong dark overlay (visible)
        auto overlay = CCLayerColor::create(ccc4(0, 0, 0, static_cast<GLubyte>(200 * darkness)));
        overlay->setZOrder(-5);
        this->addChild(overlay);

        // Replace RobTop logo area with MDC
        if (auto social = this->getChildByID("social-media-menu")) {
            if (auto robtopBtn = social->getChildByID("robtop-logo-button")) {
                robtopBtn->setVisible(false);
            }
        }

        // Big MDC title
        auto mdcLabel = CCLabelBMFont::create("MDC", "bigFont.fnt");
        mdcLabel->setScale(1.4f);
        mdcLabel->setPosition({winSize.width / 2, winSize.height - 40});
        mdcLabel->setColor(ccc3(220, 220, 255));
        mdcLabel->setZOrder(10);
        this->addChild(mdcLabel);

        // Soft flowing pulse on title
        auto pulse = CCSequence::create(
            CCFadeTo::create(1.2f, 180),
            CCFadeTo::create(1.2f, 255),
            nullptr
        );
        mdcLabel->runAction(CCRepeatForever::create(pulse));

        // Flow button (more visible)
        auto btn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
            this,
            menu_selector(FlowMenuLayer::onFlowButton)
        );
        btn->setID("flow-button"_spr);
        btn->setScale(1.1f);

        if (auto menu = this->getChildByID("bottom-menu")) {
            menu->addChild(btn);
            menu->updateLayout();
        }

        // Continuous soft scale pulse on button
        auto btnPulse = CCSequence::create(
            CCScaleTo::create(0.8f, 1.15f),
            CCScaleTo::create(0.8f, 1.0f),
            nullptr
        );
        btn->runAction(CCRepeatForever::create(btnPulse));

        log::info("MDC UI - Flow theme applied to MenuLayer");
        return true;
    }

    void onFlowButton(CCObject*) {
        if (Mod::get()->getSettingValue<bool>("sound-effects")) {
            FMODAudioEngine::sharedEngine()->playEffect("click.ogg");
        }
        FLAlertLayer::create(
            "MDC UI",
            "Flow temalı dark arayüz aktif.\nAyarları Geode mod sayfasından değiştir.",
            "Tamam"
        )->show();
    }
};

// ===== LOADING SCREEN =====
class $modify(FlowLoadingLayer, LoadingLayer) {
    bool init(bool fromReload) {
        if (!LoadingLayer::init(fromReload)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Dark loading background
        auto dark = CCLayerColor::create(ccc4(8, 8, 12, 255));
        dark->setZOrder(-10);
        this->addChild(dark);

        // MDC loading text
        auto loadLabel = CCLabelBMFont::create("MDC", "bigFont.fnt");
        loadLabel->setScale(1.6f);
        loadLabel->setPosition(winSize / 2);
        loadLabel->setColor(ccc3(200, 200, 255));
        loadLabel->setZOrder(5);
        this->addChild(loadLabel);

        // Soft pulse on loading text
        auto pulse = CCSequence::create(
            CCScaleTo::create(0.7f, 1.7f),
            CCScaleTo::create(0.7f, 1.5f),
            nullptr
        );
        loadLabel->runAction(CCRepeatForever::create(pulse));

        log::info("MDC UI - Loading screen themed");
        return true;
    }
};
