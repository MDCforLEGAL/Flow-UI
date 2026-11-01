#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/async.hpp>

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

        // Strong dark overlay
        auto overlay = CCLayerColor::create(ccc4(0, 0, 0, static_cast<GLubyte>(200 * darkness)));
        overlay->setZOrder(-5);
        this->addChild(overlay);

        // Hide RobTop logo button
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

        auto pulse = CCSequence::create(
            CCFadeTo::create(1.2f, 180),
            CCFadeTo::create(1.2f, 255),
            nullptr
        );
        mdcLabel->runAction(CCRepeatForever::create(pulse));

        // Flow button
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

        auto btnPulse = CCSequence::create(
            CCScaleTo::create(0.8f, 1.15f),
            CCScaleTo::create(0.8f, 1.0f),
            nullptr
        );
        btn->runAction(CCRepeatForever::create(btnPulse));

        // Auto update check
        if (mod->getSettingValue<bool>("check-updates")) {
            this->checkForUpdate();
        }

        log::info("MDC UI v1.0.1 - Flow theme applied");
        return true;
    }

    void checkForUpdate() {
        async::spawn(
            web::WebRequest().get("https://api.github.com/repos/MDCforLEGAL/Flow-UI/releases/latest"),
            [](web::WebResponse resp) {
                if (!resp.ok()) return;

                auto jsonRes = resp.json();
                if (!jsonRes.isOk()) return;
                auto json = jsonRes.unwrap();

                auto tagRes = json.get("tag_name");
                if (!tagRes.isOk()) return;

                std::string latest = tagRes.unwrap().asString().unwrapOr("");
                if (latest.starts_with("v")) latest = latest.substr(1);

                std::string current = Mod::get()->getVersion().toDEPRECATEDString();
                if (latest != current && !latest.empty()) {
                    FLAlertLayer::create(
                        "MDC UI Güncelleme",
                        fmt::format("Yeni sürüm bulundu: v{}\nGitHub Releases'ten indirip yükle.", latest),
                        "Tamam"
                    )->show();
                }
            }
        );
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

        auto dark = CCLayerColor::create(ccc4(8, 8, 12, 255));
        dark->setZOrder(-10);
        this->addChild(dark);

        auto loadLabel = CCLabelBMFont::create("MDC", "bigFont.fnt");
        loadLabel->setScale(1.6f);
        loadLabel->setPosition(winSize / 2);
        loadLabel->setColor(ccc3(200, 200, 255));
        loadLabel->setZOrder(5);
        this->addChild(loadLabel);

        auto pulse = CCSequence::create(
            CCScaleTo::create(0.7f, 1.7f),
            CCScaleTo::create(0.7f, 1.5f),
            nullptr
        );
        loadLabel->runAction(CCRepeatForever::create(pulse));

        return true;
    }
};
