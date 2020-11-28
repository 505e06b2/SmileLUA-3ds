local sdmc = os.storageInfo()

print("SD Info:")
print(string.format("Total Size (Gb): %.2f", sdmc["total"]/1024/1024/1024))
print(string.format("Free Space (Gb): %.2f", sdmc["free"]/1024/1024/1024))
print(string.format("Used: %.0f%%", 100-sdmc["free"]/sdmc["total"]*100))
