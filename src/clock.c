void Clock_Init(void) {
    // Set MOSC as system clock
    SYSCTL->RSCLKCFG |= SYSCTL_RSCLKCFG_OSCSRC_MOSC;
    // Set high frequency range
    SYSCTL->MOSCCTL |= SYSCTL_MOSCCTL_OSCRNG;
}