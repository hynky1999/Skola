import Simulated7SegmentDisplay

# If you would like to experiment with different segment numbering layouts,
# simply rename (reorder) files "OnSegment0.png", "OnSegment1.png", ..., "OnSegment6.png",
# so that a file with a specific segment number holds correct ON segment image for your layout.

displaySet = Simulated7SegmentDisplay.DisplaySet([ "One digit\ndisplay" ])
output = Simulated7SegmentDisplay.OutputTo7SegmentDisplay(displaySet, 0)

while True:
    number = int(input("Enter hex number from range 00 to 7F (-1 to END): "), base=16)
    if number == -1:
        break

    output.outputChanged(number)

displaySet.close()

# NOTE: Program will display "Tcl_AsyncDelete: async handler deleted by the wrong thread" message
# when ending - this is expected behavior. Implementation of display visualization had to be
# simplified in order to have the main program simple enough to be easily readable be programming beginners.
# In order to do that, we are using TkInter knowningly incorrectly (not from the main thread), which results
# in this bug - however it has no influence on the main program behavior.