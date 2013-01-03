
	db	0
	partn	2
	adrs	_start1
	adrsm	_start2
	adrsm	0
	adrsm	0
	adrsm	0


_start1:
	ino 003 012 gat 20 tmp 60

	leg c40 lof c40
	leg c40 lof c40
	end

	ppa 0 128	; =256 1step ‚Å‚P”¼‰¹ˆÚ“®
	c30 pon c40 pof
	012 pon c30 c40 c30 c40
	end

_start2:
	repeat 20
	002 c50
	next
	end
