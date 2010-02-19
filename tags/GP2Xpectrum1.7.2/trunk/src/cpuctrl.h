#if !defined(_CPUCTRL_)
#define _CPUCTRL_

void wait_vsync();
unsigned get_pixel_clock_div();
unsigned get_display_clock_div();
void set_display_clock_div(unsigned div);

void cpuctrl_init(); // call this at first

void save_system_regs(); // save some registers
void load_system_regs();

void set_FCLK(unsigned MHZ); // adjust the clock frequency (in Mhz units)

unsigned get_freq_UCLK();
unsigned get_freq_ACLK();
unsigned get_freq_920_CLK();
unsigned get_freq_940_CLK();
unsigned get_freq_DCLK();

void set_920_Div(unsigned short div); /* 0 to 7 divider (freq=FCLK/(1+div)) */
unsigned short get_920_Div();

void set_940_Div(unsigned short div); /* 0 to 7 divider (freq=FCLK/(1+div)) */
unsigned short get_940_Div();

void set_DCLK_Div(unsigned short div); /* 0 to 7 divider (freq=FCLK/(1+div)) */
unsigned short get_DCLK_Div();

unsigned short Disable_Int_920();
unsigned short Disable_Int_940();

void Enable_Int_920(unsigned short flag);
void Enable_Int_940(unsigned short flag);

void Disable_940(); // 940t down

extern volatile unsigned  *arm940code; // memory address of 940t code

void Load_940_code(unsigned *code,int size); // enable 940t, load 940t code  and clock 940t off


void clock_940_off(); // 940t stops
void clock_940_on();  // 940t running

void set_display(int mode); // 1- display on 0-display off

void set_battery_led(int on);

unsigned get_status_UCLK();

unsigned get_status_ACLK();

void set_status_UCLK(unsigned s);

void set_status_ACLK(unsigned s);


#endif
