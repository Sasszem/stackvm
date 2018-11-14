main:
	// Calculate string length
	push &calclen call

	// Counter starts on zero
	push 0
	loop:
		dup
		push &str + load // Index array
		outchar // Print char

		push 1 + // Increment
		dup push &len load = // Compare with len
		push &loop jz // Loop
	push 10 outchar // Print newline
	halt

str:
	"This_is_a_string" // It must not contain spaces
strend:

calclen:
	push &strend
	push &str
	-
	push &len store
ret

len:
	5
