from tkinter import *
import threading
import time

class OutputTo7SegmentDisplay:
    def __init__(self, displaySet, displayIndex):
        self.displaySet = displaySet
        self.displayIndex = displayIndex
        return

    def outputChanged(self, outputValue):
        self.displaySet.setDisplayState(self.displayIndex, outputValue & 0x7F)
        return

class Display:
    def __init__(self, name):
        self.name = name
        self.state = 0x00       # Bit 7 = unused, bit 6 = segment 6, ..., bit 0 = segment 0
                                # Bit value 0 = segment is OFF, bit value 1 = segment is ON (is lit)

        self.isDirty = False    # True, if state changed since last paint of this display
        
        self.segments = []      # Will contain 1 canvas placed image for every display segment -> 7 items in total

REFRESH_RATE = 50   # Display painting refresh rate in miliseconds

class DisplaySet:
    def __init__(self, displayNames):
        self.displays = []
        for name in displayNames:
            self.displays += [ Display(name) ]

        self.quitRequested = False
        t = threading.Thread(target=self.uiThread)
        t.start()

    def close(self):
        self.quitRequested = True
    
    def setDisplayState(self, displayIndex, newState):
        display = self.displays[displayIndex]
        display.state = newState
        display.isDirty = True

    def uiThread(self):
        self.window = Tk()
        self.window.tk.call("tk", "scaling", 1.0)
        self.window.title("Simulated 7-segment display view")
        self.window.geometry("650x250")

        self.canvas = Canvas(self.window, width=2000, height=2000)
        self.canvas.configure(bg="white")
        self.canvas.pack()

        digitImage = PhotoImage(file="AllSegmentsOff.png")
        digitOverlayImage = PhotoImage(file="AllSegmentsOverlay.png")
        segmentImages = []
        for i in range(0, 7):
            fileName = f"OnSegment{i}.png"
            segmentImages += [ PhotoImage(file=fileName) ]

        x = 50
        y = 50
        for display in self.displays:
            self.canvas.create_image(x, y, image=digitImage, anchor=NW)
            self.canvas.create_text(x + digitImage.width() / 2, y + digitImage.height() + 5, text=display.name, anchor=N)
            
            for segmentImage in segmentImages:
                display.segments += [ self.canvas.create_image(x, y, image=segmentImage, anchor=NW, state="hidden") ]

            self.canvas.create_image(x, y, image=digitOverlayImage, anchor=NW)

            x += digitImage.width() + 5

        self.window.after(REFRESH_RATE, self.displayPaintTask)    # Plan execution of 1st display refresh paint
        mainloop()
    
        self.window.quit()
        quit()

    def displayPaintTask(self):
        if self.quitRequested:
            self.window.destroy()   # Close main window
            return

        for display in self.displays:
            if display.isDirty:
                for segmentIndex in range(0, 7):
                    if (display.state & (1 << segmentIndex)) == 0:
                        newState = "hidden"
                    else:
                        newState = "normal"
                    self.canvas.itemconfigure(display.segments[segmentIndex], state=newState)
                display.isDirty = False

        self.window.after(REFRESH_RATE, self.displayPaintTask)  # Plan execution of next display refresh paint
