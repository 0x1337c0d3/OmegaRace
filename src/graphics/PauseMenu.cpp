#include "PauseMenu.h"
#include "Window.h"

namespace omegarace {

PauseMenu::PauseMenu() {
    m_IsVisible = false;
    m_SelectedOption = RESUME;
    
    // Initialize menu text
    m_TitleText = "GAME PAUSED";
    m_MenuText[RESUME] = "RESUME";
    m_MenuText[QUIT] = "QUIT";
    
    // Initialize colors
    m_NormalColor.red = 255;
    m_NormalColor.green = 255;
    m_NormalColor.blue = 255;
    m_NormalColor.alpha = 255;
    
    m_SelectedColor.red = 0;
    m_SelectedColor.green = 255;
    m_SelectedColor.blue = 255;
    m_SelectedColor.alpha = 255;
    
    // Semi-transparent dark background
    m_BackgroundColor.red = 0;
    m_BackgroundColor.green = 0;
    m_BackgroundColor.blue = 0;
    m_BackgroundColor.alpha = 180;
    
    // Menu spacing
    m_MenuSpacing = 40;
    
    // Create letter renderer
    pLetter = std::make_unique<Letter>();
}

PauseMenu::~PauseMenu() {
}

void PauseMenu::initialize() {
    Vector2i windowSize = Window::GetWindowSize();
    
    // Calculate menu dimensions
    m_MenuWidth = 300;
    m_MenuHeight = 200;
    
    // Center the menu on screen
    m_MenuPosition.x = (windowSize.x - m_MenuWidth) / 2;
    m_MenuPosition.y = (windowSize.y - m_MenuHeight) / 2;
    
    // Position title
    m_TitlePosition.x = windowSize.x / 2 - 80; // Approximate center for "GAME PAUSED"
    m_TitlePosition.y = m_MenuPosition.y + 40;
    
    // Position menu options
    m_MenuStartPosition.x = windowSize.x / 2 - 40; // Approximate center for menu options
    m_MenuStartPosition.y = m_TitlePosition.y + 60;
    
    // Initialize letter renderer
    pLetter->initializeLetterLine();
    pLetter->setColor(m_NormalColor);
}

void PauseMenu::draw() {
    if (!m_IsVisible) {
        return;
    }
    
    std::cout << "PauseMenu::draw() called - menu is visible" << std::endl;
    
    drawBackground();
    drawMenuOptions();
}

void PauseMenu::update() {
    // Nothing to update currently - menu is static
}

void PauseMenu::handleUp() {
    if (!m_IsVisible) {
        return;
    }
    
    // Move selection up (with wrapping)
    if (m_SelectedOption == RESUME) {
        m_SelectedOption = QUIT;
    } else {
        m_SelectedOption = static_cast<MENU_OPTION>(m_SelectedOption - 1);
    }
}

void PauseMenu::handleDown() {
    if (!m_IsVisible) {
        return;
    }
    
    // Move selection down (with wrapping)
    if (m_SelectedOption == QUIT) {
        m_SelectedOption = RESUME;
    } else {
        m_SelectedOption = static_cast<MENU_OPTION>(m_SelectedOption + 1);
    }
}

void PauseMenu::handleSelect() {
    if (!m_IsVisible) {
        return;
    }
    
    // The caller will check getSelectedOption() to determine action
    // For now, just hide the menu - the game controller will handle the logic
    hide();
}

void PauseMenu::show() {
    m_IsVisible = true;
    resetSelection();
}

void PauseMenu::hide() {
    m_IsVisible = false;
}

void PauseMenu::toggle() {
    if (m_IsVisible) {
        hide();
    } else {
        show();
    }
}

void PauseMenu::resetSelection() {
    m_SelectedOption = RESUME;
}

void PauseMenu::drawBackground() {
    // Draw semi-transparent background overlay
    Rectangle background;
    background.x = 0;
    background.y = 0;
    background.width = Window::GetWindowSize().x;
    background.height = Window::GetWindowSize().y;
    
    Window::DrawRect(&background, m_BackgroundColor);
    
    // Draw menu panel background (slightly lighter)
    Rectangle menuPanel;
    menuPanel.x = m_MenuPosition.x;
    menuPanel.y = m_MenuPosition.y;
    menuPanel.width = m_MenuWidth;
    menuPanel.height = m_MenuHeight;
    
    Color panelColor;
    panelColor.red = 40;
    panelColor.green = 40;
    panelColor.blue = 60;
    panelColor.alpha = 220;
    
    Window::DrawRect(&menuPanel, panelColor);
    
    // Draw menu panel border
    Color borderColor;
    borderColor.red = 100;
    borderColor.green = 150;
    borderColor.blue = 255;
    borderColor.alpha = 255;
    
    // Draw border lines
    Line borderLines[4];
    
    // Top border
    borderLines[0].start = Vector2i(menuPanel.x, menuPanel.y);
    borderLines[0].end = Vector2i(menuPanel.x + menuPanel.width, menuPanel.y);
    
    // Bottom border
    borderLines[1].start = Vector2i(menuPanel.x, menuPanel.y + menuPanel.height);
    borderLines[1].end = Vector2i(menuPanel.x + menuPanel.width, menuPanel.y + menuPanel.height);
    
    // Left border
    borderLines[2].start = Vector2i(menuPanel.x, menuPanel.y);
    borderLines[2].end = Vector2i(menuPanel.x, menuPanel.y + menuPanel.height);
    
    // Right border
    borderLines[3].start = Vector2i(menuPanel.x + menuPanel.width, menuPanel.y);
    borderLines[3].end = Vector2i(menuPanel.x + menuPanel.width, menuPanel.y + menuPanel.height);
    
    for (int i = 0; i < 4; i++) {
        Window::DrawLine(&borderLines[i], borderColor);
    }
}

void PauseMenu::drawMenuOptions() {
    int titleSize = 12;
    int menuSize = 10;
    
    // Draw title
    pLetter->setColor(m_SelectedColor); // Use highlight color for title
    pLetter->processString(m_TitleText, m_TitlePosition, titleSize);
    
    // Draw menu options
    Vector2i currentPosition = m_MenuStartPosition;
    
    for (int i = 0; i < NUM_OPTIONS; i++) {
        // Set color based on selection
        if (i == m_SelectedOption) {
            pLetter->setColor(m_SelectedColor);
        } else {
            pLetter->setColor(m_NormalColor);
        }
        
        // Draw option text
        pLetter->processString(m_MenuText[i], currentPosition, menuSize);
        
        // Move to next option position
        currentPosition.y += m_MenuSpacing;
    }
}

} // namespace omegarace
