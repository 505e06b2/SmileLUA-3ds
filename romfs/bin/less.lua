local args = {...}

local function printHelp()
	print("Usage: less FILE")
end

local term_info = {width=40, height=30, size=50*30} --chars
local lines = {}

if #args >= 1 then
	local source = io.open(args[1], "r")
	if source then
		local text = source:read("*line")
		while text do
			x = text:gsub("\t", "  ") --tab to space
			--x = x:gsub("\\", "\\") --escape backslashes -> NOT NEEDED
			table.insert(lines, x) --turn tabs into X spaces - use os.terminal.tab_size in the future
			text = source:read("*line")
		end

		if #lines == 0 then
			table.insert(lines, "")
		end
		source:close()
	else
		shell.printError("less: " .. args[1] .. ": could not be opened")
		return
	end
else
	printHelp()
	return
end

local current_line = 1
function generateLines(new_index)
	if new_index then
		current_line = new_index + current_line
		if current_line < 1 then current_line = 1
		elseif current_line > #lines then current_line = #lines end
	end

	local ret = {}
	local y = 1
	local height = term_info.height * 2
	for i=current_line,#lines,1 do
		local text = lines[i]
		local prefix = string.format("\x1b[33;1m%-3d\x1b[31;1m|\x1b[0m", i)
		local prefix_len = 4 --take into account for ansi codes
		local width = term_info.width-prefix_len-2 --
		for j=1,#text,width+1 do
			table.insert(ret, prefix .. text:sub(j,j+width))
			prefix = string.rep(" ", prefix_len) --same length as line count
			y = y + 1
			if y >= height then return ret end --*2 for 2 screen
		end
	end
	return ret
end

function printText(text_lines)
	io.write("\x1b[2J")
	io.writeBottom("\x1b[2J")
	for i=1,#text_lines,1 do
		if i <= term_info.height then
			io.write("\n     ", text_lines[i])
		else
			io.writeBottom("\n", text_lines[i])
		end
	end
end

printText(generateLines())
local controls = io.readControls()
local cooldown = false
while os.mainLoop() and not controls["Start"] do
	controls = io.readControls()
	if cooldown and (
		controls["Up"] or controls["Down"] or
		controls["L"] or controls["R"] or
		controls["ZL"] or controls["ZR"]
	) then --skip
	elseif controls["Up"] then
		cooldown = true
		printText(generateLines(-1))
	elseif controls["Down"] then
		cooldown = true
		printText(generateLines(1))
	elseif controls["L"] then
		cooldown = true
		printText(generateLines(-5))
	elseif controls["ZL"] then
		cooldown = true
		printText(generateLines(-10))
	elseif controls["R"] then
		cooldown = true
		printText(generateLines(5))
	elseif controls["ZR"] then
		cooldown = true
		printText(generateLines(10))
	else
		cooldown = false
	end
end

io.write("\x1b[2J")
io.writeBottom("\x1b[2J")
