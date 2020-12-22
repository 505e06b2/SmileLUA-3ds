io.writeBottom("\x1b[2J", "Hold Start to exit")

local controls = io.readControls()

while os.mainLoop() and not controls["Start"] do
	controls = io.readControls()
	local x, y, z = io.readCirclePad()
	print(string.format("Circlepad - X: %d, Y: %d", x, y))

	x, y = io.readTouchscreen()
	if x == nil then
		x, y = 0, 0
	end
	print(string.format("Touch - X: %d, Y: %d", x, y))

	x, y, z = io.readGyroscope()
	print(string.format("Gyro - Roll: %d, Pitch: %d, Yaw: %d", x, y, z))

	x, y, z = io.readAccelerometer() --doesn't seem to work?
	print(string.format("Accelerometer - X: %d, Y: %d, Z: %d", x, y, z))

	local pressed = 0
	for key, value in pairs(controls) do
		if value then
			pressed = pressed + 1
		end
	end
	print(string.format("Keys pressed - %d", pressed))
	print("")
	os.sleep(0.16)
end
