print("Loaded from SDMC, press A to exit")

while os.mainLoop() and not io.readControls()["A"] do end
