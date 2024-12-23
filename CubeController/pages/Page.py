from src import DisplayManager

class Page:
    def __init__(self, page_manager: "PageManager", display_manager: DisplayManager):
        self.page_manager = page_manager
        self.display_manager = display_manager
    
    def show(self):
        raise NotImplementedError("Subclasses must implement show().")

    def increment(self, delta: int):
        raise NotImplementedError("Subclasses must implement increment().")

    def press(self):
        raise NotImplementedError("Subclasses must implement press().")
