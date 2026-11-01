#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/FLAlertLayer.hpp>

using namespace geode::prelude;

class $modify(FlowMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto mod = Mod::get();
        float darkness = 0.85f;
        try {
            darkness = mod->getSettingValue<float>("darkness");
        } catch (...) {}

        // Dark overlay for modern black theme
        auto overlay = CCLayerColor::create(ccc4(0, 0, 0, static_cast<GLubyte>(200 * darkness)));
        this->addChild(overlay, -10);

        // Simple wind-like moving lines using labels or sprites (safe fallback)
        if (mod->getSettingValue<bool>("wind-particles")) {
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            for (int i = 0; i < 12; ++i) {
                auto line = CCLabelBMFont::create("—", "chatFont.fnt");
                line->setScale(0.4f);
                line->setOpacity(80);
                line->setColor(ccc3(220, 220, 230));
                float x = (i * 80.0f) - 100.0f;
                float y = winSize.height * 0.3f + (i % 4) * 40.0f;
                line->setPosition({x, y});
                this->addChild(line, -5);
                // Soft flow movement
                auto move = CCMoveBy::create(4.0f + (i % 3), ccp(winSize.width + 200.0f, 0));
                auto reset = CCMoveBy::create(0.0f, ccp(-(winSize.width + 200.0f), 0));
                line->runAction(CCRepeatForever::create(CCSequence::create(move, reset, nullptr)));
            }
        }

        // Soft pulse on bottom menu buttons
        if (auto bottom = this->getChildByID("bottom-menu")) {
            for (auto* child : CCArrayExt<CCNode*>(bottom->getChildren())) {
                auto pulse = CCSequence::create(
                    CCScaleTo::create(1.1f, 1.05f),
                    CCScaleTo::create(1.1f, 1.0f),
                    nullptr
                );
                child->runAction(CCRepeatForever::create(pulse));
            }
        }

        log::info("MDC UI Flow effects applied");
        return true;
    }
};

class $modify(FLAlertLayer) {
    bool init(FLAlertLayerProtocol* p0, char const* title, gd::string desc, char const* btn1, char const* btn2, float width, bool scroll, float height, float textScale) {
        if (!FLAlertLayer::init(p0, title, desc, btn1, btn2, width, scroll, height, textScale)) return false;

        // Darker alerts
        if (auto bg = this->getChildByID("background")) {
            bg->setColor(ccc3(18, 18, 22));
        }
        return true;
    }
};
