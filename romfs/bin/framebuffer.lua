--[[

	The standard "rotated" framebuffer (top/bottom) start in the top left of the screen and go from left to right
	The raw framebuffer topRaw/bottomRaw start in the bottom left and go from bottom to top

--]]

local red = tonumber("1111100000000000", 2) --easier to get rgb565 this way
local blue = tonumber("0000000000011111", 2)

function checkColour(colour)
	if colour == red then return "Red" end
	if colour == blue then return "Blue" end
	return "FAILED"
end

io.write("\x1b[2J")
io.writeBottom("\x1b[2J")

--left
for i=1, 240*10, 1 do
	io.framebuffer.topRaw[i] = red
end

--right
for i=#io.framebuffer.topRaw, (#io.framebuffer.topRaw-240*10), -1 do
	io.framebuffer.topRaw[i] = red
end

--top
for i=1, 400*10, 1 do
	io.framebuffer.top[i] = blue
end

--bottom
for i=#io.framebuffer.top, (#io.framebuffer.top-400*10), -1 do
	io.framebuffer.top[i] = blue
end

io.writeBottom(
	"Tests:\n\n",
	" Top Left:      ", checkColour(io.framebuffer.top[1]), "\n",
	" Middle Left:   ", checkColour(io.framebuffer.topRaw[120]), "\n",
	" Bottom Left:   ", checkColour(io.framebuffer.topRaw[1]), "\n",
	"\n",
	" Middle Top:    ", checkColour(io.framebuffer.top[120]), "\n",
	" Middle Bottom: ", checkColour(io.framebuffer.top[#io.framebuffer.top-120]), "\n",
	"\n",
	" Top Right:     ", checkColour(io.framebuffer.top[400]), "\n", --could also use io.framebuffer.topRaw[#io.framebuffer.topRaw]
	" Middle Right:  ", checkColour(io.framebuffer.topRaw[#io.framebuffer.top-120]), "\n",
	" Bottom Right:  ", checkColour(io.framebuffer.top[#io.framebuffer.top]), "\n",

	"\n\nPress B to exit"
)

--infinite wait - safe since os.mainLoop is called every loop
while os.mainLoop() and not io.readControls()["B"] do end

io.write("\x1b[2J")
io.writeBottom("\x1b[2J")
