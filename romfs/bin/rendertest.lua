while os.renderLoop() do
	local controls = io.readControls()
	if controls["B"] then
		break
	elseif controls["A"] then
		io.writeBottom("lole")
	end
end
