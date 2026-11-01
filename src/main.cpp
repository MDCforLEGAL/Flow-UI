#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/FLAlertLayer.hpp>
#include <cocos2d.h>
#include <cmath>

using namespace geode::prelude;
using namespace cocos2d;

class FlowUIManager {
public:
    static float time;
    static CCParticleSystemQuad* windParticles;
    static std::vector<CCNode*> animatedButtons;

    static void updateEffects(float dt) {
        time += dt;
        auto mod = Mod::get();
        if (!mod->getSettingValue<bool>("flow-effects")) return;

        float intensity = mod->getSettingValue<float>("flow-intensity");
        for (auto btn : animatedButtons) {
            if (!btn) continue;
            float offset = std::sin(time * 2.0f + btn->getPositionX() * 0.01f) * 3.0f * intensity;
            auto pos = btn->getPosition();
            btn->setPositionY(pos.y + offset * 0.1f);
        }
    }

    static void playClickSound() {
        if (!Mod::get()->getSettingValue<bool>("sound-effects")) return;
        auto engine = FMODAudioEngine::sharedEngine();
        if (engine) {
            engine->playEffect("click.ogg", 1.0f, 0.0f, 0.8f);
        }
    }

    static void createWindParticles(CCNode* parent) {
        if (!Mod::get()->getSettingValue<bool>("wind-particles")) return;
        if (windParticles) windParticles->removeFromParent();

        windParticles = CCParticleSystemQuad::createWithTotalParticles(80);
        if (!windParticles) return;

        float speed = Mod::get()->getSettingValue<float>("wind-speed");
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        windParticles->setDuration(-1);
        windParticles->setEmitterMode(kCCParticleModeGravity);
        windParticles->setGravity(ccp(120.0f * speed, -20.0f));
        windParticles->setSpeed(40.0f * speed);
        windParticles->setSpeedVar(15.0f);
        windParticles->setAngle(0);
        windParticles->setAngleVar(10);
        windParticles->setPosVar(ccp(winSize.width, 20));
        windParticles->setLife(3.5f);
        windParticles->setLifeVar(1.0f);
        windParticles->setStartSize(2.0f);
        windParticles->setEndSize(0.5f);
        windParticles->setStartColor(ccc4f(1, 1, 1, 0.3f));
        windParticles->setEndColor(ccc4f(1, 1, 1, 0.0f));
        windParticles->setEmissionRate(20.0f * speed);
        windParticles->setPosition(ccp(0, winSize.height * 0.6f));
        windParticles->setBlendAdditive(true);

        parent->addChild(windParticles, -1);
    }
};

float FlowUIManager::time = 0.0f;
CCParticleSystemQuad* FlowUIManager::windParticles = nullptr;
std::vector<CCNode*> FlowUIManager::animatedButtons;

class $modify(FlowMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto mod = Mod::get();
        float darkness = mod->getSettingValue<float>("darkness");

        auto bg = this->getChildByID("main-menu-bg");
        if (bg) {
            bg->setColor(ccc3(static_cast<GLubyte>(20 * (1.0f - darkness + 0.2f)), 
                              static_cast<GLubyte>(20 * (1.0f - darkness + 0.2f)), 
                              static_cast<GLubyte>(25 * (1.0f - darkness + 0.2f))));
            bg->setOpacity(static_cast<GLubyte>(255 * darkness));
        }

        auto overlay = CCLayerColor::create(ccc4(0, 0, 0, static_cast<GLubyte>(180 * darkness)));
        this->addChild(overlay, -2);

        FlowUIManager::createWindParticles(this);

        auto bottomMenu = this->getChildByID("bottom-menu");
        if (bottomMenu) {
            for (auto child : CCArrayExt<CCNode*>(bottomMenu->getChildren())) {
                FlowUIManager::animatedButtons.push_back(child);
                auto glow = CCLayerColor::create(ccc4(255, 255, 255, 30));
                glow->setContentSize(child->getContentSize() * 1.2f);
                glow->setPosition(child->getPosition() - child->getContentSize() * 0.1f);
                glow->setOpacity(40);
                bottomMenu->addChild(glow, -1);
                auto fade = CCFadeTo::create(1.2f, 20);
                auto fadeBack = CCFadeTo::create(1.2f, 60);
                glow->runAction(CCRepeatForever::create(CCSequence::create(fade, fadeBack, nullptr)));
            }
            bottomMenu->updateLayout();
        }

        this->schedule(schedule_selector(FlowMenuLayer::flowUpdate), 0.016f);

        return true;
    }

    void flowUpdate(float dt) {
        FlowUIManager::updateEffects(dt);
    }

    void onMyButton(CCObject* sender) {
        FlowUIManager::playClickSound();
        auto btn = static_cast<CCNode*>(sender);
        auto scaleUp = CCScaleTo::create(0.08f, 1.15f);
        auto scaleDown = CCScaleTo::create(0.12f, 1.0f);
        btn->runAction(CCSequence::create(scaleUp, scaleDown, nullptr));

        FLAlertLayer::create(
            "MDC UI",
            "Flow temalı arayüz aktif! Ayarları Geode mod menüsünden özelleştir.",
            "Tamam"
        )->show();
    }
};

class $modify(FLAlertLayer) {
    bool init(FLAlertLayerProtocol* p0, char const* title, gd::string desc, char const* btn1, char const* btn2, float width, bool scroll, float height, float textScale) {
        if (!FLAlertLayer::init(p0, title, desc, btn1, btn2, width, scroll, height, textScale)) return false;

        float darkness = Mod::get()->getSettingValue<float>("darkness");
        if (auto bg = this->getChildByID("background")) {
            bg->setColor(ccc3(15, 15, 18));
            bg->setOpacity(static_cast<GLubyte>(230 * darkness));
        }
        return true;
    }
};

$execute {
    log::info("MDC UI (Flow) mod loaded successfully.");
}
