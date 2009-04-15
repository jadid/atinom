/*
	idle hook
	barangkali perlu
	
	15 april 2009
	furkan jadid, daun biru engineering
	
	*/

extern unsigned int loop_idle;
	
void vApplicationIdleHook( void )
{
	loop_idle++;	
}
