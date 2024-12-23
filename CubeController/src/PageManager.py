class PageManager:
    def __init__(self):
        self.page: "Page | None" = None
    
    def setPage(self, page: "Page"):
        print(f"Setting page to {page.__class__.__name__}")
        self.page = page
        self.page.show()
    
    def increment(self, delta: int):
        if self.page is not None:
            self.page.increment(delta)
    
    def press(self):
        if self.page is not None:
            self.page.press()