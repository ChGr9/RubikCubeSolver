from src import DisplayManager
from pages import Page

class ListMenuPage(Page):
    def __init__(self, page_manager: "PageManager", display_manager: DisplayManager):
        super().__init__(page_manager, display_manager)
        self.items = [
            "Manual setting",
            "Face detection",
            "Solve Cube"
        ]
        self.selected_index = 0
    
    def show(self):
        self.display_manager.clear()
        for i, item in enumerate(self.items):
            y = 5 + i * 20
            self.display_manager.write(f"{'>' if i == self.selected_index else ' '}{item}", y)
            
        print("Showing List")
        self.display_manager.show()
    
    def increment(self, delta: int):
        self.selected_index = (self.selected_index + delta) % len(self.items)
        self.show()
    
    def press(self):
        print(f"Pressed {self.items[self.selected_index]}")