#pragma once

#include "../core/Common.h"
#include "Letter.h"
#include <memory>

namespace omegarace {

class PauseMenu : Common {
public:
    enum MENU_OPTION {
        RESUME = 0,
        QUIT = 1,
        NUM_OPTIONS = 2
    };

    PauseMenu();
    ~PauseMenu();

    void initialize();
    void draw();
    void update();
    
    // Input handling
    void handleUp();
    void handleDown();
    void handleSelect(); // Fire/Space key
    
    // Menu state
    bool isVisible() const { return m_IsVisible; }
    void show();
    void hide();
    void toggle();
    
    // Get selected option when player presses select
    MENU_OPTION getSelectedOption() const { return m_SelectedOption; }
    
    // Reset selection to default
    void resetSelection();

private:
    void updateMenuText();
    void drawBackground();
    void drawMenuOptions();
    
    bool m_IsVisible;
    MENU_OPTION m_SelectedOption;
    
    // Menu text
    std::string m_MenuText[NUM_OPTIONS];
    std::string m_TitleText;
    
    // Positions
    Vector2i m_TitlePosition;
    Vector2i m_MenuStartPosition;
    int m_MenuSpacing;
    
    // Colors
    Color m_NormalColor;
    Color m_SelectedColor;
    Color m_BackgroundColor;
    
    // Text rendering
    std::unique_ptr<Letter> pLetter;
    
    // Menu dimensions
    int m_MenuWidth;
    int m_MenuHeight;
    Vector2i m_MenuPosition;
};

} // namespace omegarace
