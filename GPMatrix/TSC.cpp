// ----------------------------------------------------------------------------
//  Name:	Clock
//  Desc:	Returns the current TSC.
// ----------------------------------------------------------------------------

__declspec(naked) __int64 Clock() {
	__asm {
		rdtsc
		ret
	}
	// Ignore the "non-void function should return a value" warning.
}
