#include "pages/ListMenuPage.h"
#include "DisplayManager.h"

void ListMenuPage::show() {
    {
        DisplayScope scope;
        int y = 20;
        int lineHeight = DisplayManager::getLineHeight();
        for(int i = 0; i < items.size(); i++) {
            String name = items[i].name;
            DisplayManager::Style style = i == selectedItem ? DisplayManager::INVERTED : DisplayManager::NORMAL;
            DisplayManager::print(name.c_str(), 0, y, style);
            y += lineHeight + 5;
        }
    }
}

void ListMenuPage::increment(int delta) {
    selectedItem = (selectedItem + delta + items.size()) % items.size();
    show();
}

std::optional<PagesEnum> ListMenuPage::press() {
    Serial.printf("ListMenuPage pressed selected item %d\n", selectedItem);
    if(selectedItem < 0 || selectedItem >= items.size()) {
        Serial.println("Invalid selected item");
        return std::nullopt;
    }
    PagesEnum page = items[selectedItem].page;
    Serial.printf("Selected menu item: %s with enum value: %d\n", 
                 items[selectedItem].name.c_str(), 
                 static_cast<int>(page));
    return page;
}