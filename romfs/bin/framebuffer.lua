local border_lines = 240/3

local colours = {0xff00, 0x0ff0, 0x00ff}

for total=1,3,1 do
	local y_offset = border_lines*400*(total-1)
	for i=1,border_lines*400,1 do
	   io.framebuffer.top[i+y_offset] = colours[total]
	end
end
